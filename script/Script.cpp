/*
 *  Script.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Aug 23 2006.
 *  Copyright (c) 2006. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <list>
#include <stack>
#include <cassert>
#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"

#include "Game.h"
#include "GameClock.h"
#include "Effect.h"
#include "MsgScroll.h"
#include "Player.h"
#include "Party.h"
#include "ActorManager.h"
#include "TileManager.h"
#include "Actor.h"
#include "Weather.h"
#include "UseCode.h"
#include "SoundManager.h"
#include "Console.h"

#include "Script.h"
#include "ScriptActor.h"
#include "ScriptCutscene.h"
#include "Magic.h"

#include <math.h>

extern "C"
{
#include "lualib.h"
}

extern bool nscript_new_actor_var(lua_State *L, uint16 actor_num);

struct ScriptObjRef
{
   uint16 refcount;
   iAVLKey key;

   ScriptObjRef()
   {
      refcount = 0;
   };
};

static iAVLTree *script_obj_list;

static iAVLKey get_iAVLKey(const void *item)
{
   return ((ScriptObjRef *)item)->key;
}

static NuvieIO *g_objlist_file = NULL;

// used for garbage collection.
//returns current object reference count. Or -1 on error.
static sint32 nscript_inc_obj_ref_count(Obj *obj);
static sint32 nscript_dec_obj_ref_count(Obj *obj);

bool nscript_get_location_from_args(lua_State *L, uint16 *x, uint16 *y, uint8 *z, int lua_stack_offset=1);
Obj *nscript_get_obj_from_args(lua_State *L, int lua_stack_offset);
extern Actor *nscript_get_actor_from_args(lua_State *L, int lua_stack_offset=1);

void nscript_new_obj_var(lua_State *L, Obj *obj);

inline bool nscript_obj_init_from_obj(lua_State *L, Obj *dst_obj);
inline bool nscript_obj_init_from_args(lua_State *L, int nargs, Obj *s_obj);
static int nscript_obj_new(lua_State *L);
int nscript_obj_new(lua_State *L, Obj *obj);
static int nscript_obj_gc(lua_State *L);
static int nscript_obj_get(lua_State *L);
static int nscript_obj_set(lua_State *L);
static int nscript_obj_movetomap(lua_State *L);
static int nscript_obj_movetoinv(lua_State *L);
static int nscript_obj_movetocont(lua_State *L);
static int nscript_obj_use(lua_State *L);
static int nscript_obj_removefromengine(lua_State *L);

static int nscript_container_remove_obj(lua_State *L);

static const struct luaL_Reg nscript_objlib_f[] =
{
   { "new", nscript_obj_new },
   { "moveToMap", nscript_obj_movetomap },
   { "moveToInv", nscript_obj_movetoinv },
   { "moveToCont", nscript_obj_movetocont },
   { "removeFromCont", nscript_container_remove_obj },
   { "removeFromEngine", nscript_obj_removefromengine },
   { "use", nscript_obj_use },

   { NULL, NULL }
};
static const struct luaL_Reg nscript_objlib_m[] =
{
   { "__index", nscript_obj_get },
   { "__newindex", nscript_obj_set },
   { "__gc", nscript_obj_gc },
   { NULL, NULL }
};

static int nscript_u6link_gc(lua_State *L);

static const struct luaL_Reg nscript_u6linklib_m[] =
{
   { "__gc", nscript_u6link_gc },
   { NULL, NULL }
};

static int nscript_u6link_recursive_gc(lua_State *L);

static const struct luaL_Reg nscript_u6linkrecursivelib_m[] =
{
   { "__gc", nscript_u6link_recursive_gc },
   { NULL, NULL }
};

static int nscript_print(lua_State *L);
static int nscript_display_prompt(lua_State *L);
//no longer used -- static int nscript_get_target(lua_State *L);
static int nscript_load(lua_State *L);

static int nscript_config_get_boolean_value(lua_State *L);

static int nscript_objlist_seek(lua_State *L);
static int nscript_objlist_read2(lua_State *L);
static int nscript_objlist_write2(lua_State *L);

static int nscript_player_get_location(lua_State *L);
static int nscript_player_get_karma(lua_State *L);
static int nscript_player_set_karma(lua_State *L);
static int nscript_player_dec_alcohol(lua_State *L);
static int nscript_player_move(lua_State *L);
static int nscript_player_set_actor(lua_State *L);

static int nscript_party_is_in_combat_mode(lua_State *L);
static int nscript_party_set_combat_mode(lua_State *L);
static int nscript_party_set_party_mode(lua_State *L);
static int nscript_party_move(lua_State *L);
static int nscript_party_get_size(lua_State *L);
static int nscript_party_get_member(lua_State *L);
static int nscript_party_update_leader(lua_State *L);
static int nscript_party_resurrect_dead_members(lua_State *L);
static int nscript_party_exit_vehicle(lua_State *L);
static int nscript_party_dismount_from_horses(lua_State *L);


static int nscript_timer_set(lua_State *L);
static int nscript_timer_get(lua_State *L);
static int nscript_timer_update_all(lua_State *L);

static int nscript_clock_get_minute(lua_State *L);
static int nscript_clock_get_hour(lua_State *L);
static int nscript_clock_inc(lua_State *L);

static int nscript_wind_set(lua_State *L);
static int nscript_wind_get(lua_State *L);

static int nscript_input_select(lua_State *L);
static int nscript_input_select_integer(lua_State *L);

//obj manager
static int nscript_objs_at_loc(lua_State *L);
static int nscript_map_get_obj(lua_State *L);
static int nscript_map_remove_obj(lua_State *L);
static int nscript_map_is_water(lua_State *L);
static int nscript_map_is_on_screen(lua_State *L);
static int nscript_map_get_impedence(lua_State *L);
static int nscript_map_get_dmg_tile_num(lua_State *L);
static int nscript_map_line_test(lua_State *L);
static int nscript_map_line_hit_check(lua_State *L);

static int nscript_map_can_put_actor(lua_State *L);
static int nscript_map_can_put_obj(lua_State *L);

static int nscript_tile_get_flag(lua_State *L);

//Misc
static int nscript_new_hit_entities_tbl_var(lua_State *L, ProjectileEffect *effect);
static int nscript_quake_start(lua_State *L);
static int nscript_explosion_start(lua_State *L);
static int nscript_projectile_anim(lua_State *L);
static int nscript_projectile_anim_multi(lua_State *L);
static int nscript_hit_anim(lua_State *L);
static int nscript_usecode_look(lua_State *L);

static int nscript_fade_out(lua_State *L);
static int nscript_fade_in(lua_State *L);
static int nscript_fade_tile(lua_State *L);
static int nscript_black_fade_obj(lua_State *L);

static int nscript_xor_effect(lua_State *L);
static int nscript_xray_effect(lua_State *L);

static int nscript_peer_effect(lua_State *L);
static int nscript_wing_strike_effect(lua_State *L);
static int nscript_hail_storm_effect(lua_State *L);
static int nscript_wizard_eye_effect(lua_State *L);

static int nscript_play_sfx(lua_State *L);
static int nscript_is_god_mode_enabled(lua_State *L);
static int nscript_set_armageddon(lua_State *L);

//Iterators
int nscript_u6llist_iter(lua_State *L);
int nscript_u6llist_iter_recursive(lua_State *L);
int nscript_party_iter(lua_State *L);

static int nscript_party(lua_State *L);
static int nscript_container(lua_State *L);
int nscript_init_u6link_iter(lua_State *L, U6LList *list, bool is_recursive);

static int nscript_find_obj(lua_State *L);

Script *Script::script = NULL;

static int lua_error_handler(lua_State *L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);
	return 1;
}

static bool get_tbl_field_uint16(lua_State *L, const char *index, uint16 *field)
{
   lua_pushstring(L, index);
   lua_gettable(L, -2);

   if(!lua_isnumber(L, -1))
      return false;

   *field = (uint16)lua_tonumber(L, -1);
   lua_pop(L, 1);
   return true;
}

static bool get_tbl_field_uint8(lua_State *L, const char *index, uint8 *field)
{
   lua_pushstring(L, index);
   lua_gettable(L, -2);

   if(!lua_isnumber(L, -1))
      return false;

   *field = (uint8)lua_tonumber(L, -1);
   lua_pop(L, 1);
   return true;
}

static bool get_tbl_field_string(lua_State *L, const char *index, char *field, uint16 max_len)
{
   lua_pushstring(L, index);
   lua_gettable(L, -2);

   if(!lua_isstring(L, -1))
      return false;

   size_t size;
   const char *string = lua_tolstring(L, -1, &size);
   if(size > max_len)
	   size = max_len;

   memcpy(field, string, size);
   field[size] = '\0';

   lua_pop(L, 1);
   return true;
}

uint8 ScriptThread::resume_with_location(MapCoord loc)
{
   lua_newtable(L);
   lua_pushstring(L, "x");
   lua_pushinteger(L, loc.x);
   lua_settable(L, -3);

   lua_pushstring(L, "y");
   lua_pushinteger(L, loc.y);
   lua_settable(L, -3);

   lua_pushstring(L, "z");
   lua_pushinteger(L, loc.z);
   lua_settable(L, -3);

   return resume(1);
}

uint8 ScriptThread::resume_with_direction(uint8 dir)
{
   lua_pushinteger(L, dir);

   return resume(1);
}

uint8 ScriptThread::resume_with_spell_num(uint8 spell_num)
{
   lua_pushinteger(L, spell_num);

   return resume(1);
}

uint8 ScriptThread::resume_with_obj(Obj *obj)
{
	if(obj)
	{
		nscript_new_obj_var(L, obj);
		return resume(1);
	}

	return resume(0);
}

uint8 ScriptThread::resume_with_nil()
{
	return resume(0);
}

uint8 ScriptThread::resume(int narg)
{
   const char *s;
   int ret = lua_resume(L, narg);

   if(ret == 0)
   {
      lua_gc(L, LUA_GCCOLLECT, 0); //FIXME! How often should we collect the garbage?
      return NUVIE_SCRIPT_FINISHED;
   }

   if(ret == LUA_YIELD)
   {
      if(lua_gettop(L) >= 1)
      {
         s = lua_tostring(L, 1);
         if(s)
         {
            if(!strcmp(s, "target"))
               return NUVIE_SCRIPT_GET_TARGET;

            if(!strcmp(s, "dir"))
               return NUVIE_SCRIPT_GET_DIRECTION;

            if(!strcmp(s, "spell"))
               return NUVIE_SCRIPT_GET_SPELL;

            if(!strcmp(s, "inv_obj"))
            {
            	Actor *actor = nscript_get_actor_from_args(L, 2);
            	data = actor->get_actor_num();
            	return NUVIE_SCRIPT_GET_INV_OBJ;
            }

            if(!strcmp(s, "obj"))
                return NUVIE_SCRIPT_GET_OBJ;

            if(!strcmp(s, "talk"))
            {
            	Actor *actor = nscript_get_actor_from_args(L, 2);
            	data = actor->get_actor_num();
            	return NUVIE_SCRIPT_TALK_TO_ACTOR;
            }

            if(!strcmp(s, "adv_game_time"))
            {
               if(lua_gettop(L) < 2)
                  data = 0;
               data = lua_tointeger(L, 2);
               return NUVIE_SCRIPT_ADVANCE_GAME_TIME;
            }
         }
      }
   }

   DEBUG(0, LEVEL_ERROR, "%s\n", lua_tostring(L, lua_gettop(L)));

   return NUVIE_SCRIPT_ERROR;
}


Script::Script(Configuration *cfg, GUI *gui, SoundManager *sm, nuvie_game_t type)
{
   const char *path;
   size_t len;
   config = cfg;
   gametype = type;
   script = this;

   script_obj_list = iAVLAllocTree(get_iAVLKey);

   L = lua_open();
   luaL_openlibs(L);

   luaL_newmetatable(L, "nuvie.U6Link");
   luaL_register(L, NULL, nscript_u6linklib_m);

   luaL_newmetatable(L, "nuvie.U6LinkRecursive");
   luaL_register(L, NULL, nscript_u6linkrecursivelib_m);

   luaL_newmetatable(L, "nuvie.Obj");
   //lua_pushvalue(L, -1); //duplicate metatable
   //lua_setfield(L, -2, "__index"); // add __index to metatable
   luaL_register(L, NULL, nscript_objlib_m);

   luaL_register(L, "Obj", nscript_objlib_f);

   lua_pushcfunction(L, nscript_load);
   lua_setglobal(L, "nuvie_load");

   lua_pushcfunction(L, nscript_config_get_boolean_value);
   lua_setglobal(L, "config_get_boolean_value");

   nscript_init_actor(L);
   nscript_init_cutscene(L, cfg, gui, sm);

   lua_pushcfunction(L, nscript_objlist_seek);
   lua_setglobal(L, "objlist_seek");

   lua_pushcfunction(L, nscript_objlist_read2);
   lua_setglobal(L, "objlist_read2");

   lua_pushcfunction(L, nscript_objlist_write2);
   lua_setglobal(L, "objlist_write2");

   lua_pushcfunction(L, nscript_print);
   lua_setglobal(L, "print");

   lua_pushcfunction(L, nscript_display_prompt);
   lua_setglobal(L, "display_prompt");

   lua_pushcfunction(L, nscript_input_select);
   lua_setglobal(L, "input_select");

   lua_pushcfunction(L, nscript_input_select_integer);
   lua_setglobal(L, "input_select_integer");

   lua_pushcfunction(L, nscript_play_sfx);
   lua_setglobal(L, "play_sfx");

   lua_pushcfunction(L, nscript_party);
   lua_setglobal(L, "party_members");

   lua_pushcfunction(L, nscript_container);
   lua_setglobal(L, "container_objs");

   lua_pushcfunction(L, nscript_find_obj);
   lua_setglobal(L, "find_obj");
   
   lua_pushcfunction(L, nscript_timer_set);
   lua_setglobal(L, "timer_set");

   lua_pushcfunction(L, nscript_timer_get);
   lua_setglobal(L, "timer_get");

   lua_pushcfunction(L, nscript_timer_update_all);
   lua_setglobal(L, "timer_update_all");

   lua_pushcfunction(L, nscript_clock_get_minute);
   lua_setglobal(L, "clock_get_minute");

   lua_pushcfunction(L, nscript_clock_get_hour);
   lua_setglobal(L, "clock_get_hour");

   lua_pushcfunction(L, nscript_clock_inc);
   lua_setglobal(L, "clock_inc");

   lua_pushcfunction(L, nscript_wind_set);
   lua_setglobal(L, "wind_set_dir");

   lua_pushcfunction(L, nscript_wind_get);
   lua_setglobal(L, "wind_get_dir");

   lua_pushcfunction(L, nscript_tile_get_flag);
   lua_setglobal(L, "tile_get_flag");

   lua_pushcfunction(L, nscript_objs_at_loc);
   lua_setglobal(L, "objs_at_loc");
   
   lua_pushcfunction(L, nscript_map_get_obj);
   lua_setglobal(L, "map_get_obj");

   lua_pushcfunction(L, nscript_map_remove_obj);
   lua_setglobal(L, "map_remove_obj");

   lua_pushcfunction(L, nscript_map_is_water);
   lua_setglobal(L, "map_is_water");

   lua_pushcfunction(L, nscript_map_is_on_screen);
   lua_setglobal(L, "map_is_on_screen");

   lua_pushcfunction(L, nscript_map_get_impedence);
   lua_setglobal(L, "map_get_impedence");

   lua_pushcfunction(L, nscript_map_get_dmg_tile_num);
   lua_setglobal(L, "map_get_dmg_tile_num");

   lua_pushcfunction(L, nscript_map_can_put_actor);
   lua_setglobal(L, "map_can_put");

   lua_pushcfunction(L, nscript_map_can_put_obj);
   lua_setglobal(L, "map_can_put_obj");

   lua_pushcfunction(L, nscript_map_line_test);
   lua_setglobal(L, "map_can_reach_point");

   lua_pushcfunction(L, nscript_map_line_hit_check);
   lua_setglobal(L, "map_line_hit_check");

   lua_pushcfunction(L, nscript_player_get_location);
   lua_setglobal(L, "player_get_location");

   lua_pushcfunction(L, nscript_player_get_karma);
   lua_setglobal(L, "player_get_karma");

   lua_pushcfunction(L, nscript_player_set_karma);
   lua_setglobal(L, "player_set_karma");

   lua_pushcfunction(L, nscript_player_dec_alcohol);
   lua_setglobal(L, "player_dec_alcohol");

   lua_pushcfunction(L, nscript_player_move);
   lua_setglobal(L, "player_move");

   lua_pushcfunction(L, nscript_player_set_actor);
   lua_setglobal(L, "player_set_actor");

   lua_pushcfunction(L, nscript_party_get_size);
   lua_setglobal(L, "party_get_size");

   lua_pushcfunction(L, nscript_party_get_member);
   lua_setglobal(L, "party_get_member");

   lua_pushcfunction(L, nscript_party_is_in_combat_mode);
   lua_setglobal(L, "party_is_in_combat_mode");

   lua_pushcfunction(L, nscript_party_set_combat_mode);
   lua_setglobal(L, "party_set_combat_mode");

   lua_pushcfunction(L, nscript_party_set_party_mode);
   lua_setglobal(L, "party_set_party_mode");

   lua_pushcfunction(L, nscript_party_move);
   lua_setglobal(L, "party_move");

   lua_pushcfunction(L, nscript_party_update_leader);
   lua_setglobal(L, "party_update_leader");

   lua_pushcfunction(L, nscript_party_resurrect_dead_members);
   lua_setglobal(L, "party_resurrect_dead_members");

   lua_pushcfunction(L, nscript_party_exit_vehicle);
   lua_setglobal(L, "party_exit_vehicle");

   lua_pushcfunction(L, nscript_party_dismount_from_horses);
   lua_setglobal(L, "party_dismount_from_horses");

   lua_pushcfunction(L, nscript_quake_start);
   lua_setglobal(L, "quake_start");

   lua_pushcfunction(L, nscript_explosion_start);
   lua_setglobal(L, "explosion_start");

   lua_pushcfunction(L, nscript_projectile_anim);
   lua_setglobal(L, "projectile_anim");

   lua_pushcfunction(L, nscript_projectile_anim_multi);
   lua_setglobal(L, "projectile_anim_multi");

   lua_pushcfunction(L, nscript_hit_anim);
   lua_setglobal(L, "hit_anim");

   lua_pushcfunction(L, nscript_usecode_look);
   lua_setglobal(L, "usecode_look");

   lua_pushcfunction(L, nscript_fade_out);
   lua_setglobal(L, "fade_out");
   
   lua_pushcfunction(L, nscript_fade_in);
   lua_setglobal(L, "fade_in");

   lua_pushcfunction(L, nscript_fade_tile);
   lua_setglobal(L, "fade_tile");

   lua_pushcfunction(L, nscript_black_fade_obj);
   lua_setglobal(L, "fade_obj");

   lua_pushcfunction(L, nscript_xor_effect);
   lua_setglobal(L, "xor_effect");

   lua_pushcfunction(L, nscript_xray_effect);
   lua_setglobal(L, "xray_effect");

   lua_pushcfunction(L, nscript_peer_effect);
   lua_setglobal(L, "peer_effect");

   lua_pushcfunction(L, nscript_wing_strike_effect);
   lua_setglobal(L, "wing_strike_effect");

   lua_pushcfunction(L, nscript_hail_storm_effect);
   lua_setglobal(L, "hail_storm_effect");

   lua_pushcfunction(L, nscript_wizard_eye_effect);
   lua_setglobal(L, "wizard_eye_effect");

   lua_pushcfunction(L, nscript_is_god_mode_enabled);
   lua_setglobal(L, "is_god_mode_enabled");

   lua_pushcfunction(L, nscript_set_armageddon);
   lua_setglobal(L, "set_armageddon");

   seed_random();

   lua_getglobal(L, "package");
   lua_pushstring(L, "path");
   lua_gettable(L, -2);

   path = lua_tolstring(L, -1, &len);
   DEBUG(0, LEVEL_INFORMATIONAL, "lua path = %s\n", path);

}

Script::~Script()
{
   if(L)
      lua_close(L);
}

bool Script::init()
{
	std::string dir, path;
	config->value("config/datadir", dir, "");
	build_path(dir, "scripts", path);
	dir = path;

	std::string game_tag = get_game_tag(gametype);
	stringToLower(game_tag);

	build_path(dir, game_tag, path);

	dir = path;
	build_path(dir, "init.lua", path);
	ConsoleAddInfo("Loading init.lua");

	std::string init_str = "init = nuvie_load(\"";
	init_str.append(game_tag);
	init_str.append("/init.lua\"); init()");

	if(run_script(init_str.c_str()) == false)
	{
		std::string errorStr = "Loading ";
		errorStr.append(path);
		ConsoleAddError(errorStr);
		return false;
	}

	return true;
}

void Script::seed_random()
{
   //Seed the lua random number generator.
   //seed with a random number from NUVIE_RAND()
   //this should be seeded at this point.

   lua_getfield(L, LUA_GLOBALSINDEX, "math");
   lua_getfield(L, -1, "randomseed");
   lua_remove(L, -2);
   lua_pushnumber(L, NUVIE_RAND());
   lua_pcall(L, 1, 0, 0);

   return;
}

bool Script::run_script(const char *script)
{
   if(luaL_dostring(L, script) != 0)
   {
      DEBUG(0, LEVEL_ERROR, "Script Error: %s\n", luaL_checkstring(L, -1));
      return false;
   }

   return true;
}

bool Script::call_actor_update_all()
{
   lua_getglobal(L, "actor_update_all");

   return call_function("actor_update_all", 0, 0);
}

bool Script::call_actor_init(Actor *actor, uint8 alignment)
{
   lua_getglobal(L, "actor_init");
   nscript_new_actor_var(L, actor->get_actor_num());
   lua_pushinteger(L, alignment);

   return call_function("actor_init", 2, 0);
}

bool Script::call_actor_attack(Actor *actor, MapCoord location, Obj *weapon, Actor * foe)
{
   lua_getglobal(L, "actor_attack");
   nscript_new_actor_var(L, actor->get_actor_num());
   //nscript_new_actor_var(L, foe->get_actor_num());
   uint8 num_arg = 6;
   lua_pushnumber(L, (lua_Number)location.x);
   lua_pushnumber(L, (lua_Number)location.y);
   lua_pushnumber(L, (lua_Number)location.z);
   if(weapon == NULL)
      nscript_new_actor_var(L, actor->get_actor_num());
   else
      nscript_obj_new(L, weapon);
   if(foe == NULL)
      num_arg = 5;
   else
      nscript_new_actor_var(L, foe->get_actor_num());

   if(call_function("actor_attack", num_arg, 0) == false)
   {
      return false;
   }

   Game::get_game()->get_map_window()->updateBlacking(); // the script might have updated the blocking objects. eg broken a door.
   return true;
}

bool Script::call_load_game(NuvieIO *objlist)
{
	return call_loadsave_game("load_game", objlist);
}

bool Script::call_save_game(NuvieIO *objlist)
{
	return call_loadsave_game("save_game", objlist);
}

bool Script::call_loadsave_game(const char *function, NuvieIO *objlist)
{
	g_objlist_file = objlist;
	lua_getglobal(L, function);

	bool result = call_function(function, 0, 0);

	g_objlist_file = NULL;
	return result;
}

bool Script::call_actor_map_dmg(Actor *actor, MapCoord location)
{

   lua_getglobal(L, "actor_map_dmg");
   nscript_new_actor_var(L, actor->get_actor_num());
   //nscript_new_actor_var(L, foe->get_actor_num());
   lua_pushnumber(L, (lua_Number)location.x);
   lua_pushnumber(L, (lua_Number)location.y);
   lua_pushnumber(L, (lua_Number)location.z);

   return call_function("actor_map_dmg", 4, 0);
}

bool Script::call_actor_tile_dmg(Actor *actor, uint16 tile_num)
{

   lua_getglobal(L, "actor_tile_dmg");
   nscript_new_actor_var(L, actor->get_actor_num());
   lua_pushnumber(L, (lua_Number)tile_num);


   return call_function("actor_tile_dmg", 2, 0);
}

bool Script::call_actor_hit(Actor *actor, uint8 dmg, bool display_hit_msg)
{
   lua_getglobal(L, "actor_hit");
   nscript_new_actor_var(L, actor->get_actor_num());
   lua_pushnumber(L, (lua_Number)dmg);

   if(call_function("actor_hit", 2, 0) == false)
	   return false;

   if(display_hit_msg)
   {
	   lua_getglobal(L, "actor_hit_msg");
	   nscript_new_actor_var(L, actor->get_actor_num());

	   return call_function("actor_hit_msg", 1, 0);
   }

   return true;
}

bool Script::call_look_obj(Obj *obj)
{
   lua_getglobal(L, "look_obj");

   nscript_obj_new(L, obj);

   if(call_function("look_obj", 1, 1) == false)
	   return false;

   return lua_toboolean(L,-1);
}

int Script::call_obj_get_readiable_location(Obj *obj)
{
   lua_getglobal(L, "obj_get_readiable_location");

   nscript_obj_new(L, obj);

   if(call_function("obj_get_readiable_location", 1, 1) == false)
	   return -1;

   return lua_tointeger(L,-1);
}


bool Script::call_actor_get_obj(Actor *actor, Obj *obj)
{
   lua_getglobal(L, "actor_get_obj");
   nscript_new_actor_var(L, actor->get_actor_num());
   nscript_obj_new(L, obj);

   if(call_function("actor_get_obj", 2, 1) == false)
	   return false;

   return lua_toboolean(L,-1);
}

bool Script::call_actor_subtract_movement_points(Actor *actor, uint8 points)
{
   lua_getglobal(L, "subtract_movement_pts");
   nscript_new_actor_var(L, actor->get_actor_num());
   lua_pushnumber(L, (lua_Number)points);

   if(call_function("subtract_movement_pts", 2, 0) == false)
	   return false;

   return true;
}

bool Script::call_actor_resurrect(Actor *actor)
{
   lua_getglobal(L, "actor_resurrect");
   nscript_new_actor_var(L, actor->get_actor_num());

   if(call_function("actor_resurrect", 1, 0) == false)
	   return false;

   return true;
}

bool Script::call_use_keg(Obj *obj)
{
   lua_getglobal(L, "use_keg");

   nscript_obj_new(L, obj);

   if(call_function("use_keg", 1, 0) == false)
	   return false;

   return true;
}

bool Script::call_magic_get_spell_list(Spell **spell_list)
{
	lua_getglobal(L, "magic_get_spell_list");

	if(call_function("magic_get_spell_list", 0, 1) == false)
		return false;

	for(int i=1;;i++)
	{
		lua_pushinteger(L, i);
		lua_gettable(L, -2);

		if(!lua_istable(L, -1)) //we've hit the end of our targets
		{
			printf("end = %d",i);
			lua_pop(L, 1);
			break;
		}

		uint16 num;
		uint8 re;
		char name[13];
		char invocation[5];

		get_tbl_field_uint16(L, "spell_num", &num);
		get_tbl_field_uint8(L, "reagents", &re);
		get_tbl_field_string(L, "name", name, 12);
		get_tbl_field_string(L, "invocation", invocation, 4);

		if(num < 256 && spell_list[num] == NULL)
		{
			spell_list[num] = new Spell((uint8)num, (const char *)name, (const char *)invocation, re);
			printf("num = %d, reagents = %d, name = %s invocation = %s\n", num, re, name, invocation);
		}

		lua_pop(L, 1);
	}

	return true;
}

bool Script::call_actor_use_effect(Obj *effect_obj, Actor *actor)
{
	lua_getglobal(L, "actor_use_effect");
	nscript_new_actor_var(L, actor->get_actor_num());
	nscript_obj_new(L, effect_obj);

	return call_function("actor_use_effect", 2, 0);
}

bool Script::call_can_get_obj_override(Obj *obj)
{
	lua_getglobal(L, "can_get_obj_override");
	nscript_obj_new(L, obj);

	if(call_function("can_get_obj_override", 1, 1) == false)
		return false;

   return lua_toboolean(L,-1);
}

bool Script::call_out_of_ammo(Actor *attacker, Obj *weapon, bool print_message)
{
	lua_getglobal(L, "out_of_ammo");
	nscript_new_actor_var(L, attacker->get_actor_num());
	if(weapon == NULL)
		nscript_new_actor_var(L, attacker->get_actor_num());
	else
		nscript_obj_new(L, weapon);
	lua_pushboolean(L, print_message);

	if(call_function("out_of_ammo", 3, 1) == false)
		return false;

	return lua_toboolean(L,-1);
}

bool Script::call_function(const char *func_name, int num_args, int num_return, bool print_stacktrace)
{
	int start_idx = lua_gettop(L);
	int error_index = 0;

	if(print_stacktrace)
	{
		error_index = lua_gettop(L) - num_args;
		lua_pushcfunction(L, lua_error_handler);
		lua_insert(L, error_index);
	}

	int result = lua_pcall(L, num_args, num_return, error_index);
	if(result != 0)
	{
		DEBUG(0, LEVEL_ERROR, "Script Error: %s(), %s\n", func_name, luaL_checkstring(L, -1));
		lua_pop(L,1);
	}

	if(print_stacktrace)
	{
		lua_remove(L, error_index);
	}

	if(lua_gettop(L) + num_args + 1 != start_idx + num_return)
		DEBUG(0, LEVEL_ERROR, "lua stack error!");

	return (result != 0) ? false : true;
}

bool Script::run_lua_file(const char *filename)
{
	std::string dir, path;
	Script::get_script()->get_config()->value("config/datadir", dir, "");

	build_path(dir, "scripts", path);
	dir = path;
	build_path(dir, filename, path);

	if(luaL_loadfile(L, path.c_str()) != 0)
	{
		DEBUG(0, LEVEL_ERROR, "loading script file %s", path.c_str() );
		return false;
	}

	return call_function(path.c_str(), 0, 0);
}

bool Script::call_moonstone_set_loc(uint8 phase, MapCoord location)
{
   lua_getglobal(L, "moonstone_set_loc");

   lua_pushnumber(L, (lua_Number)phase);
   lua_pushnumber(L, (lua_Number)location.x);
   lua_pushnumber(L, (lua_Number)location.y);
   lua_pushnumber(L, (lua_Number)location.z);

   return call_function("moonstone_set_loc", 4, 0);
}

MapCoord Script::call_moonstone_get_loc(uint8 phase)
{
   MapCoord loc(0,0,0);

   lua_getglobal(L, "moonstone_get_loc");

   lua_pushnumber(L, (lua_Number)phase);

   if(call_function("moonstone_get_loc", 1, 1) == false)
	   return loc;

   get_tbl_field_uint16(L, "x", &loc.x);
   get_tbl_field_uint16(L, "y", &loc.y);
   get_tbl_field_uint8(L, "z", &loc.z);

   return loc;
}

bool Script::call_update_moongates(bool visible)
{
   lua_getglobal(L, "update_moongates");

   lua_pushboolean(L, visible);

   return call_function("update_moongates", 1, 0);
}

bool Script::call_advance_time(uint16 minutes)
{
	lua_getglobal(L, "advance_time");

	lua_pushnumber(L, (lua_Number)minutes);

	return call_function("advance_time", 1, 0);
}

ScriptThread *Script::new_thread(const char *scriptfile)
{
   ScriptThread *t = NULL;
   lua_State *s;

   s = lua_newthread(L);
   lua_getglobal(s, "run_script");
   lua_pushstring(s, scriptfile);

   t = new ScriptThread(s, 1);

   return t;
}

ScriptThread *Script::new_thread_from_string(const char *script)
{
   ScriptThread *t = NULL;
   lua_State *s;

   s = lua_newthread(L);

   if(luaL_loadbuffer(s, script, strlen(script), "nuvie") != 0)
      return NULL;

   t = new ScriptThread(s, 0);

   return t;
}

bool nscript_get_location_from_args(lua_State *L, uint16 *x, uint16 *y, uint8 *z, int lua_stack_offset)
{
   if(lua_istable(L, lua_stack_offset))
   {
      if(!get_tbl_field_uint16(L, "x", x)) return false;

      if(!get_tbl_field_uint16(L, "y", y)) return false;

      if(!get_tbl_field_uint8(L, "z", z)) return false;
   }
   else
   {
	  if(lua_isnil(L, lua_stack_offset)) return false;
      *x = (uint16)luaL_checkinteger(L, lua_stack_offset);
      *y = (uint16)luaL_checkinteger(L, lua_stack_offset + 1);
      *z = (uint8)luaL_checkinteger(L,  lua_stack_offset + 2);
   }

   return true;
}

Obj *nscript_get_obj_from_args(lua_State *L, int lua_stack_offset)
{
	   Obj **s_obj = (Obj **)luaL_checkudata(L, lua_stack_offset, "nuvie.Obj");
	   if(s_obj == NULL)
		   return NULL;

	   return *s_obj;
}

void nscript_new_obj_var(lua_State *L, Obj *obj)
{
	Obj **p_obj;
    p_obj = (Obj **)lua_newuserdata(L, sizeof(Obj *));

    luaL_getmetatable(L, "nuvie.Obj");
    lua_setmetatable(L, -2);

    *p_obj = obj;

    nscript_inc_obj_ref_count(obj);
}

static int nscript_obj_new(lua_State *L)
{
   return nscript_obj_new(L, NULL);
}

int nscript_obj_new(lua_State *L, Obj *obj)
{
   Obj **p_obj;

   p_obj = (Obj **)lua_newuserdata(L, sizeof(Obj *));

   luaL_getmetatable(L, "nuvie.Obj");
   lua_setmetatable(L, -2);

   if(obj == NULL)
   {
      obj = new Obj();

      if(lua_gettop(L) > 1) // do we have arguments?
      {
         if(lua_isuserdata(L, 1)) // do we have an obj
         {
            if(nscript_obj_init_from_obj(L, obj) == false)
               return 0;
         }
         else // init object from arguments
         {
            if(nscript_obj_init_from_args(L, lua_gettop(L) - 1, obj) == false)
               return 0;
         }
      }
   }

   *p_obj = obj;

   nscript_inc_obj_ref_count(obj);

   return 1;
}

sint32 nscript_inc_obj_ref_count(Obj *obj)
{
   ScriptObjRef *obj_ref;
   iAVLKey key = (iAVLKey)obj;

   obj_ref = (ScriptObjRef *)iAVLSearch(script_obj_list, key);
   if(obj_ref == NULL)
   {
      obj->set_in_script(true); // mark as being used by script engine.
      obj_ref =  new ScriptObjRef();
      obj_ref->key = (iAVLKey)obj;
      iAVLInsert(script_obj_list, obj_ref);
   }

   obj_ref->refcount++;

   return (sint32)obj_ref->refcount;
}

sint32 nscript_dec_obj_ref_count(Obj *obj)
{
   ScriptObjRef *obj_ref;
   iAVLKey key = (iAVLKey)obj;

   obj_ref = (ScriptObjRef *)iAVLSearch(script_obj_list, key);
   if(obj_ref == NULL)
      return -1;


   obj_ref->refcount--;

   if(obj_ref->refcount == 0)
   {
      iAVLDelete(script_obj_list, key);
      delete obj_ref;
      obj->set_in_script(false); //nolonger being referenced by the script engine.
      return 0;
   }

   return obj_ref->refcount;
}

inline bool nscript_obj_init_from_obj(lua_State *L, Obj *s_obj)
{
   Obj *ptr = (Obj *)luaL_checkudata(L, 1, "nuvie.Obj");

   if(ptr == NULL)
      return false;

   s_obj->obj_n = ptr->obj_n;
   s_obj->frame_n = ptr->frame_n;
   s_obj->quality = ptr->quality;
   s_obj->qty = ptr->qty;
   s_obj->x = ptr->x;
   s_obj->y = ptr->y;
   s_obj->z = ptr->z;

   return true;
}

inline bool nscript_obj_init_from_args(lua_State *L, int nargs, Obj *s_obj)
{
   uint8 i = nargs;

   if(i)
   {
      if(!lua_isnil(L, 1))
         s_obj->obj_n = (uint16)lua_tointeger(L, 1);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 2))
         s_obj->frame_n = (uint8)lua_tointeger(L, 2);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 3))
         s_obj->quality = (uint8)lua_tointeger(L, 3);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 4))
         s_obj->qty = (uint16)lua_tointeger(L, 4);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 5))
         s_obj->x = (uint16)lua_tointeger(L, 5);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 6))
         s_obj->y = (uint16)lua_tointeger(L, 6);
      i--;
   }

   if(i)
   {
      if(!lua_isnil(L, 7))
         s_obj->z = (uint8)lua_tointeger(L, 7);
      i--;
   }

   return true;
}

static int nscript_obj_gc(lua_State *L)
{
   //DEBUG(0, LEVEL_INFORMATIONAL, "\nObj garbage Collection!\n");

   Obj **p_obj = (Obj **)lua_touserdata(L, 1);
   Obj *obj;

   if(p_obj == NULL)
      return false;

   obj = *p_obj;

   if(nscript_dec_obj_ref_count(obj) == 0) // no longer referenced by the script engine
   {
      // remove object if it is not referenced by the game engine.
      if(obj->get_engine_loc() == OBJ_LOC_NONE)
         delete_obj(obj);
   }

   return 0;
}

/*
   static inline Obj *nscript_get_obj_ptr(ScriptObj *s_obj)
   {
   if(s_obj)
   {
    if(s_obj->obj_ptr)
      return s_obj->obj_ptr;
    else
      return &s_obj->script_obj;
   }

   return NULL;
   }
 */
static int nscript_obj_set(lua_State *L)
{
   Obj **s_obj;
   Obj *obj;
   //Obj *ptr;
   const char *key;

   s_obj = (Obj **)lua_touserdata(L, 1);
   if(s_obj == NULL)
      return 0;

   obj = *s_obj;
   if(obj == NULL)
      return 0;

   // ptr = nscript_get_obj_ptr(s_obj);

   key = lua_tostring(L, 2);

   if(!strcmp(key, "x"))
   {
      obj->x = (uint16)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "y"))
   {
      obj->y = (uint16)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "z"))
   {
      obj->z = (uint8)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "obj_n"))
   {
      obj->obj_n = (uint16)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "frame_n"))
   {
      obj->frame_n = (uint8)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "quality"))
   {
      obj->quality = (uint8)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "qty"))
   {
      obj->qty = (uint8)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "status"))
   {
      obj->status = (uint8)lua_tointeger(L, 3);
      return 0;
   }

   if(!strcmp(key, "invisible"))
   {
      obj->set_invisible((bool)lua_toboolean(L, 3));
      return 0;
   }

   if(!strcmp(key, "ok_to_take"))
   {
	   obj->set_ok_to_take((bool)lua_toboolean(L, 3));
	   return 0;
   }

   if(!strcmp(key, "temporary"))
   {
	   obj->set_temporary((bool)lua_toboolean(L, 3));
	   return 0;
   }

   return 0;
}

static int nscript_obj_get(lua_State *L)
{
   Obj **s_obj;
   Obj *obj;
   const char *key;

   s_obj = (Obj **)lua_touserdata(L, 1);
   if(s_obj == NULL)
      return 0;

   obj = *s_obj;
   if(obj == NULL)
      return 0;

   //ptr = nscript_get_obj_ptr(s_obj);

   key = lua_tostring(L, 2);

   if(!strcmp(key, "luatype"))
   {
      lua_pushstring(L, "obj"); return 1;
   }

   if(!strcmp(key, "x"))
   {
      lua_pushinteger(L, obj->x); return 1;
   }

   if(!strcmp(key, "y"))
   {
      lua_pushinteger(L, obj->y); return 1;
   }

   if(!strcmp(key, "z"))
   {
      lua_pushinteger(L, obj->z); return 1;
   }

   if(!strcmp(key, "obj_n"))
   {
      lua_pushinteger(L, obj->obj_n); return 1;
   }

   if(!strcmp(key, "frame_n"))
   {
      lua_pushinteger(L, obj->frame_n); return 1;
   }

   if(!strcmp(key, "quality"))
   {
      lua_pushinteger(L, obj->quality); return 1;
   }

   if(!strcmp(key, "qty"))
   {
      lua_pushinteger(L, obj->qty); return 1;
   }

   if(!strcmp(key, "name"))
   {
      ObjManager *obj_manager = Game::get_game()->get_obj_manager();
      lua_pushstring(L, obj_manager->get_obj_name(obj->obj_n, obj->frame_n));
      return 1;
   }
/*
   if(!strcmp(key, "container"))
   {
	   U6LList *obj_list = obj->container;
	   if(obj_list == NULL)
	      return 0;

	   U6Link *link = obj_list->start();

	   lua_pushcfunction(L, nscript_u6llist_iter);

	   U6Link **p_link = (U6Link **)lua_newuserdata(L, sizeof(U6Link *));
	   *p_link = link;

	   luaL_getmetatable(L, "nuvie.U6Link");
	   lua_setmetatable(L, -2);

	   return 2;
   }
*/
   if(!strcmp(key, "look_string"))
   {
      ObjManager *obj_manager = Game::get_game()->get_obj_manager();
      lua_pushstring(L, obj_manager->look_obj(obj, true)); return 1;
   }
   
   if(!strcmp(key, "readied"))
   {
      lua_pushboolean(L, (int)obj->is_readied()); return 1;
   }

   if(!strcmp(key, "stackable"))
   {
      ObjManager *obj_manager = Game::get_game()->get_obj_manager();
      lua_pushboolean(L, (int)obj_manager->is_stackable(obj)); return 1;
   }

   if(!strcmp(key, "weight"))
   {
      ObjManager *obj_manager = Game::get_game()->get_obj_manager();
      float weight = obj_manager->get_obj_weight(obj,OBJ_WEIGHT_INCLUDE_CONTAINER_ITEMS,OBJ_WEIGHT_DONT_SCALE);
      weight = floorf(weight); //get rid of the tiny fraction
      weight /= 10; //now scale.
      lua_pushnumber(L, (lua_Number)weight); return 1;
   }

   if(!strcmp(key, "tile_num"))
   {
      ObjManager *obj_manager = Game::get_game()->get_obj_manager();
      Tile *tile = obj_manager->get_obj_tile(obj->obj_n, obj->frame_n);
      lua_pushinteger(L, (int)tile->tile_num); return 1;
   }
   
   if(!strcmp(key, "getable"))
   {
	   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
	   lua_pushboolean(L, (int)obj_manager->can_get_obj(obj)); return 1;
   }

   if(!strcmp(key, "ok_to_take"))
   {
	   lua_pushboolean(L, (int)obj->is_ok_to_take()); return 1;
   }

   return 0;
}

static int nscript_obj_movetomap(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();

   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;

   MapCoord loc;
   if(lua_gettop(L) >= 2)
   {
	   if(nscript_get_location_from_args(L, &loc.x, &loc.y, &loc.z, 2) == false)
		   return 0;
   }
   else
   {
	   loc.x = obj->x;
	   loc.y = obj->y;
	   loc.z = obj->z;
   }

   if(obj)
   {
      if(obj_manager->moveto_map(obj, loc) == false)
      {
         //delete map_obj;
         return luaL_error(L, "moving obj to map!");
      }

      //s_obj->obj_ptr = map_obj;
   }

   return 0;
}

static int nscript_obj_movetoinv(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   Actor *actor;

   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;

   if(lua_gettop(L) < 2)
      return luaL_error(L, "You must supply an Actor # to Obj.moveToInv()");

   actor = actor_manager->get_actor(lua_tointeger(L, 2));

   if(actor == NULL)
      return luaL_error(L, "Getting Actor (%d)", lua_tointeger(L, 2));

   if(obj)
   {
      if(obj_manager->moveto_inventory(obj, actor) == false)
      {
         //delete inv_obj;
         return luaL_error(L, "moving obj to actor inventory!");
      }

      //s_obj->obj_ptr = inv_obj;
   }


   return 0;
}

static int nscript_obj_movetocont(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;
   Obj *container_obj;

   obj = *s_obj;

   if(obj == NULL)
      return 0;

   if(lua_gettop(L) < 2)
      return luaL_error(L, "You must supply an Object to move into in Obj.moveToCont()");

   s_obj = (Obj **)luaL_checkudata(L, 2, "nuvie.Obj");
   container_obj = *s_obj;

   if(container_obj)
   {
      if(obj_manager->moveto_container(obj, container_obj) == false)
      {
         return luaL_error(L, "moving obj into container!");
      }
   }

   //pos = lua_tointeger(L, 2);

   return 0;
}

static int nscript_container_remove_obj(lua_State *L)
{
   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;
   Obj *cont_obj;

   obj = *s_obj;

   if(obj == NULL)
      return luaL_error(L, "getting obj!");

   cont_obj = obj->get_container_obj();

   if(cont_obj == NULL)
      return luaL_error(L, "obj not in a container!");

   if(cont_obj->remove(obj) == false)
      return luaL_error(L, "removing obj from container!");

   return 0;
}

static int nscript_obj_use(lua_State *L)
{
   UseCode *usecode = Game::get_game()->get_usecode();
   Player *player = Game::get_game()->get_player();
   Actor *actor = player->get_actor();

   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;

   if(obj)
   {
      usecode->use_obj(obj, actor);
   }


   return 0;
}

static int nscript_obj_removefromengine(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");

   Obj *obj;

   obj = *s_obj;

   if(obj)
   {
       obj_manager->unlink_from_engine(obj);
   }

   return 0;
}

/* release last iter U6Link if required. */
static int nscript_u6link_gc(lua_State *L)
{
   U6Link **s_link = (U6Link **)luaL_checkudata(L, 1, "nuvie.U6Link");
   U6Link *link = *s_link;

   if(link == NULL)
      return 0;

   releaseU6Link(link);

   printf("U6Link garbage collector!!");
   return 0;
}

/* free up resources for a recursive U6Link iterator. */
static int nscript_u6link_recursive_gc(lua_State *L)
{
   std::stack<U6Link *> **s_stack = (std::stack<U6Link *> **)luaL_checkudata(L, 1, "nuvie.U6LinkRecursive");
   std::stack<U6Link *> *s = *s_stack;

   if(s->empty() == false)
   {
      for(; !s->empty(); s->pop())
      {
          U6Link *link = s->top();

          if(link != NULL)
             releaseU6Link(link);
      }
   }

   delete s;

   printf("U6LinkResursive garbage collector!!\n");
   return 0;
}

static int nscript_print(lua_State *L)
{
   MsgScroll *scroll = Game::get_game()->get_scroll();
   const char *string = luaL_checkstring(L, 1);

   scroll->display_string(string);

   return 0;
}

static int nscript_display_prompt(lua_State *L)
{
   MsgScroll *scroll = Game::get_game()->get_scroll();

   if(!scroll->can_displayed_prompt())
      return 0;

   bool newline = lua_toboolean(L, 1);
   if(newline)
      scroll->display_string("\n");
   scroll->display_prompt();
   return 0;
}

static int nscript_load(lua_State *L)
{
   const char *file = luaL_checkstring(L, 1);
   string dir;
   string path;

   Script::get_script()->get_config()->value("config/datadir", dir, "");

   build_path(dir, "scripts", path);
   dir = path;
   build_path(dir, file, path);

   luaL_loadfile(L, path.c_str());

   return 1;
}

static int nscript_config_get_boolean_value(lua_State *L)
{
	bool value;
	const char *config_key = luaL_checkstring(L, 1);
	Script::get_script()->get_config()->value(std::string(config_key), value);

	lua_pushboolean(L, value);
	return 1;
}

static int nscript_objlist_seek(lua_State *L)
{
	uint32 position = (uint32)lua_tointeger(L, 1);
	if(g_objlist_file)
		g_objlist_file->seek(position);

	return 0;
}

static int nscript_objlist_read2(lua_State *L)
{
	if(g_objlist_file)
	{
		lua_pushinteger(L, g_objlist_file->read2());
		return 1;
	}

	return 0;
}

static int nscript_objlist_write2(lua_State *L)
{
	bool ret = false;
	uint16 value = (uint16)lua_tointeger(L, 1);
	if(g_objlist_file)
	{
		ret = g_objlist_file->write2(value);
	}

	lua_pushboolean(L, ret);
	return 1;
}

static int nscript_player_get_location(lua_State *L)
{
   Player *player = Game::get_game()->get_player();

   uint16 x, y;
   uint8 z;

   player->get_actor()->get_location(&x, &y, &z);

   lua_newtable(L);
   lua_pushstring(L, "x");
   lua_pushinteger(L, x);
   lua_settable(L, -3);

   lua_pushstring(L, "y");
   lua_pushinteger(L, y);
   lua_settable(L, -3);

   lua_pushstring(L, "z");
   lua_pushinteger(L, z);
   lua_settable(L, -3);

   return 1;
}

static int nscript_player_get_karma(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	lua_pushinteger(L, player->get_karma());
	return 1;
}

static int nscript_player_set_karma(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	player->set_karma((uint8)lua_tointeger(L, 1));
	return 0;
}

static int nscript_player_dec_alcohol(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	player->dec_alcohol((uint8)lua_tointeger(L, 1));
	return 0;
}

static int nscript_party_is_in_combat_mode(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	lua_pushboolean(L, party->is_in_combat_mode());
	return 1;
}

static int nscript_party_set_combat_mode(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	party->set_in_combat_mode(lua_toboolean(L, 1));
	return 0;
}

static int nscript_party_set_party_mode(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	player->set_party_mode(player->get_party()->get_actor(0));

	return 0;
}

static int nscript_party_move(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	uint16 x, y;
	uint8 z;

	if(nscript_get_location_from_args(L, &x, &y, &z) == false)
		return 0;

	party->move(x, y, z);

	return 0;
}

static int nscript_player_move(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	uint16 x, y;
	uint8 z;

	if(nscript_get_location_from_args(L, &x, &y, &z) == false)
		return 0;

	player->move(x, y, z, lua_toboolean(L, 4));

	return 0;
}

static int nscript_player_set_actor(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	Actor *actor = nscript_get_actor_from_args(L, 1);

	if(actor && actor != player->get_actor())
		player->update_player(actor);

	return 0;
}

static int nscript_party_get_size(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	lua_pushinteger(L, party->get_party_size());
	return 1;
}

static int nscript_party_get_member(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	uint8 member_num = (uint8)lua_tointeger(L, 1);

	Actor *actor = party->get_actor(member_num);

	if(actor == NULL)
		return 0;

	nscript_new_actor_var(L, actor->get_actor_num());
	return 1;
}

static int nscript_party_update_leader(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	Player *player = Game::get_game()->get_player();

	Actor *leader = party->get_actor(party->get_leader());

	if(leader)
	{
		player->update_player(leader);
	}

	return 0;
}

static int nscript_party_resurrect_dead_members(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	party->resurrect_dead_members();

	return 0;
}

static int nscript_party_exit_vehicle(lua_State *L)
{
	Party *party = Game::get_game()->get_party();

	uint16 x, y;
	uint8 z;

	if(nscript_get_location_from_args(L, &x, &y, &z) == false)
		return 0;

	party->exit_vehicle(x, y, z);

	return 0;
}

static int nscript_party_dismount_from_horses(lua_State *L)
{
	Party *party = Game::get_game()->get_party();
	party->dismount_from_horses();
	return 0;
}

static int nscript_map_get_obj(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   Obj *obj;

   uint16 x, y;
   uint8 z;

   if(nscript_get_location_from_args(L, &x, &y, &z) == false)
      return 0;


   if(lua_gettop(L) > 3)
   {
	   uint16 obj_n = lua_tointeger(L, 4);
	   obj = obj_manager->get_obj_of_type_from_location(obj_n, x, y, z);
   }
   else
   {
	   obj = obj_manager->get_obj(x, y, z);
   }

   if(obj)
   {
	  nscript_new_obj_var(L, obj);
      return 1;
   }

   return 0;
}

static int nscript_map_remove_obj(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();

   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;

   if(obj_manager->remove_obj_from_map(obj))
      lua_pushboolean(L, true);
   else
      lua_pushboolean(L, false);

   return 1;
}

static int nscript_map_can_put_actor(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   uint16 x, y;
   uint8 z;

   if(nscript_get_location_from_args(L, &x, &y, &z) == false)
	  return 0;

   lua_pushboolean(L, actor_manager->can_put_actor(MapCoord(x,y,z)));

   return 1;
}

static int nscript_map_can_put_obj(lua_State *L)
{
   Map *map = Game::get_game()->get_game_map();
   uint16 x, y;
   uint8 z;

   if(nscript_get_location_from_args(L, &x, &y, &z, 1) == false)
	  return 0;

   lua_pushboolean(L, map->can_put_obj(x,y,z));

   return 1;
}

static int nscript_map_is_water(lua_State *L)
{
	Map *map = Game::get_game()->get_game_map();

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);
	uint8 z = (uint8) luaL_checkinteger(L, 3);

	lua_pushboolean(L, map->is_water(x, y, z));

	return 1;
}

static int nscript_map_is_on_screen(lua_State *L)
{
	MapWindow *map_window = Game::get_game()->get_map_window();

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);
	uint8 z = (uint8) luaL_checkinteger(L, 3);

	lua_pushboolean(L, map_window->is_on_screen(x, y, z));

	return 1;
}

static int nscript_map_get_impedence(lua_State *L)
{
	Map *map = Game::get_game()->get_game_map();

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);
	uint8 z = (uint8) luaL_checkinteger(L, 3);

	bool ignore_objects = true;

	if(lua_gettop(L) >= 4)
		ignore_objects = (bool) lua_toboolean(L, 4);

	lua_pushinteger(L, map->get_impedance(x, y, z, ignore_objects));

	return 1;
}

static int nscript_map_get_dmg_tile_num(lua_State *L)
{
	Map *map = Game::get_game()->get_game_map();

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);
	uint8 z = (uint8) luaL_checkinteger(L, 3);

	Tile *t = map->get_dmg_tile(x, y, z);
	if(t != NULL)
	{
		lua_pushinteger(L, t->tile_num);
		return 1;
	}

	return 0;
}

static int nscript_map_line_test(lua_State *L)
{
	Map *map = Game::get_game()->get_game_map();
	LineTestResult result;
	bool ret = false;

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);

	uint16 x1 = (uint16) luaL_checkinteger(L, 3);
	uint16 y1 = (uint16) luaL_checkinteger(L, 4);
	uint8 level = (uint8) luaL_checkinteger(L, 5);

	if(map->lineTest(x, y, x1, y1, level, LT_HitMissileBoundary, result) == false)
		ret = true;

	lua_pushboolean(L, (int)ret);
	return 1;
}

static int nscript_map_line_hit_check(lua_State *L)
{
	Map *map = Game::get_game()->get_game_map();
	LineTestResult result;

	uint16 x = (uint16) luaL_checkinteger(L, 1);
	uint16 y = (uint16) luaL_checkinteger(L, 2);

	uint16 x1 = (uint16) luaL_checkinteger(L, 3);
	uint16 y1 = (uint16) luaL_checkinteger(L, 4);
	uint8 level = (uint8) luaL_checkinteger(L, 5);

	if(map->lineTest(x, y, x1, y1, level, LT_HitMissileBoundary, result))
	{
		lua_pushinteger(L, result.hit_x);
		lua_pushinteger(L, result.hit_y);
	}
	else
	{
		lua_pushinteger(L, x1);
		lua_pushinteger(L, y1);
	}

	return 2;
}

static int nscript_tile_get_flag(lua_State *L)
{
	uint16 tile_num = (uint16) luaL_checkinteger(L, 1);
	uint8 flag_set = (uint8) luaL_checkinteger(L, 2);
	uint8 bit = (uint8) luaL_checkinteger(L, 3);

	Tile *tile = Game::get_game()->get_tile_manager()->get_tile(tile_num);

	if(tile == NULL || flag_set < 1 || flag_set > 3 || bit > 7)
		return 0;

	uint8 bit_flags = 0;

	if(flag_set == 1)
	{
		bit_flags = tile->flags1;
	}
	else if(flag_set == 2)
	{
		bit_flags = tile->flags2;
	}
	else if(flag_set == 3)
	{
		bit_flags = tile->flags3;
	}

	lua_pushboolean(L, (bool)(bit_flags & (1 << bit)));
	return 1;
}

static int nscript_quake_start(lua_State *L)
{
   Player *player = Game::get_game()->get_player();

   uint8 magnitude = (uint8)luaL_checkinteger(L, 1);
   uint32 duration = (uint32)luaL_checkinteger(L, 2);

   new QuakeEffect(magnitude, duration, player->get_actor());

   lua_pushboolean(L, true);
   return 1;
}

static int nscript_new_hit_entities_tbl_var(lua_State *L, ProjectileEffect *effect)
{
	   vector<MapEntity> *hit_items = effect->get_hit_entities();

	   lua_newtable(L);

	   for(uint16 i=0;i<hit_items->size();i++)
	   {
	      lua_pushinteger(L, i);

	      MapEntity m = (*hit_items)[i];
	      if(m.entity_type == ENT_OBJ)
	    	  nscript_obj_new(L, m.obj);
	      else if(m.entity_type == ENT_ACTOR)
	      {
	    	  nscript_new_actor_var(L, m.actor->get_actor_num());
	      }

	      lua_settable(L, -3);
	   }

	   return 1;
}

static int nscript_explosion_start(lua_State *L)
{
   uint16 tile_num = (uint16)luaL_checkinteger(L, 1);
   uint16 x = (uint16)luaL_checkinteger(L, 2);
   uint16 y = (uint16)luaL_checkinteger(L, 3);

   ExpEffect *effect = new ExpEffect(tile_num, MapCoord(x, y));
   AsyncEffect *e = new AsyncEffect(effect);
   e->run();

   return nscript_new_hit_entities_tbl_var(L, (ProjectileEffect *)effect);
}

static int nscript_projectile_anim(lua_State *L)
{
uint16 tile_num = (uint16)luaL_checkinteger(L, 1);
   uint16 startx = (uint16)luaL_checkinteger(L, 2);
   uint16 starty = (uint16)luaL_checkinteger(L, 3);
   uint16 targetx = (uint16)luaL_checkinteger(L, 4);
   uint16 targety = (uint16)luaL_checkinteger(L, 5);
   uint16 speed = (uint16)luaL_checkinteger(L, 6);
   bool trail = (bool)lua_toboolean(L, 7);
   uint8 initial_tile_rotation = (uint8)luaL_checkinteger(L, 8);
   uint16 rotation_amount = 0;
   uint8 src_tile_y_offset = 0;

   if(lua_gettop(L) >= 9)
	   rotation_amount = (uint16)luaL_checkinteger(L, 9);

   if(lua_gettop(L) >= 10)
	   src_tile_y_offset = (uint8)luaL_checkinteger(L, 10);

   ProjectileEffect *projectile_effect = new ProjectileEffect(tile_num, MapCoord(startx,starty), MapCoord(targetx,targety), speed, trail, initial_tile_rotation, rotation_amount, src_tile_y_offset);
   AsyncEffect *e = new AsyncEffect(projectile_effect);
   e->run();

   lua_pushboolean(L, true);
   return 1;
}

static int nscript_projectile_anim_multi(lua_State *L)
{
   uint16 tile_num = (uint16)luaL_checkinteger(L, 1);
   uint16 startx = (uint16)luaL_checkinteger(L, 2);
   uint16 starty = (uint16)luaL_checkinteger(L, 3);

   if(!lua_istable(L, 4))
   {
	   lua_pushboolean(L, false);
	   return 1;
   }

   lua_pushvalue(L, 4); //push table containing targets to top of stack

   uint16 x = 0;
   uint16 y = 0;
   uint8 z = 0;

   vector<MapCoord> t;

   for(int i=1;;i++)
   {
	   lua_pushinteger(L, i);
	   lua_gettable(L, -2);

	   if(!lua_istable(L, -1)) //we've hit the end of our targets
	   {
		   printf("end = %d",i);
		   lua_pop(L, 1);
		   break;
	   }
	   //get target fields here.

	   get_tbl_field_uint16(L, "x", &x);
	   get_tbl_field_uint16(L, "y", &y);
	   get_tbl_field_uint8(L, "z", &z);

	   t.push_back(MapCoord(x,y,z));

	   lua_pop(L, 1);
   }

   uint16 speed = (uint16)luaL_checkinteger(L, 5);
   bool trail = (bool)luaL_checkinteger(L, 6);
   uint8 initial_tile_rotation = (uint8)luaL_checkinteger(L, 7);

   ProjectileEffect *effect = new ProjectileEffect(tile_num, MapCoord(startx,starty), t, speed, trail, initial_tile_rotation);
   AsyncEffect *e = new AsyncEffect(effect);
   e->run();

   return nscript_new_hit_entities_tbl_var(L, effect);
}

static int nscript_hit_anim(lua_State *L)
{
   uint16 targetx = (uint16)luaL_checkinteger(L, 1);
   uint16 targety = (uint16)luaL_checkinteger(L, 2);


   AsyncEffect *e = new AsyncEffect(new HitEffect(MapCoord(targetx,targety)));
   e->run();

   lua_pushboolean(L, true);
   return 1;
}


//FIXME need to move this into lua script.
static int nscript_usecode_look(lua_State *L)
{
   Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
   Obj *obj;
   
   obj = *s_obj;
   
   UseCode *usecode = Game::get_game()->get_usecode();
   Player *player = Game::get_game()->get_player();
   
   lua_pushboolean(L, (int)usecode->look_obj(obj, player->get_actor()));
   return 1;
}

static int nscript_fade_out(lua_State *L)
{
	AsyncEffect *e = new AsyncEffect(new FadeEffect(FADE_PIXELATED, FADE_OUT));
	e->run();

	return 0;
}

static int nscript_fade_in(lua_State *L)
{
	AsyncEffect *e = new AsyncEffect(new FadeEffect(FADE_PIXELATED, FADE_IN));
	e->run();

	return 0;
}

static int nscript_fade_tile(lua_State *L)
{
	MapCoord loc;
	Tile *tile_from =  NULL;
	Tile *tile_to =  NULL;
	TileManager *tm = Game::get_game()->get_tile_manager();

	if(nscript_get_location_from_args(L, &loc.x, &loc.y, &loc.z) == false)
		return 0;

	if(lua_isnumber(L, 4))
		tile_from = tm->get_tile((uint16)luaL_checkinteger(L, 4));

	if(lua_gettop(L) > 4)
		tile_to = tm->get_tile((uint16)luaL_checkinteger(L, 5));


	AsyncEffect *e = new AsyncEffect(new TileFadeEffect(loc, tile_from, tile_to, FADE_PIXELATED, 10));
//	AsyncEffect *e = new AsyncEffect(new TileFadeEffect(loc, tile_from, 0, 4, false, 20));
	e->run();


	return 0;
}

static int nscript_black_fade_obj(lua_State *L)
{
	   Obj *obj = nscript_get_obj_from_args(L, 1);
	   uint8 fade_color = (uint8)lua_tointeger(L, 2);
	   uint16 fade_speed = (uint8)lua_tointeger(L, 3);

	   if(obj != NULL)
	   {
		   AsyncEffect *e = new AsyncEffect(new TileBlackFadeEffect(obj, fade_color, fade_speed));
		   e->run();
	   }

	   return 0;
}

static int nscript_xor_effect(lua_State *L)
{
	uint16 duration = (uint16)luaL_checkinteger(L, 1);

	AsyncEffect *e = new AsyncEffect(new XorEffect(duration));
	e->run();

	return 0;
}

static int nscript_xray_effect(lua_State *L)
{
	uint16 duration = (uint16)luaL_checkinteger(L, 1);

	AsyncEffect *e = new AsyncEffect(new XRayEffect(duration));
	e->run();

	return 0;
}

static int nscript_peer_effect(lua_State *L)
{
	uint16 x, y;
	 uint8 z;

	 Game::get_game()->get_player()->get_location(&x,&y,&z);

	AsyncEffect *e = new AsyncEffect(new PeerEffect((x-x%8)-18,(y-y%8)-18,z));
		e->run(EFFECT_PROCESS_GUI_INPUT);

		return 0;
}

static int nscript_wing_strike_effect(lua_State *L)
{
	Actor *actor = nscript_get_actor_from_args(L, 1);

	if(actor != NULL)
	{
		AsyncEffect *e = new AsyncEffect(new WingStrikeEffect(actor));
		e->run();
	}

	return 0;
}

static int nscript_hail_storm_effect(lua_State *L)
{
	MapCoord loc;
	if(nscript_get_location_from_args(L, &loc.x, &loc.y, &loc.z) == false)
		return 0;


	AsyncEffect *e = new AsyncEffect(new HailStormEffect(loc));
	e->run();


	return 0;
}

static int nscript_wizard_eye_effect(lua_State *L)
{
	MapCoord loc;
	uint16 duration = (uint16)luaL_checkinteger(L, 1);

	if(nscript_get_location_from_args(L, &loc.x, &loc.y, &loc.z, 2) == false)
		return 0;

	AsyncEffect *e = new AsyncEffect(new WizardEyeEffect(loc, duration));
	e->run(EFFECT_PROCESS_GUI_INPUT);


	return 0;
}

static int nscript_play_sfx(lua_State *L)
{
	bool play_mode = SFX_PLAY_SYNC;
	uint16 sfx_id = (uint16)luaL_checkinteger(L, 1);
	if(lua_gettop(L) > 1)
	{
		if(lua_toboolean(L, 2) == true)
			play_mode = SFX_PLAY_ASYNC;
	}

	Game::get_game()->get_sound_manager()->playSfx(sfx_id, play_mode);

	return 0;
}

int nscript_u6llist_iter(lua_State *L)
{
   U6Link **s_link = (U6Link **)luaL_checkudata(L, 1, "nuvie.U6Link");
   U6Link *link = *s_link;

   if(link == NULL || link->data == NULL)
      return 0;

   Obj *obj = (Obj *)link->data;
   nscript_obj_new(L, obj);

   retainU6Link(link->next);
   *s_link = link->next;

   releaseU6Link(link); // release old link object.

   return 1;
}

int nscript_u6llist_iter_recursive(lua_State *L)
{
   std::stack<U6Link *> **s_stack = (std::stack<U6Link *> **)luaL_checkudata(L, 1, "nuvie.U6LinkRecursive");
   std::stack<U6Link *> *s = *s_stack;

   if(s->empty() || s->top() == NULL)
      return 0;

   U6Link *link = s->top();

   Obj *obj = (Obj *)link->data;
   nscript_obj_new(L, obj);

   s->pop();
   if(link->next != NULL)
   {
	   s->push(link->next);
	   retainU6Link(link->next);
   }

   if(obj->container && obj->container->count() > 0)
   {
	   s->push(obj->container->start());
	   retainU6Link(obj->container->start());
   }

   releaseU6Link(link); // release old link object.

   return 1;
}

int nscript_party_iter(lua_State *L)
{
   uint16 party_index = (uint16)lua_tointeger(L, lua_upvalueindex(1));
   
   if(party_index == Game::get_game()->get_party()->get_party_size())
      return 0;
   
   uint8 actor_num = Game::get_game()->get_party()->get_actor_num(party_index);
   
   lua_pushinteger(L, party_index + 1);
   lua_replace(L, lua_upvalueindex(1));
   
   nscript_new_actor_var(L, actor_num);
   
   return 1;
}

static int nscript_party(lua_State *L)
{
   lua_pushinteger(L, 0);
   lua_pushcclosure(L, &nscript_party_iter, 1);
   return 1;
}

int nscript_find_obj_iter(lua_State *L)
{
	Obj *cur_obj = NULL;

	if(!lua_isnil(L, lua_upvalueindex(1)))
		cur_obj = nscript_get_obj_from_args(L, lua_upvalueindex(1));
	uint8 level = (uint8)lua_tointeger(L, lua_upvalueindex(2));
	bool match_frame_n = (bool)lua_toboolean(L, lua_upvalueindex(3));
	bool match_quality = (bool)lua_toboolean(L, lua_upvalueindex(4));

	if(cur_obj == NULL)
		return 0;

	ObjManager *obj_manager = Game::get_game()->get_obj_manager();
	Obj *next_obj = obj_manager->find_next_obj(level, cur_obj, match_frame_n, match_quality);

	if(next_obj == NULL)
	{
		lua_pushnil(L);
	}
	else
	{
		nscript_new_obj_var(L, next_obj);
	}
	lua_replace(L, lua_upvalueindex(1));

	lua_pushinteger(L, level);
	lua_replace(L, lua_upvalueindex(2));

	lua_pushboolean(L, match_frame_n);
	lua_replace(L, lua_upvalueindex(3));

	lua_pushboolean(L, match_quality);
	lua_replace(L, lua_upvalueindex(4));

	nscript_new_obj_var(L, cur_obj);

	return 1;
}

static int nscript_find_obj(lua_State *L)
{
	uint8 level = (uint8)luaL_checkinteger(L, 1);
	uint16 obj_n = (uint16)luaL_checkinteger(L, 2);
	uint16 frame_n = 0;
	bool match_frame_n = OBJ_NOMATCH_FRAME_N;
	uint16 quality = 0;
	bool match_quality = OBJ_NOMATCH_QUALITY;

	if(lua_gettop(L) >= 3 && !lua_isnil(L, 3))
	{
		frame_n = (uint16)luaL_checkinteger(L, 3);
		match_frame_n = OBJ_MATCH_FRAME_N;
	}

	if(lua_gettop(L) >= 4 && !lua_isnil(L, 4))
	{
		quality = (uint16)luaL_checkinteger(L, 4);
		match_quality = OBJ_MATCH_QUALITY;
	}

	ObjManager *obj_manager = Game::get_game()->get_obj_manager();
	Obj *obj = obj_manager->find_obj(level, obj_n, quality, match_quality, frame_n, match_frame_n);
	if(obj != NULL)
	{
		nscript_new_obj_var(L, obj);
	}
	else
	{
		lua_pushnil(L);
	}

	lua_pushinteger(L, level);
	lua_pushboolean(L, match_frame_n);
	lua_pushboolean(L, match_quality);

	lua_pushcclosure(L, &nscript_find_obj_iter, 4);

	return 1;
}

static int nscript_timer_set(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	uint8 timer_num = (uint8)luaL_checkinteger(L, 1);
	uint8 value = (uint8)luaL_checkinteger(L, 2);

	clock->set_timer(timer_num, value);

	return 0;
}

static int nscript_timer_get(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	uint8 timer_num = (uint8)luaL_checkinteger(L, 1);

	lua_pushinteger(L, clock->get_timer(timer_num));

	return 1;
}

static int nscript_timer_update_all(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	uint8 value = (uint8)luaL_checkinteger(L, 1);

	clock->update_timers(value);

	return 0;
}

static int nscript_clock_get_minute(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	lua_pushinteger(L, clock->get_minute());

	return 1;
}

static int nscript_clock_get_hour(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	lua_pushinteger(L, clock->get_hour());

	return 1;
}

static int nscript_clock_inc(lua_State *L)
{
	GameClock *clock = Game::get_game()->get_clock();

	uint16 minutes = (uint16)luaL_checkinteger(L, 1);

	clock->inc_minute(minutes);

	return 0;
}

static int nscript_wind_set(lua_State *L)
{
	Weather *weather = Game::get_game()->get_weather();
	uint8 wind_dir = (uint8)luaL_checkinteger(L, 1);

	weather->set_wind_dir(wind_dir);

	return 0;
}

static int nscript_wind_get(lua_State *L)
{
	Weather *weather = Game::get_game()->get_weather();
	lua_pushinteger(L, weather->get_wind_dir());
	return 1;
}

static int nscript_input_select(lua_State *L)
{
	const char *allowed_chars = NULL;

	if(!lua_isnil(L, 1))
		allowed_chars = luaL_checkstring(L, 1);

	bool can_escape = lua_toboolean(L, 2);

	TextInputEffect *inputEffect = new TextInputEffect(allowed_chars, can_escape);
	AsyncEffect *e = new AsyncEffect(inputEffect);
	e->run(EFFECT_PROCESS_GUI_INPUT);

	std::string input = inputEffect->get_input();

	lua_pushstring(L, input.c_str());

	return 1;
}

static int nscript_input_select_integer(lua_State *L)
{
	const char *allowed_chars = NULL;

	if(!lua_isnil(L, 1))
		allowed_chars = luaL_checkstring(L, 1);

	bool can_escape = lua_toboolean(L, 2);

	TextInputEffect *inputEffect = new TextInputEffect(allowed_chars, can_escape);
	AsyncEffect *e = new AsyncEffect(inputEffect);
	e->run(EFFECT_PROCESS_GUI_INPUT);

	std::string input = inputEffect->get_input();

	int num = (int)strtol(input.c_str(), (char **)NULL, 10);
	lua_pushinteger(L, num);

	return 1;
}
//lua function objs_at_loc(x,y,z)
static int nscript_objs_at_loc(lua_State *L)
{
   U6Link *link = NULL;
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   
   uint16 x, y;
   uint8 z;
   
   if(nscript_get_location_from_args(L, &x, &y, &z) == false)
      return 0;

   
   U6LList *obj_list = obj_manager->get_obj_list(x, y, z);
   if(obj_list != NULL)
	  link = obj_list->start();
   
   lua_pushcfunction(L, nscript_u6llist_iter);
   
   U6Link **p_link = (U6Link **)lua_newuserdata(L, sizeof(U6Link *));
   *p_link = link;

   retainU6Link(link);

   luaL_getmetatable(L, "nuvie.U6Link");
   lua_setmetatable(L, -2);
   
   return 2;
}

static int nscript_container(lua_State *L)
{
	bool is_recursive = false;
	Obj **s_obj = (Obj **)luaL_checkudata(L, 1, "nuvie.Obj");
	Obj *obj;

	obj = *s_obj;

	U6LList *obj_list = obj->container;

    if(lua_gettop(L) >= 2)
    	is_recursive = lua_toboolean(L, 2);

    return nscript_init_u6link_iter(L, obj_list, is_recursive);
}

int nscript_init_u6link_iter(lua_State *L, U6LList *list, bool is_recursive)
{
	U6Link *link = NULL;

    if(list != NULL)
       link = list->start();

    retainU6Link(link);

    if(is_recursive)
    {
    	lua_pushcfunction(L, nscript_u6llist_iter_recursive);

    	std::stack<U6Link *> **p_stack = (std::stack<U6Link *> **)lua_newuserdata(L, sizeof(std::stack<U6Link *> *));
		*p_stack = new std::stack<U6Link *>();
		(*p_stack)->push(link);

		luaL_getmetatable(L, "nuvie.U6LinkRecursive");
    }
    else
    {
    	lua_pushcfunction(L, nscript_u6llist_iter);

		U6Link **p_link = (U6Link **)lua_newuserdata(L, sizeof(U6Link *));
		*p_link = link;

		luaL_getmetatable(L, "nuvie.U6Link");
    }

	lua_setmetatable(L, -2);

	return 2;
}

static int nscript_is_god_mode_enabled(lua_State *L)
{
	bool god_mode = Game::get_game()->is_god_mode_enabled();
	lua_pushboolean(L, god_mode);
	return 1;
}

static int nscript_set_armageddon(lua_State *L)
{
	Game::get_game()->set_armageddon((bool)lua_toboolean(L, 1));
	return 0;
}
