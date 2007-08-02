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

extern "C"
{
#include "lauxlib.h"
}

class Configuration;
class MapCoord;

#define NUVIE_SCRIPT_ERROR             0
#define NUVIE_SCRIPT_FINISHED          1
#define NUVIE_SCRIPT_GET_TARGET        2
#define NUVIE_SCRIPT_GET_DIRECTION     3
#define NUVIE_SCRIPT_GET_INV_OBJ       4
#define NUVIE_SCRIPT_ADVANCE_GAME_TIME 5
#define NUVIE_SCRIPT_ADVANCE_REAL_TIME 6

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
  uint8 resume(int narg = 0);
  bool finished() { return lua_status(L) != LUA_YIELD ? true : false; }
  int get_error() { return lua_status(L); }

};

class Script
{
  static Script *script;
 Configuration *config;
 nuvie_game_t gametype; // what game is being played?
 
 lua_State *L;

 public:

 Script(Configuration *cfg, nuvie_game_t type);
 ~Script();

 /* Return instance of self */
 static Script *get_script()           { return(script); }
 Configuration *get_config() { return(config); }
 
 bool run_script(const char *script);

 ScriptThread *new_thread(const char *scriptfile);
 ScriptThread *new_thread_from_string(const char *script);
 
 protected:
   void seed_random();
 
};

#endif /* __Script_h__ */
