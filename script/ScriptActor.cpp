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
#include "Game.h"
#include "ActorManager.h"
#include "Actor.h"

extern bool nscript_get_location_from_args(lua_State *L, uint16 *x, uint16 *y, uint8 *z, int lua_stack_offset=1);
extern int nscript_obj_new(lua_State *L, Obj *obj);
extern int nscript_u6llist_iter(lua_State *L);

bool nscript_new_actor_var(lua_State *L, uint16 actor_num);

static int nscript_actor_new(lua_State *L);
static int nscript_actor_clone(lua_State *L);
static int nscript_get_actor_from_num(lua_State *L);

inline Actor *nscript_get_actor_from_args(lua_State *L, int lua_stack_offset=1);
static int nscript_actor_set(lua_State *L);
static int nscript_actor_get(lua_State *L);
static int nscript_actor_kill(lua_State *L);
static int nscript_actor_hit(lua_State *L);
static int nscript_actor_resurrect(lua_State *L);
static int nscript_actor_inv_add_obj(lua_State *L);
static int nscript_actor_inv_remove_obj(lua_State *L);
static int nscript_actor_inv_remove_obj_qty(lua_State *L);
static int nscript_actor_inv_ready_obj(lua_State *L);
static int nscript_actor_inv_has_obj_n(lua_State *L);
static int nscript_actor_move(lua_State *L);

static const struct luaL_Reg nscript_actorlib_f[] =
{
   { "new", nscript_actor_new },
   { "clone", nscript_actor_clone },
   { "kill", nscript_actor_kill },
   { "hit", nscript_actor_hit },
   { "resurrect", nscript_actor_resurrect },
   { "move", nscript_actor_move },
   { "get", nscript_get_actor_from_num },
   { "inv_add_obj", nscript_actor_inv_add_obj },
   { "inv_remove_obj", nscript_actor_inv_remove_obj },
   { "inv_remove_obj_qty", nscript_actor_inv_remove_obj_qty },   
   { "inv_ready_obj", nscript_actor_inv_ready_obj },
   { "inv_has_obj_n", nscript_actor_inv_has_obj_n },

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
   "combat_mode",
   "dex",
   "direction",
   "exp",
   "frame_n",
   "hp",
   "int",
   "level",
   "magic",
   "mpts",
   "obj_n",
   "str",
   "wt",
   "x",
   "y",
   "z"
};

//Actor variables
static const char *actor_get_vars[] =
{
   "align",
   "alive",
   "combat_mode",
   "dex",
   "direction",
   "exp",
   "frame_n",
   "hp",
   "in_party",
   "int",
   "level",
   "luatype",
   "magic",
   "mpts",
   "name",
   "obj_n",
   "sched_loc",
   "str",
   "visible",
   "wt",
   "x",
   "y",
   "z"
};

//Actor set
static int nscript_actor_set_align(Actor *actor, lua_State *L);
static int nscript_actor_set_combat_mode(Actor *actor, lua_State *L);
static int nscript_actor_set_dexterity(Actor *actor, lua_State *L);
static int nscript_actor_set_direction(Actor *actor, lua_State *L);
static int nscript_actor_set_exp(Actor *actor, lua_State *L);
static int nscript_actor_set_frame_n(Actor *actor, lua_State *L);
static int nscript_actor_set_hp(Actor *actor, lua_State *L);
static int nscript_actor_set_intelligence(Actor *actor, lua_State *L);
static int nscript_actor_set_level(Actor *actor, lua_State *L);
static int nscript_actor_set_magic(Actor *actor, lua_State *L);
static int nscript_actor_set_movement_pts(Actor *actor, lua_State *L);
static int nscript_actor_set_obj_n(Actor *actor, lua_State *L);
static int nscript_actor_set_strength(Actor *actor, lua_State *L);
static int nscript_actor_set_worktype(Actor *actor, lua_State *L);
static int nscript_actor_set_x(Actor *actor, lua_State *L);
static int nscript_actor_set_y(Actor *actor, lua_State *L);
static int nscript_actor_set_z(Actor *actor, lua_State *L);

int (*actor_set_func[])(Actor *, lua_State *) =
{
   nscript_actor_set_align,
   nscript_actor_set_combat_mode,
   nscript_actor_set_dexterity,
   nscript_actor_set_direction,
   nscript_actor_set_exp,
   nscript_actor_set_frame_n,
   nscript_actor_set_hp,
   nscript_actor_set_intelligence,
   nscript_actor_set_level,
   nscript_actor_set_magic,
   nscript_actor_set_movement_pts,
   nscript_actor_set_obj_n,
   nscript_actor_set_strength,
   nscript_actor_set_worktype,
   nscript_actor_set_x,
   nscript_actor_set_y,
   nscript_actor_set_z
};

//Actor get
static int nscript_actor_get_align(Actor *actor, lua_State *L);
static int nscript_actor_get_alive(Actor *actor, lua_State *L);
static int nscript_actor_get_combat_mode(Actor *actor, lua_State *L);
static int nscript_actor_get_dexterity(Actor *actor, lua_State *L);
static int nscript_actor_get_direction(Actor *actor, lua_State *L);
static int nscript_actor_get_exp(Actor *actor, lua_State *L);
static int nscript_actor_get_frame_n(Actor *actor, lua_State *L);
static int nscript_actor_get_hp(Actor *actor, lua_State *L);
static int nscript_actor_get_in_party_status(Actor *actor, lua_State *L);
static int nscript_actor_get_intelligence(Actor *actor, lua_State *L);
static int nscript_actor_get_level(Actor *actor, lua_State *L);
static int nscript_actor_get_luatype(Actor *actor, lua_State *L);
static int nscript_actor_get_magic(Actor *actor, lua_State *L);
static int nscript_actor_get_movement_pts(Actor *actor, lua_State *L);
static int nscript_actor_get_name(Actor *actor, lua_State *L);
static int nscript_actor_get_obj_n(Actor *actor, lua_State *L);
static int nscript_actor_get_sched_loc(Actor *actor, lua_State *L);
static int nscript_actor_get_strength(Actor *actor, lua_State *L);
static int nscript_actor_get_visible(Actor *actor, lua_State *L);
static int nscript_actor_get_worktype(Actor *actor, lua_State *L);
static int nscript_actor_get_x(Actor *actor, lua_State *L);
static int nscript_actor_get_y(Actor *actor, lua_State *L);
static int nscript_actor_get_z(Actor *actor, lua_State *L);

int (*actor_get_func[])(Actor *, lua_State *) =
{
   nscript_actor_get_align,
   nscript_actor_get_alive,
   nscript_actor_get_combat_mode,
   nscript_actor_get_dexterity,
   nscript_actor_get_direction,
   nscript_actor_get_exp,
   nscript_actor_get_frame_n,
   nscript_actor_get_hp,
   nscript_actor_get_in_party_status,
   nscript_actor_get_intelligence,
   nscript_actor_get_level,
   nscript_actor_get_luatype,
   nscript_actor_get_magic,
   nscript_actor_get_movement_pts,
   nscript_actor_get_name,
   nscript_actor_get_obj_n,
   nscript_actor_get_sched_loc,
   nscript_actor_get_strength,
   nscript_actor_get_visible,
   nscript_actor_get_worktype,
   nscript_actor_get_x,
   nscript_actor_get_y,
   nscript_actor_get_z
};


static int nscript_map_get_actor(lua_State *L);


//Actor FIXME might not need these anymore.
static int nscript_actor_poison(lua_State *L);
static int nscript_actor_add_hp(lua_State *L);
static int nscript_actor_add_mp(lua_State *L);

static int nscript_actor_inv(lua_State *L);

void nscript_init_actor(lua_State *L)
{
   luaL_newmetatable(L, "nuvie.Actor");

   luaL_register(L, NULL, nscript_actorlib_m);

   luaL_register(L, "Actor", nscript_actorlib_f);

   lua_pushcfunction(L, nscript_map_get_actor);
   lua_setglobal(L, "map_get_actor");

   lua_pushcfunction(L, nscript_actor_poison);
   lua_setglobal(L, "actor_poison");
   lua_pushcfunction(L, nscript_actor_add_hp);
   lua_setglobal(L, "actor_add_hp");
   lua_pushcfunction(L, nscript_actor_add_mp);
   lua_setglobal(L, "actor_add_mp");

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

inline Actor *nscript_get_actor_from_args(lua_State *L, int lua_stack_offset)
{
   uint16 *actor_num;
   Actor *actor = NULL;

   actor_num = (uint16 *)luaL_checkudata(L, lua_stack_offset, "nuvie.Actor");

   if(actor_num != NULL)
      actor = Game::get_game()->get_actor_manager()->get_actor(*actor_num);

   return actor;
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

static int nscript_actor_set_combat_mode(Actor *actor, lua_State *L)
{
   actor->set_combat_mode((uint8)lua_tointeger(L, 3));
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

static int nscript_actor_set_strength(Actor *actor, lua_State *L)
{
   actor->set_strength((uint8)lua_tointeger(L, 3));
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

static int nscript_actor_get_align(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_alignment()); return 1;
}

static int nscript_actor_get_alive(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, actor->is_alive()); return 1;
}

static int nscript_actor_get_combat_mode(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_combat_mode()); return 1;
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

static int nscript_actor_get_strength(Actor *actor, lua_State *L)
{
   lua_pushinteger(L, actor->get_strength()); return 1;
}

static int nscript_actor_get_visible(Actor *actor, lua_State *L)
{
   lua_pushboolean(L, (int)actor->is_visible()); return 1;
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

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   actor->die();

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

   return 0;
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

static int nscript_actor_poison(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   uint16 actor_num;
   Actor *actor;

   actor_num = (uint16)luaL_checkinteger(L, 1);

   actor = actor_manager->get_actor(actor_num);

   actor->set_poisoned(true);

   return 0;
}

static int nscript_actor_add_hp(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   uint16 actor_num;
   uint8 hp;
   Actor *actor;

   actor_num = (uint16)luaL_checkinteger(L, 1);
   hp = (uint8)luaL_checkinteger(L, 2);

   actor = actor_manager->get_actor(actor_num);

   actor->set_hp(MAX((actor->get_hp() + hp), actor->get_maxhp()));

   return 0;
}

static int nscript_actor_add_mp(lua_State *L)
{
   ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   uint16 actor_num;
   uint8 mp;
   Actor *actor;

   actor_num = (uint16)luaL_checkinteger(L, 1);
   mp = (uint8)luaL_checkinteger(L, 2);

   actor = actor_manager->get_actor(actor_num);

   actor->set_magic(MAX((actor->get_magic() + mp), actor->get_maxmagic()));

   return 0;
}

//lua function actor_inventory(actor)
static int nscript_actor_inv(lua_State *L)
{
   Actor *actor;
   U6Link *link = NULL;

   actor = nscript_get_actor_from_args(L);
   if(actor == NULL)
      return 0;

   U6LList *inv = actor->get_inventory_list();
   if(inv != NULL)
      link = inv->start();

   lua_pushcfunction(L, nscript_u6llist_iter);

   U6Link **p_link = (U6Link **)lua_newuserdata(L, sizeof(U6Link *));
   *p_link = link;

   retainU6Link(link);

   luaL_getmetatable(L, "nuvie.U6Link");
   lua_setmetatable(L, -2);

   return 2;
}
