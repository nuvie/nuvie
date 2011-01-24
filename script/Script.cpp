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
#include "Effect.h"
#include "MsgScroll.h"
#include "Player.h"
#include "Party.h"
#include "ActorManager.h"
#include "Actor.h"
#include "Weather.h"
#include "UseCode.h"

#include "Script.h"
#include "ScriptActor.h"

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

// used for garbage collection.
//returns current object reference count. Or -1 on error.
static sint32 nscript_inc_obj_ref_count(Obj *obj);
static sint32 nscript_dec_obj_ref_count(Obj *obj);

bool nscript_get_location_from_args(lua_State *L, uint16 *x, uint16 *y, uint8 *z, int lua_stack_offset=1);

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

static bool nscript_container_new(lua_State *L, Obj *parent_obj);
static int nscript_container_gc(lua_State *L);
static int nscript_container_get(lua_State *L);
static int nscript_container_length(lua_State *L);

static const struct luaL_Reg nscript_containerlib_m[] =
{
   { "__len", nscript_container_length },
   { "__index", nscript_container_get },
   { "__gc", nscript_container_gc },
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
//no longer used -- static int nscript_get_target(lua_State *L);
static int nscript_load(lua_State *L);

static int nscript_player_get_location(lua_State *L);
static int nscript_player_get_karma(lua_State *L);
static int nscript_player_set_karma(lua_State *L);

static int nscript_party_is_in_combat_mode(lua_State *L);
static int nscript_party_set_combat_mode(lua_State *L);
static int nscript_party_move(lua_State *L);

//obj manager
static int nscript_objs_at_loc(lua_State *L);
static int nscript_map_get_obj(lua_State *L);
static int nscript_map_remove_obj(lua_State *L);
static int nscript_map_is_water(lua_State *L);
static int nscript_map_can_put(lua_State *L);

//Misc
static int nscript_eclipse_start(lua_State *L);
static int nscript_quake_start(lua_State *L);
static int nscript_explosion_start(lua_State *L);
static int nscript_projectile_anim(lua_State *L);
static int nscript_projectile_anim_multi(lua_State *L);
static int nscript_hit_anim(lua_State *L);
static int nscript_usecode_look(lua_State *L);

static int nscript_fade_out(lua_State *L);
static int nscript_fade_in(lua_State *L);

static int nscript_xor_effect(lua_State *L);

//Iterators
int nscript_u6llist_iter(lua_State *L);
int nscript_u6llist_iter_recursive(lua_State *L);
int nscript_party_iter(lua_State *L);


static int nscript_party(lua_State *L);
static int nscript_container(lua_State *L);
int nscript_init_u6link_iter(lua_State *L, U6LList *list, bool is_recursive);

Script *Script::script = NULL;

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


Script::Script(Configuration *cfg, nuvie_game_t type)
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

   luaL_newmetatable(L, "nuvie.Container");
   luaL_register(L, NULL, nscript_containerlib_m);

   lua_pushcfunction(L, nscript_load);
   lua_setglobal(L, "nuvie_load");

   nscript_init_actor(L);

   lua_pushcfunction(L, nscript_print);
   lua_setglobal(L, "print");

   lua_pushcfunction(L, nscript_party);
   lua_setglobal(L, "party_members");

   lua_pushcfunction(L, nscript_container);
   lua_setglobal(L, "container_objs");
   
   lua_pushcfunction(L, nscript_objs_at_loc);
   lua_setglobal(L, "objs_at_loc");
   
   lua_pushcfunction(L, nscript_map_get_obj);
   lua_setglobal(L, "map_get_obj");

   lua_pushcfunction(L, nscript_map_remove_obj);
   lua_setglobal(L, "map_remove_obj");

   lua_pushcfunction(L, nscript_map_is_water);
   lua_setglobal(L, "map_is_water");

   lua_pushcfunction(L, nscript_map_can_put);
   lua_setglobal(L, "map_can_put");


   lua_pushcfunction(L, nscript_player_get_location);
   lua_setglobal(L, "player_get_location");

   lua_pushcfunction(L, nscript_player_get_karma);
   lua_setglobal(L, "player_get_karma");

   lua_pushcfunction(L, nscript_player_set_karma);
   lua_setglobal(L, "player_set_karma");

   lua_pushcfunction(L, nscript_party_is_in_combat_mode);
   lua_setglobal(L, "party_is_in_combat_mode");

   lua_pushcfunction(L, nscript_party_set_combat_mode);
   lua_setglobal(L, "party_set_combat_mode");

   lua_pushcfunction(L, nscript_party_move);
   lua_setglobal(L, "party_move");

   lua_pushcfunction(L, nscript_eclipse_start);
   lua_setglobal(L, "eclipse_start");

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

   lua_pushcfunction(L, nscript_xor_effect);
   lua_setglobal(L, "xor_effect");

   seed_random();

   lua_getglobal(L, "package");
   lua_pushstring(L, "path");
   lua_gettable(L, -2);

   path = lua_tolstring(L, -1, &len);
   DEBUG(0, LEVEL_INFORMATIONAL, "lua path = %s\n", path);

   run_script("init = nuvie_load(\"u6/init.lua\"); init()");

}

Script::~Script()
{
   if(L)
      lua_close(L);
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
   }

   return true;
}

bool Script::call_actor_update_all()
{
   lua_getglobal(L, "actor_update_all");

   if(lua_pcall(L, 0, 0, 0) != 0)
   {
      DEBUG(0, LEVEL_ERROR, "Script Error: actor_update_all() %s\n", luaL_checkstring(L, -1));
      return false;
   }
   
   return true;
}

bool Script::call_actor_init(Actor *actor)
{
   lua_getglobal(L, "actor_init");
   nscript_new_actor_var(L, actor->get_actor_num());

   if(lua_pcall(L, 1, 0, 0) != 0)
   {
      DEBUG(0, LEVEL_ERROR, "Script Error: actor_init() %s\n", luaL_checkstring(L, -1));
      return false;
   }

   return true;
}

bool Script::call_actor_attack(Actor *actor, MapCoord location, Obj *weapon)
{
   lua_getglobal(L, "actor_attack");
   nscript_new_actor_var(L, actor->get_actor_num());
   //nscript_new_actor_var(L, foe->get_actor_num());
   lua_pushnumber(L, (lua_Number)location.x);
   lua_pushnumber(L, (lua_Number)location.y);
   lua_pushnumber(L, (lua_Number)location.z);
   if(weapon == NULL)
      nscript_new_actor_var(L, actor->get_actor_num());
   else
      nscript_obj_new(L, weapon);
   if(lua_pcall(L, 5, 0, 0) != 0)
   {
      DEBUG(0, LEVEL_ERROR, "Script Error: actor_attack() %s\n", luaL_checkstring(L, -1));
      return false;
   }

   Game::get_game()->get_map_window()->updateBlacking(); // the script might have updated the blocking objects. eg broken a door.
   return true;
}

bool Script::call_look_obj(Obj *obj)
{
   lua_getglobal(L, "look_obj");

   nscript_obj_new(L, obj);
   if(lua_pcall(L, 1, 1, 0) != 0)
   {
      DEBUG(0, LEVEL_ERROR, "Script Error: look_obj() %s\n", luaL_checkstring(L, -1));
      return false;
   }
   
   return lua_toboolean(L,-1);
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
      *x = (uint16)luaL_checkinteger(L, lua_stack_offset);
      *y = (uint16)luaL_checkinteger(L, lua_stack_offset + 1);
      *z = (uint8)luaL_checkinteger(L,  lua_stack_offset + 2);
   }

   return true;
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
      lua_pushstring(L, obj_manager->get_obj_name(obj->obj_n));
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

static bool nscript_container_new(lua_State *L, Obj *parent_obj)
{
   Obj **p_obj;

   p_obj = (Obj **)lua_newuserdata(L, sizeof(Obj *));

   luaL_getmetatable(L, "nuvie.Container");
   lua_setmetatable(L, -2);

   *p_obj = parent_obj;

   nscript_inc_obj_ref_count(parent_obj);

   return true;
}

static int nscript_container_gc(lua_State *L)
{
   return nscript_obj_gc(L); //a container is just another Obj.
}

static int nscript_container_get(lua_State *L)
{
   Obj **s_obj;
   Obj *obj;
   U6Link *link;

   int key;

   s_obj = (Obj **)lua_touserdata(L, 1);
   if(s_obj == NULL)
      return 0;

   obj = *s_obj;
   if(obj == NULL)
      return 0;

   //ptr = nscript_get_obj_ptr(s_obj);
   if(obj->container == NULL)
      return 0;

   key = lua_tointeger(L, 2);

   DEBUG(0, LEVEL_DEBUGGING, "Key = %d\n", key);

   link = obj->container->gotoPos(key - 1);

   if(link && link->data)
      return nscript_obj_new(L, (Obj *)link->data); //return container child Obj to script.

   return 0;
}

static int nscript_container_length(lua_State *L)
{
   Obj **s_obj;
   Obj *obj;
   uint32 count = 0;

   s_obj = (Obj **)lua_touserdata(L, 1);
   if(s_obj == NULL)
      return 0;

   obj = *s_obj;
   if(obj == NULL)
      return 0;

   if(obj->container != NULL)
      count = obj->container->count();

   lua_pushinteger(L, (int)count);
   return 1;
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

static int nscript_party_move(lua_State *L)
{
	Player *player = Game::get_game()->get_player();
	uint16 x, y;
	uint8 z;

	if(nscript_get_location_from_args(L, &x, &y, &z) == false)
		return 0;

	player->move(x, y, z); //FIXME should this be party move?

	return 0;
}

static int nscript_map_get_obj(lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   Obj **p_obj;
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
      p_obj = (Obj **)lua_newuserdata(L, sizeof(Obj *));

      luaL_getmetatable(L, "nuvie.Obj");
      lua_setmetatable(L, -2);

      *p_obj = obj;

      nscript_inc_obj_ref_count(obj);
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

static int nscript_map_can_put(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   uint16 x, y;
   uint8 z;

   if(nscript_get_location_from_args(L, &x, &y, &z) == false)
	  return 0;

   lua_pushboolean(L, actor_manager->can_put_actor(MapCoord(x,y,z)));

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

static int nscript_eclipse_start(lua_State *L)
{
   Weather *weather = Game::get_game()->get_weather();
   uint8 length = (uint8)luaL_checkinteger(L, 1);

   weather->start_eclipse(length);

   lua_pushboolean(L, true);
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

static int nscript_explosion_start(lua_State *L)
{
   uint16 tile_num = (uint16)luaL_checkinteger(L, 1);
   uint16 x = (uint16)luaL_checkinteger(L, 2);
   uint16 y = (uint16)luaL_checkinteger(L, 3);

   ExpEffect *effect = new ExpEffect(tile_num, MapCoord(x, y));
   AsyncEffect *e = new AsyncEffect(effect);
   e->run();

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

static int nscript_projectile_anim(lua_State *L)
{
uint16 tile_num = (uint16)luaL_checkinteger(L, 1);
   uint16 startx = (uint16)luaL_checkinteger(L, 2);
   uint16 starty = (uint16)luaL_checkinteger(L, 3);
   uint16 targetx = (uint16)luaL_checkinteger(L, 4);
   uint16 targety = (uint16)luaL_checkinteger(L, 5);
   uint16 speed = (uint16)luaL_checkinteger(L, 6);
   bool trail = (bool)luaL_checkinteger(L, 7);
   uint8 initial_tile_rotation = (uint8)luaL_checkinteger(L, 8);

   ProjectileEffect *projectile_effect = new ProjectileEffect(tile_num, MapCoord(startx,starty), MapCoord(targetx,targety), speed, trail, initial_tile_rotation);
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

   AsyncEffect *e = new AsyncEffect(new ProjectileEffect(tile_num, MapCoord(startx,starty), t, speed, trail, initial_tile_rotation));
   e->run();

   lua_pushboolean(L, true);
   return 1;
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

static int nscript_xor_effect(lua_State *L)
{
	uint16 duration = (uint16)luaL_checkinteger(L, 1);

	AsyncEffect *e = new AsyncEffect(new XorEffect(duration));
	e->run();

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
	U6Link *link;

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
