/*
 *  ScriptActor.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jun 7 2009.
 *  Copyright (c) 2009. All rights reserved.
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
#include "nuvieDefs.h"
#include "U6misc.h"

#include "ScriptActor.h"
#include "Player.h"
#include "Game.h"
#include "Effect.h"
#include "ActorManager.h"
#include "Actor.h"
#include "ViewManager.h"
#include "Converse.h"

extern bool nscript_get_location_from_args(lua_State *L, uint16 *x, uint16 *y, uint8 *z, int lua_stack_offset=1);
extern Obj *nscript_get_obj_from_args(lua_State *L, int lua_stack_offset);
extern void nscript_new_obj_var(lua_State *L, Obj *obj);
extern int nscript_obj_new(lua_State *L, Obj *obj);
extern int nscript_u6llist_iter(lua_State *L);
extern int nscript_init_u6link_iter(lua_State *L, U6LList *list, bool is_recursive);

bool nscript_new_actor_var(lua_State *L, uint16 actor_num);

static int nscript_actor_new(lua_State *L);
static int nscript_actor_clone(lua_State *L);
static int nscript_get_actor_from_num(lua_State *L);

Actor *nscript_get_actor_from_args(lua_State *L, int lua_stack_offset=1);
static int nscript_actor_set(lua_State *L);
static int nscript_actor_get(lua_State *L);
static int nscript_get_player_actor(lua_State *L);
static int nscript_actor_kill(lua_State *L);
static int nscript_actor_hit(lua_State *L);
static int nscript_actor_resurrect(lua_State *L);
static int nscript_actor_inv_add_obj(lua_State *L);
static int nscript_actor_inv_remove_obj(lua_State *L);
static int nscript_actor_inv_remove_obj_qty(lua_State *L);
static int nscript_actor_inv_ready_obj(lua_State *L);
static int nscript_actor_inv_has_obj_n(lua_State *L);
static int nscript_actor_inv_get_obj_n(lua_State *L);
static int nscript_actor_inv_get_obj_total_qty(lua_State *L);
static int nscript_actor_move(lua_State *L);
static int nscript_actor_walk_path(lua_State *L);
static int nscript_actor_is_at_scheduled_location(lua_State *L);
static int nscript_actor_can_carry_obj(lua_State *L);
static int nscript_actor_black_fade_effect(lua_State *L);
static int nscript_actor_show_portrait(lua_State *L);
static int nscript_actor_hide_portrait(lua_State *L);
static int nscript_actor_talk(lua_State *L);

static const struct luaL_Reg nscript_actorlib_f[] =
{
   { "new", nscript_actor_new },
   { "clone", nscript_actor_clone },
   { "kill", nscript_actor_kill },
   { "hit", nscript_actor_hit },
   { "resurrect", nscript_actor_resurrect },
   { "move", nscript_actor_move },
   { "walk_path", nscript_actor_walk_path },
   { "get", nscript_get_actor_from_num },
   { "get_player_actor", nscript_get_player_actor },
   { "inv_add_obj", nscript_actor_inv_add_obj },
   { "inv_remove_obj", nscript_actor_inv_remove_obj },
   { "inv_remove_obj_qty", nscript_actor_inv_remove_obj_qty },   
   { "inv_ready_obj", nscript_actor_inv_ready_obj },
   { "inv_has_obj_n", nscript_actor_inv_has_obj_n },
   { "inv_get_obj_n", nscript_actor_inv_get_obj_n },
   { "inv_get_obj_total_qty", nscript_actor_inv_get_obj_total_qty },
   { "is_at_scheduled_location", nscript_actor_is_at_scheduled_location },
   { "can_carry_obj", nscript_actor_can_carry_obj },
   { "black_fade_effect", nscript_actor_black_fade_effect },
   { "show_portrait", nscript_actor_show_portrait },
   { "hide_portrait", nscript_actor_hide_portrait },
   { "talk", nscript_actor_talk },

   { NULL, NULL }
};
static const struct luaL_Reg nscript_actorlib_m[] =
{
   { "__index", nscript_actor_get },
   { "__newindex", nscript_actor_set },
   { NULL, NULL }
};


//Actor variables
static const char *actor_set_vars[] =
{
   "align",
   "asleep",
   "base_obj_n",
   "charmed",
   "combat_mode",
   "corpser_flag",
   "cursed",
   "dex",
   "direction",
   "exp",
   "frame_n",
   "hit_flag",
   "hp",
   "int",
   "level",
   "magic",
   "mpts",
   "obj_n",
   "old_align",
   "paralyzed",
   "poisoned",
   "protected",
   "str",
   "visible",
   "wt",
   "x",
   "y",
   "z"
};

//Actor variables
static const char *actor_get_vars[] =
{
   "actor_num",
   "align",
   "alive",
   "asleep",
   "charmed",
   "combat_mode",
   "corpser_flag",
   "cursed",
   "dex",
   "direction",
   "exp",
   "frame_n",
   "hit_flag",
   "hp",
   "in_party",
   "int",
   "level",
   "luatype",
   "magic",
   "max_hp",
   "mpts",
   "name",
   "obj_n",
   "old_align",
   "old_frame_n",
   "paralyzed",
   "poisoned",
   "protected",
   "sched_loc",
   "sched_wt",
   "str",
   "temp",
   "tile_num",
   "visible",
   "wt",
   "x",
   "y",
   "z"
};

//Actor set
static int nscript_actor_set_align(Actor *actor, lua_State *L);
static int nscript_actor_set_asleep_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_base_obj_n(Actor *actor, lua_State *L);
static int nscript_actor_set_charmed_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_combat_mode(Actor *actor, lua_State *L);
static int nscript_actor_set_corpser_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_cursed_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_dexterity(Actor *actor, lua_State *L);
static int nscript_actor_set_direction(Actor *actor, lua_State *L);
static int nscript_actor_set_exp(Actor *actor, lua_State *L);
static int nscript_actor_set_frame_n(Actor *actor, lua_State *L);
static int nscript_actor_set_hit(Actor *actor, lua_State *L);
static int nscript_actor_set_hp(Actor *actor, lua_State *L);
static int nscript_actor_set_intelligence(Actor *actor, lua_State *L);
static int nscript_actor_set_level(Actor *actor, lua_State *L);
static int nscript_actor_set_magic(Actor *actor, lua_State *L);
static int nscript_actor_set_movement_pts(Actor *actor, lua_State *L);
static int nscript_actor_set_obj_n(Actor *actor, lua_State *L);
static int nscript_actor_set_old_align(Actor *actor, lua_State *L);
static int nscript_actor_set_paralyzed_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_poisoned_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_protected_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_strength(Actor *actor, lua_State *L);
static int nscript_actor_set_visible_flag(Actor *actor, lua_State *L);
static int nscript_actor_set_worktype(Actor *actor, lua_State *L);
static int nscript_actor_set_x(Actor *actor, lua_State *L);
static int nscript_actor_set_y(Actor *actor, lua_State *L);
static int nscript_actor_set_z(Actor *actor, lua_State *L);

int (*actor_set_func[])(Actor *, lua_State *) =
{
   nscript_actor_set_align,
   nscript_actor_set_asleep_flag,
   nscript_actor_set_base_obj_n,
   nscript_actor_set_charmed_flag,
   nscript_actor_set_combat_mode,
   nscript_actor_set_corpser_flag,
   nscript_actor_set_cursed_flag,
   nscript_actor_set_dexterity,
   nscript_actor_set_direction,
   nscript_actor_set_exp,
   nscript_actor_set_frame_n,
   nscript_actor_set_hit,
   nscript_actor_set_hp,
   nscript_actor_set_intelligence,
   nscript_actor_set_level,
   nscript_actor_set_magic,
   nscript_actor_set_movement_pts,
   nscript_actor_set_obj_n,
   nscript_actor_set_old_align,
   nscript_actor_set_paralyzed_flag,
   nscript_actor_set_poisoned_flag,
   nscript_actor_set_protected_flag,
   nscript_actor_set_strength,
   nscript_actor_set_visible_flag,
   nscript_actor_set_worktype,
   nscript_actor_set_x,
   nscript_actor_set_y,
   nscript_actor_set_z
};

//Actor get
static int nscript_actor_get_actor_num(Actor *actor, lua_State *L);
static int nscript_actor_get_align(Actor *actor, lua_State *L);
static int nscript_actor_get_alive(Actor *actor, lua_State *L);
static int nscript_actor_get_asleep_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_charmed_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_combat_mode(Actor *actor, lua_State *L);
static int nscript_actor_get_corpser_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_cursed_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_dexterity(Actor *actor, lua_State *L);
static int nscript_actor_get_direction(Actor *actor, lua_State *L);
static int nscript_actor_get_exp(Actor *actor, lua_State *L);
static int nscript_actor_get_frame_n(Actor *actor, lua_State *L);
static int nscript_actor_get_hit_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_hp(Actor *actor, lua_State *L);
static int nscript_actor_get_in_party_status(Actor *actor, lua_State *L);
static int nscript_actor_get_intelligence(Actor *actor, lua_State *L);
static int nscript_actor_get_level(Actor *actor, lua_State *L);
static int nscript_actor_get_luatype(Actor *actor, lua_State *L);
static int nscript_actor_get_magic(Actor *actor, lua_State *L);
static int nscript_actor_get_max_hp(Actor *actor, lua_State *L);
static int nscript_actor_get_movement_pts(Actor *actor, lua_State *L);
static int nscript_actor_get_name(Actor *actor, lua_State *L);
static int nscript_actor_get_obj_n(Actor *actor, lua_State *L);
static int nscript_actor_get_old_align(Actor *actor, lua_State *L);
static int nscript_actor_get_old_frame_n(Actor *actor, lua_State *L);
static int nscript_actor_get_paralyzed_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_poisoned_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_protected_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_sched_loc(Actor *actor, lua_State *L);
static int nscript_actor_get_sched_worktype(Actor *actor, lua_State *L);
static int nscript_actor_get_strength(Actor *actor, lua_State *L);
static int nscript_actor_get_temp_status(Actor *actor, lua_State *L);
static int nscript_actor_get_tile_num(Actor *actor, lua_State *L);
static int nscript_actor_get_visible_flag(Actor *actor, lua_State *L);
static int nscript_actor_get_worktype(Actor *actor, lua_State *L);
static int nscript_actor_get_x(Actor *actor, lua_State *L);
static int nscript_actor_get_y(Actor *actor, lua_State *L);
static int nscript_actor_get_z(Actor *actor, lua_State *L);

int (*actor_get_func[])(Actor *, lua_State *) =
{
   nscript_actor_get_actor_num,
   nscript_actor_get_align,
   nscript_actor_get_alive,
   nscript_actor_get_asleep_flag,
   nscript_actor_get_charmed_flag,
   nscript_actor_get_combat_mode,
   nscript_actor_get_corpser_flag,
   nscript_actor_get_cursed_flag,
   nscript_actor_get_dexterity,
   nscript_actor_get_direction,
   nscript_actor_get_exp,
   nscript_actor_get_frame_n,
   nscript_actor_get_hit_flag,
   nscript_actor_get_hp,
   nscript_actor_get_in_party_status,
   nscript_actor_get_intelligence,
   nscript_actor_get_level,
   nscript_actor_get_luatype,
   nscript_actor_get_magic,
   nscript_actor_get_max_hp,
   nscript_actor_get_movement_pts,
   nscript_actor_get_name,
   nscript_actor_get_obj_n,
   nscript_actor_get_old_align,
   nscript_actor_get_old_frame_n,
   nscript_actor_get_paralyzed_flag,
   nscript_actor_get_poisoned_flag,
   nscript_actor_get_protected_flag,
   nscript_actor_get_sched_loc,
   nscript_actor_get_sched_worktype,
   nscript_actor_get_strength,
   nscript_actor_get_temp_status,
   nscript_actor_get_tile_num,
   nscript_actor_get_visible_flag,
   nscript_actor_get_worktype,
   nscript_actor_get_x,
   nscript_actor_get_y,
   nscript_actor_get_z
};


static int nscript_map_get_actor(lua_State *L);
static int nscript_update_actor_schedules(lua_State *L);

static int nscript_actor_inv(lua_State *L);

void nscript_init_actor(lua_State *L)
{
   luaL_newmetatable(L, "nuvie.Actor");

   luaL_register(L, NULL, nscript_actorlib_m);

   luaL_register(L, "Actor", nscript_actorlib_f);

   lua_pushcfunction(L, nscript_map_get_actor);
   lua_setglobal(L, "map_get_actor");

   lua_pushcfunction(L, nscript_update_actor_schedules);
   lua_setglobal(L, "update_actor_schedules");

   lua_pushcfunction(L, nscript_actor_inv);
   lua_setglobal(L, "actor_inventory");
}

bool nscript_new_actor_var(lua_State *L, uint16 actor_num)
{
   uint16 *userdata;

   userdata = (uint16 *)lua_newuserdata(L, sizeof(uint16));

   luaL_getmetatable(L, "nuvie.Actor");
   lua_setmetatable(L, -2);

   *userdata = actor_num;

   return true;
}

static int nscript_actor_new(lua_State *L)
{
   Actor *actor;
   uint16 obj_n = 0;
   uint16 x = 0;
   uint16 y = 0;
   uint8 z = 0;
   uint8 alignment = ACTOR_ALIGNMENT_NEUTRAL;
   uint8 worktype = 0; //motionless

   int nargs = lua_gettop(L);

   if(nargs > 1) // do we have arguments?
   {
      uint8 i = nargs;

      if(i)
      {
         if(!lua_isnil(L, 1))
            obj_n = (uint16)lua_tointeger(L, 1);
         i--;
      }

      if(i)
      {
         if(!lua_isnil(L, 2))
            x = (uint16)lua_tointeger(L, 2);
         i--;
      }

      if(i)
      {
         if(!lua_isnil(L, 3))
            y = (uint16)lua_tointeger(L, 3);
         i--;
      }

      if(i)
      {
         if(!lua_isnil(L, 4))
            z = (uint8)lua_tointeger(L, 4);
         i--;
      }

      if(i)
      {
         if(!lua_isnil(L, 5))
            alignment = (uint8)lua_tointeger(L, 5);
         i--;
      }

      if(i)
      {
         if(!lua_isnil(L, 6))
            worktype = (uint8)lua_tointeger(L, 6);
         i--;
      }

      //init here.
      if(Game::get_game()->get_actor_manager()->create_temp_actor(obj_n, x, y, z, alignment, worktype, &actor) == false)
         return 0;

      //create the new lua Actor variable
      if(nscript_new_actor_var(L, actor->get_actor_num()) == false)
         return 0;
   }

   return 1;
}

static int nscript_actor_clone(lua_State *L)
{
   Actor *actor, *new_actor;
   uint16 x, y;
   uint8 z;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
	  return 0;

   if(nscript_get_location_from_args(L, &x, &y, &z, 2) == false)
	  return 0;

	if(Game::get_game()->get_actor_manager()->clone_actor(actor, &new_actor, MapCoord(x,y,z)))
	{
	  if(nscript_new_actor_var(L, actor->get_actor_num()) == true)
		 return 1;
	}

	return 0;
}

static int nscript_get_actor_from_num(lua_State *L)
{
   uint16 actor_num;
   actor_num = (uint16)lua_tointeger(L, 1);

   return nscript_new_actor_var(L, actor_num);
}

Actor *nscript_get_actor_from_args(lua_State *L, int lua_stack_offset)
{
   uint16 *actor_num;
   Actor *actor = NULL;

   actor_num = (uint16 *)luaL_checkudata(L, lua_stack_offset, "nuvie.Actor");

   if(actor_num != NULL)
      actor = Game::get_game()->get_actor_manager()->get_actor(*actor_num);

   return actor;
}


static int nscript_get_player_actor(lua_State *L)
{
	Actor *player_actor = Game::get_game()->get_player()->get_actor();

	return nscript_new_actor_var(L, player_actor->get_actor_num());
}

static int nscript_actor_set(lua_State *L)
{
   Actor *actor;
   const char *key;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   key = lua_tostring(L, 2);

   int idx = str_bsearch(actor_set_vars, sizeof(actor_set_vars) / sizeof(actor_set_vars[0]), (char *)key);
   if(idx == -1)
      return 0;

   (*actor_set_func[idx])(actor, L);

   return 0;
}


static int nscript_actor_set_align(Actor *actor, lua_State *L)
{
   actor->set_alignment((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_asleep_flag(Actor *actor, lua_State *L)
{
	actor->set_asleep(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_charmed_flag(Actor *actor, lua_State *L)
{
	actor->set_charmed(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_combat_mode(Actor *actor, lua_State *L)
{
   actor->set_combat_mode((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_corpser_flag(Actor *actor, lua_State *L)
{
	actor->set_corpser_flag(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_cursed_flag(Actor *actor, lua_State *L)
{
	actor->set_cursed(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_dexterity(Actor *actor, lua_State *L)
{
   actor->set_dexterity((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_direction(Actor *actor, lua_State *L)
{
   actor->set_direction((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_exp(Actor *actor, lua_State *L)
{
   actor->set_exp((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_frame_n(Actor *actor, lua_State *L)
{
   actor->set_frame_n((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_hp(Actor *actor, lua_State *L)
{
   actor->set_hp((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_hit(Actor *actor, lua_State *L)
{
   actor->set_hit_flag((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_intelligence(Actor *actor, lua_State *L)
{
   actor->set_intelligence((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_level(Actor *actor, lua_State *L)
{
   actor->set_level((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_magic(Actor *actor, lua_State *L)
{
   actor->set_magic((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_movement_pts(Actor *actor, lua_State *L)
{
   actor->set_moves_left((sint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_obj_n(Actor *actor, lua_State *L)
{
   actor->set_obj_n((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_old_align(Actor *actor, lua_State *L)
{
	actor->set_old_alignment((sint8)lua_tointeger(L, 3));
	return 0;
}

static int nscript_actor_set_base_obj_n(Actor *actor, lua_State *L)
{
   actor->change_base_obj_n((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_paralyzed_flag(Actor *actor, lua_State *L)
{
	actor->set_paralyzed(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_poisoned_flag(Actor *actor, lua_State *L)
{
	actor->set_poisoned(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_protected_flag(Actor *actor, lua_State *L)
{
	actor->set_protected(lua_toboolean(L, 3));
	return 0;
}

static int nscript_actor_set_strength(Actor *actor, lua_State *L)
{
   actor->set_strength((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_visible_flag(Actor *actor, lua_State *L)
{
	actor->set_invisible(!lua_toboolean(L, 3)); //negate value before passing back to actor.
	return 0;
}

static int nscript_actor_set_worktype(Actor *actor, lua_State *L)
{
   actor->set_worktype((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_x(Actor *actor, lua_State *L)
{
   //actor->set_x((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_y(Actor *actor, lua_State *L)
{
   //actor->set_y((uint16)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_set_z(Actor *actor, lua_State *L)
{
   //actor->set_z((uint8)lua_tointeger(L, 3));
   return 0;
}

static int nscript_actor_get(lua_State *L)
{
   Actor *actor;
   const char *key;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   key = lua_tostring(L, 2);

   int idx = str_bsearch(actor_get_vars, sizeof(actor_get_vars) / sizeof(actor_get_vars[0]), (char *)key);
   if(idx == -1)
      return 0;

   return (*actor_get_func[idx])(actor, L);
}

static int nscript_actor_get_actor_num(Actor *actor, lua_State *L)
{
	lua_pushinteger(L, actor->get_actor_num()); return 1;
}

static int nscript_actor_get_align(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_alignment()); return 1;
}

static int nscript_actor_get_alive(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, actor->is_alive()); return 1;
}

static int nscript_actor_get_asleep_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_sleeping()); return 1;
}

static int nscript_actor_get_cursed_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_cursed()); return 1;
}

static int nscript_actor_get_combat_mode(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_combat_mode()); return 1;
}

static int nscript_actor_get_charmed_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_charmed()); return 1;
}

static int nscript_actor_get_corpser_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->get_corpser_flag()); return 1;
}

static int nscript_actor_get_dexterity(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_dexterity()); return 1;
}

static int nscript_actor_get_direction(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_direction()); return 1;
}

static int nscript_actor_get_exp(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_exp()); return 1;
}

static int nscript_actor_get_frame_n(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_frame_n()); return 1;
}

static int nscript_actor_get_hit_flag(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, (int)actor->is_hit()); return 1;
}

static int nscript_actor_get_hp(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_hp()); return 1;
}

static int nscript_actor_get_in_party_status(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, (int)actor->is_in_party()); return 1;
}

static int nscript_actor_get_intelligence(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_intelligence()); return 1;
}

static int nscript_actor_get_level(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_level()); return 1;
}

static int nscript_actor_get_luatype(Actor *actor, lua_State *L)
{
   lua_pushstring(L, "actor"); return 1;
}

static int nscript_actor_get_magic(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_magic()); return 1;
}

static int nscript_actor_get_max_hp(Actor *actor, lua_State *L)
{
	lua_pushinteger(L, actor->get_maxhp()); return 1;
}

static int nscript_actor_get_movement_pts(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_moves_left()); return 1;
}

static int nscript_actor_get_name(Actor *actor, lua_State *L)
{
   lua_pushstring(L, actor->get_name()); return 1;
}

static int nscript_actor_get_obj_n(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_obj_n()); return 1;
}

static int nscript_actor_get_old_frame_n(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_old_frame_n()); return 1;
}

static int nscript_actor_get_old_align(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_old_alignment()); return 1;
}

static int nscript_actor_get_paralyzed_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_paralyzed()); return 1;
}

static int nscript_actor_get_poisoned_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_poisoned()); return 1;
}

static int nscript_actor_get_protected_flag(Actor *actor, lua_State *L)
{
	lua_pushboolean(L, (int)actor->is_protected()); return 1;
}

static int nscript_actor_get_sched_loc(Actor *actor, lua_State *L)
{
   MapCoord sched_loc;
   
   if(actor->get_schedule_location(&sched_loc) == false)
      return 0;

   lua_newtable(L);
   lua_pushstring(L, "x");
   lua_pushinteger(L, sched_loc.x);
   lua_settable(L, -3);
   
   lua_pushstring(L, "y");
   lua_pushinteger(L, sched_loc.y);
   lua_settable(L, -3);
   
   lua_pushstring(L, "z");
   lua_pushinteger(L, sched_loc.z);
   lua_settable(L, -3);
   
   return 1;
}

static int nscript_actor_get_sched_worktype(Actor *actor, lua_State *L)
{
	lua_pushinteger(L, actor->get_sched_worktype()); return 1;
}

static int nscript_actor_get_strength(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_strength()); return 1;
}

static int nscript_actor_get_temp_status(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, (int)actor->is_temp()); return 1;
}

static int nscript_actor_get_tile_num(Actor *actor, lua_State *L)
{
   ObjManager *obj_manager = Game::get_game()->get_obj_manager();
   Tile *tile = obj_manager->get_obj_tile(actor->get_obj_n(), actor->get_frame_n());

   lua_pushinteger(L, (int)tile->tile_num); return 1;
}

static int nscript_actor_get_visible_flag(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, (int)(actor->is_invisible() ? false : true)); return 1;
}

static int nscript_actor_get_worktype(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_worktype()); return 1;
}

static int nscript_actor_get_x(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_x()); return 1;
}

static int nscript_actor_get_y(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_y()); return 1;
}

static int nscript_actor_get_z(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_z()); return 1;
}

static int nscript_actor_kill(lua_State *L)
{
   Actor *actor;
   bool create_body = true;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   if(lua_gettop(L) >= 2)
	   create_body = (bool)lua_toboolean(L, 2);

   actor->die(create_body);

   return 0;
}

static int nscript_actor_hit(lua_State *L)
{
   Actor *actor;
   uint8 damage;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   damage = (uint8)luaL_checkinteger(L, 2);

   actor->hit(damage, true); //force hit

   return 0;
}

static int nscript_actor_move(lua_State *L)
{
   Actor *actor;
   uint16 x, y;
   uint8 z;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   if(nscript_get_location_from_args(L, &x, &y, &z, 2) == false)
      return 0;

   lua_pushboolean(L, (int)actor->move(x, y, z));

   return 1;
}

static int nscript_actor_walk_path(lua_State *L)
{
	   Actor *actor = nscript_get_actor_from_args(L);
	   if(actor == NULL)
	      return 0;

	   actor->update(); //FIXME this should be specific to pathfinding.

	   return 0;
}

static int nscript_actor_is_at_scheduled_location(lua_State *L)
{
	   Actor *actor = nscript_get_actor_from_args(L);
	   if(actor == NULL)
	      return 0;

	   lua_pushboolean(L, actor->is_at_scheduled_location());
	   return 1;
}

static int nscript_actor_can_carry_obj(lua_State *L)
{
	   Actor *actor = nscript_get_actor_from_args(L);
	   if(actor == NULL)
	      return 0;

	   Obj *obj = nscript_get_obj_from_args(L, 2);
	   if(obj == NULL)
		   return 0;

	   lua_pushboolean(L, (int)actor->can_carry_object(obj));
	   return 1;
}

static int nscript_actor_black_fade_effect(lua_State *L)
{
	   Actor *actor = nscript_get_actor_from_args(L);
	   uint8 fade_color = (uint8)lua_tointeger(L, 2);
	   uint16 fade_speed = (uint8)lua_tointeger(L, 3);

	   if(actor != NULL)
	   {
		   AsyncEffect *e = new AsyncEffect(new TileBlackFadeEffect(actor, fade_color, fade_speed));
		   e->run();
	   }

	   return 0;
}

static int nscript_actor_show_portrait(lua_State *L)
{
	Actor *actor = nscript_get_actor_from_args(L);
	if(actor == NULL)
		return 0;

	Game::get_game()->get_view_manager()->set_portrait_mode(actor, actor->get_name());

	return 0;
}

static int nscript_actor_hide_portrait(lua_State *L)
{
	Game::get_game()->get_view_manager()->set_party_mode();

	return 0;
}

static int nscript_actor_talk(lua_State *L)
{
	Actor *actor = nscript_get_actor_from_args(L);
	if(actor == NULL)
		return 0;

	Game::get_game()->get_converse()->start(actor);
	return 0;
}

static int nscript_actor_resurrect(lua_State *L)
{
   Actor *actor;
   MapCoord loc;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   if(nscript_get_location_from_args(L, &loc.x, &loc.y, &loc.z, 2) == false)
      return 0;

   actor->resurrect(loc);

   return 0;
}

static int nscript_actor_inv_add_obj(lua_State *L)
{
   Actor *actor;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   Obj **s_obj = (Obj **)luaL_checkudata(L, 2, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;


   actor->inventory_add_object(obj, NULL, false);

   return 0;
}

static int nscript_actor_inv_remove_obj(lua_State *L)
{
   Actor *actor;
   
   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;
   
   Obj **s_obj = (Obj **)luaL_checkudata(L, 2, "nuvie.Obj");
   Obj *obj;
   
   obj = *s_obj;
   
   
   actor->inventory_remove_obj(obj);
   
   return 0;
}

static int nscript_actor_inv_remove_obj_qty(lua_State *L)
{
   Actor *actor;
   
   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;
   
   uint16 obj_n = (uint16)lua_tointeger(L, 2);
   uint16 qty = (uint16)lua_tointeger(L, 3);
   
   
   lua_pushinteger(L, actor->inventory_del_object(obj_n, qty, 0));
   
   return 1;
}

static int nscript_actor_inv_ready_obj(lua_State *L)
{
   Actor *actor;
   MapCoord loc;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   Obj **s_obj = (Obj **)luaL_checkudata(L, 2, "nuvie.Obj");
   Obj *obj;

   obj = *s_obj;

   actor->add_readied_object(obj);

   return 0;
}

static int nscript_actor_inv_has_obj_n(lua_State *L)
{
   Actor *actor;
   uint16 obj_n;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   obj_n = (uint16)luaL_checkinteger(L, 2);

   lua_pushboolean(L, (int)actor->inventory_has_object(obj_n, 0, false));

   return 1;
}

static int nscript_actor_inv_get_obj_n(lua_State *L)
{
   Actor *actor;
   uint16 obj_n;
   Obj *obj;
   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   obj_n = (uint16)luaL_checkinteger(L, 2);

   obj = actor->inventory_get_object(obj_n, 0, false);

   if(obj)
   {
	   nscript_new_obj_var(L, obj);
	   return 1;
   }

   return 0;
}

static int nscript_actor_inv_get_obj_total_qty(lua_State *L)
{
   Actor *actor;
   uint16 obj_n;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   obj_n = (uint16)luaL_checkinteger(L, 2);

   lua_pushinteger(L, actor->inventory_count_object(obj_n));

   return 1;
}

static int nscript_map_get_actor(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   Actor *actor;

   uint16 x, y;
   uint8 z;

   if(nscript_get_location_from_args(L, &x, &y, &z) == false)
      return 0;

   actor = actor_manager->get_actor(x, y, z);

   if(actor == NULL)
      return 0;

   if(nscript_new_actor_var(L, actor->get_actor_num()) == false)
      return 0;

   return 1;
}

static int nscript_update_actor_schedules(lua_State *L)
{
	ActorManager *actor_manager = Game::get_game()->get_actor_manager();
	actor_manager->updateSchedules();
	return 0;
}

//lua function actor_inventory(actor, is_recursive)
static int nscript_actor_inv(lua_State *L)
{
   Actor *actor;
   bool is_recursive = false;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   if(lua_gettop(L) >= 2)
   	  is_recursive = lua_toboolean(L, 2);

   U6LList *inv = actor->get_inventory_list();

   return nscript_init_u6link_iter(L, inv, is_recursive);
}
