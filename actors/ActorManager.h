#ifndef __ActorManager_h__
#define __ActorManager_h__

/*
 *  ActorManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
 *  Copyright (c) 2003. All rights reserved.
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

#include "ObjManager.h"

class Configuration;
class Actor;
class Map;
class TileManager;
class GameClock;

class ActorManager
{
 Configuration *config;
 TileManager *tile_manager;
 ObjManager *obj_manager;

 bool update; // ActorManager is not paused
 
 Map *map;
 Actor *actors[256];
 uint8 player_actor;
 GameClock *clock;
 uint8 game_hour;
 
 uint16 last_obj_blk_x, last_obj_blk_y;
 uint8 last_obj_blk_z;
 
 public:
  
 ActorManager(Configuration *cfg, Map *m, TileManager *tm, ObjManager *om, GameClock *c);
 ~ActorManager();

 bool load(NuvieIO *objlist);
 
 Actor *get_actor(uint8 actor_num);
 Actor *get_actor(uint16 x, uint16 y, uint8 z);
 Actor *get_actor_holding_obj(Obj *obj);
 
 Actor *get_player();
 void set_player(Actor *a);

 const char *look_actor(Actor *a, bool show_prefix = true);

 void set_update(bool u) { update = u; }

 void updateActors(uint16 x, uint16 y, uint8 z);
 void twitchActors();
 void print_actor(Actor *actor);
 const char *get_worktype_string(uint32 wt);
 
 void clear_actor(Actor *actor);
 
 bool is_temp_actor(Actor *actor);
 bool create_temp_actor(uint16 obj_n, uint16 x, uint16 y, uint8 z, uint8 worktype, Actor **new_actor = NULL);
 bool toss_actor(Actor *actor, uint16 xrange, uint16 yrange);

 protected:
 
 bool loadActorSchedules();
 inline Actor *find_free_temp_actor();
 
 void update_temp_actors(uint16 x, uint16 y, uint8 z);
 void clean_temp_actors_from_level(uint8 level);
 void clean_temp_actors_from_area(uint16 x, uint16 y);

 inline void clean_temp_actor(Actor *actor);
};

#endif /* __ActorManager_h__ */
