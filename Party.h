#ifndef _PARTY_H_
#define _PARTY_H_
/*
 *  Party.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 06 2003.
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

#include <string.h>
#include "ObjManager.h"

class Configuration;
class Game;
class Actor;
class ActorManager;
class MapCoord;
class Map;
class NuvieIO;

struct PartyMember {
char name[14];
Actor *actor;
uint8 combat_position;
sint8 form_x; // relative position left or right of leader
sint8 form_y; // relative position in front or in back of leader
              // (leader is at 0,0 in formation)
uint16 leader_x, leader_y; // last seen location of leader
};

/* party walking formations: */
#define PARTY_FORM_STANDARD 0
#define PARTY_FORM_COLUMN   1
#define PARTY_FORM_ROW      2
#define PARTY_FORM_PHALANX  3
#define PARTY_FORM_DELTA    4
#define PARTY_FORM_DIAMOND  5
#define PARTY_FORM_CROWD    6
#define PARTY_FORM_COMBAT   7

/*   0 <- standard  *
 *  1 2             *
 * 4 3 5            *
 *  6 7             *
 *                  *
 * 0 <- column      * 3210 <- row
 * 1                * 7654
 * 2                *
 * 3...             *
 *                  *
 *  102             *    0 <- delta
 * 9354A <- phalanx *   172
 * B687C            *  38 94
 * DFHGE            * 5A   B6
 * I   J            *
 *
 *   0 <- diamond
 *  182
 * 39 A4
 *  5B6
 *   7
 *
 * (Combat positions are dynamic, based on combat mode)
 */

class Party {
 Game *game; // get pointers here to avoid construct order issues in loadGame()
 Configuration *config;
 ActorManager *actor_manager;
 Map *map;
 
 PartyMember member[16];
 uint8 num_in_party; // number of party members.
 uint8 formation; // walking formation
 bool autowalk; // party is automatically walking to a destination
 
 bool in_vehicle; //Party is in a vehicle.
 bool in_combat_mode;
 
 public:
 
 Party(Configuration *cfg);
 ~Party();
 
 bool init(Game *g, ActorManager *am);
 bool load(NuvieIO *objlist);
 
 bool add_actor(Actor *actor);
 bool remove_actor(Actor *actor);
 
 void split_gold();
 void gather_gold();
 
 uint8 get_party_size();
 uint8 get_party_max() { return(8); } // U6
 Actor *get_actor(uint8 member_num);
 char *get_actor_name(uint8 member_num);
 sint8 get_member_num(Actor *actor);
 sint8 get_member_num(uint8 a);
 //get actor id_n from party_member num.
 uint8 get_actor_num(uint8 member_num);
 MapCoord get_formation_coords(uint8 m);
 inline MapCoord get_location(uint8 m = 0);

 void reform_party();
 void follow();
 void find_leader(uint8 m);

 bool has_obj(uint16 obj_n, uint8 quality);
 uint16 who_has_obj(uint16 obj_n, uint8 quality);

 bool is_leader(Actor *actor) { return(get_member_num(actor) == 0); }
 bool contains_actor(Actor *actor);
 bool contains_actor(uint8 actor_num);
 bool is_at(uint16 x, uint16 y, uint8 z, uint32 threshold = 0);
 bool is_at(MapCoord &xyz, uint32 threshold = 0);
 
 void set_in_vehicle(bool value);
 bool is_in_vehicle() { return in_vehicle; }
 
 void set_in_combat_mode(bool value);
 bool is_in_combat_mode() { return in_combat_mode; }
 
 void update_music();
 
 void show();
 void hide();
 
 bool move(uint16 dx, uint16 dy, uint8 dz);

 void walk(MapCoord *walkto, MapCoord *teleport, uint32 step_delay = 0);
 void walk(MapCoord *walkto, uint32 step_delay = 0) { walk(walkto, NULL, step_delay); }
 void walk(Obj *moongate, MapCoord *teleport, uint32 step_delay = 0);
 void enter_vehicle(Obj *ship_obj, uint32 step_delay = 0);
 void stop_walking();
 bool get_autowalk() { return(autowalk); } 

 void dismount_from_horses();

 protected:

};

#endif /* _PARTY_H_ */
