#ifndef __Player_h__
#define __Player_h__

/*
 *  player.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Mar 23 2003.
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

#include "ObjManager.h"

class Configuration;
class GameClock;
class Actor;
class ActorManager;
class MapWindow;
class Party;
class NuvieIO;

class Player
{
 Configuration *config;
 int game_type;
 GameClock *clock;
 Party *party;
 bool party_mode;
 bool mapwindow_centered;
 Actor *actor;
 ActorManager *actor_manager;
 ObjManager *obj_manager;
 
 char name[14];
 uint8 gender;

 uint8 questf; 
 uint8 karma;
 
 MapWindow *map_window;
 
 public:
 
 Player(Configuration *cfg);
 
 bool init(ObjManager *om, ActorManager *am, MapWindow *mw, GameClock *c, Party *p);
 void init();
 bool load(NuvieIO *objlist);
 bool save(NuvieIO *objlist);
 
 Actor *find_actor();

 bool is_mapwindow_centered()            { return(mapwindow_centered); }
 void set_mapwindow_centered(bool state);

 Party *get_party()      { return(party); }
 bool set_party_mode(Actor *new_actor);
 bool set_solo_mode(Actor *new_actor);
 bool in_party_mode()    { return(party_mode); }

 void set_karma(uint8 val) { karma = val; }
 uint8 get_karma()         { return(karma); }
 void add_karma(uint8 val = 1);
 void subtract_karma(uint8 val = 1);

 void set_quest_flag(uint8 val) { questf = val; }
 uint8 get_quest_flag()         { return(questf); }
 
 void set_actor(Actor *new_actor);
 Actor *get_actor();
 void get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level);
 uint8 get_location_level();
 
 char *get_name();
 void set_gender(uint8 val) { gender = val; }
 char *get_gender_title();
 uint8 get_gender()         { return(gender); }

 void moveRelative(sint16 rel_x, sint16 rel_y);
 void move(sint16 new_x, sint16 new_y, uint8 new_level);
 void moveLeft();
 void moveRight();
 void moveUp();
 void moveDown();
 void pass();

 uint32 get_walk_delay();

 protected:
 
};

#endif /* __Player_h__ */
