#ifndef __Script_h__
#define __Script_h__

/*
 *  Script.h
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

#include <string>
#include <list>
#include "GUI.h"

extern "C"
{
#include "lauxlib.h"
}

class Configuration;
class MapCoord;
class Spell;
class Obj;
class Actor;
class SoundManager;

#define NUVIE_SCRIPT_ERROR             0
#define NUVIE_SCRIPT_FINISHED          1
#define NUVIE_SCRIPT_GET_TARGET        2
#define NUVIE_SCRIPT_GET_DIRECTION     3
#define NUVIE_SCRIPT_GET_INV_OBJ       4
#define NUVIE_SCRIPT_ADVANCE_GAME_TIME 5
#define NUVIE_SCRIPT_ADVANCE_REAL_TIME 6
#define NUVIE_SCRIPT_TALK_TO_ACTOR     7
#define NUVIE_SCRIPT_GET_SPELL         8

#define NUVIE_SCRIPT_CB_ADV_GAME_TIME  1

class ScriptThread
{
  lua_State *L;
  int start_nargs;
  uint32 data;

public:
  
    ScriptThread(lua_State *l, int nargs) { L = l; start_nargs = nargs; data = 0; }
  ~ScriptThread() {  }
  uint32 get_data() { return data; }
  uint8 start() { return resume(start_nargs); }
  uint8 resume_with_location(MapCoord loc);
  uint8 resume_with_direction(uint8 dir);
  uint8 resume_with_spell_num(uint8 spell_num);
  uint8 resume_with_obj(Obj *obj);
  uint8 resume_with_nil();
  uint8 resume(int narg = 0);
  bool finished() { return lua_status(L) != LUA_YIELD ? true : false; }
  int get_error() { return lua_status(L); }

};

#define SCRIPT_DISPLAY_HIT_MSG true
class Script
{
  static Script *script;
 Configuration *config;
 nuvie_game_t gametype; // what game is being played?
 
 lua_State *L;

 public:

 Script(Configuration *cfg, GUI *gui, SoundManager *sm, nuvie_game_t type);
 ~Script();

 bool init();

 /* Return instance of self */
 static Script *get_script()           { return(script); }
 Configuration *get_config() { return(config); }
 
 bool run_script(const char *script);
   bool call_load_game(NuvieIO *objlist);
   bool call_save_game(NuvieIO *objlist);

   bool call_actor_update_all();
   bool call_actor_init(Actor *actor, uint8 alignment);
   bool call_actor_attack(Actor *actor, MapCoord location, Obj *weapon);
   bool call_actor_map_dmg(Actor *actor, MapCoord location);
   bool call_actor_hit(Actor *actor, uint8 dmg, bool display_hit_msg=false);
   bool call_look_obj(Obj *obj);
   int call_obj_get_readiable_location(Obj *obj);
   bool call_actor_get_obj(Actor *actor, Obj *obj);
   bool call_actor_subtract_movement_points(Actor *actor, uint8 points);
   bool call_use_keg(Obj *obj); //we need this until we move all usecode into script.
   bool call_magic_get_spell_list(Spell **spell_list);
   bool call_actor_use_effect(Obj *effect_obj, Actor *actor);
   bool call_function(const char *func_name, int num_args, int num_return, bool print_stacktrace=true);
   bool run_lua_file(const char *filename);
   bool call_moonstone_set_loc(uint8 phase, MapCoord location); //this is a hack until we have 'use' moonstone in script.
   bool call_advance_time(uint16 minutes);

   MapCoord call_moonstone_get_loc(uint8 phase);
   bool call_update_moongates(bool visible);
   
 ScriptThread *new_thread(const char *scriptfile);
 ScriptThread *new_thread_from_string(const char *script);
 
 protected:
   bool call_loadsave_game(const char *function, NuvieIO *objlist);
   void seed_random();
 
};

#endif /* __Script_h__ */
