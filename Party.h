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

#include <string>
#include "ObjManager.h"

class Configuration;
class Game;
class Actor;
class ActorManager;
class MapCoord;
class Map;
class NuvieIO;
class PartyPathFinder;
class PartySeek;

struct PartyMember {
char name[14];
Actor *actor;
//bool inactive; // true if not in formation
uint8 combat_position;
sint8 form_x; // relative position left or right of leader
sint8 form_y; // relative position in front or in back of leader
              // (leader is at 0,0 in formation)
};

#define PARTY_MAX_MEMBERS 16
#define PARTY_NAME_MAX_LENGTH 13

#define PARTY_KEEP_PARTY_FLAG true

/* party walking formations: */
#define PARTY_FORM_STANDARD 0
#define PARTY_FORM_COLUMN   1
#define PARTY_FORM_ROW      2
#define PARTY_FORM_DELTA    4
#define PARTY_FORM_COMBAT   7
#define PARTY_FORM_REST     8

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
 *    0 <- delta
 *   172
 *  38 94
 * 5A   B6
 *
 * (Combat positions are dynamic, based on combat mode)
 */

class Party {
 protected:
 friend class PartyPathFinder;
 Game *game; // get pointers here to avoid construct order issues in loadGame()
 Configuration *config;
 ActorManager *actor_manager;
 Map *map;
 PartyPathFinder *pathfinder;

 PartyMember member[PARTY_MAX_MEMBERS];
 uint8 num_in_party; // number of party members.
 uint8 formation; // walking formation
 uint16 prev_leader_x; // last location of leader
 uint16 prev_leader_y;

 bool autowalk; // party is automatically walking to a destination
 bool in_vehicle; //Party is in a vehicle.
 bool in_combat_mode;

 bool defer_removing_dead_members;

 Obj *rest_campfire;

 public:

 Party(Configuration *cfg);
 virtual ~Party();

 virtual bool init(Game *g, ActorManager *am);
 virtual bool load(NuvieIO *objlist);
 virtual bool save(NuvieIO *objlist);

 // Basic methods
 void follow(sint8 rel_x, sint8 rel_y); // follow in direction leader moved
 bool move(uint16 dx, uint16 dy, uint8 dz);
 void show(); // Actor::show()
 void hide(); // Actor::hide()
 virtual void dismount_from_horses();
 virtual void update_music(); // set music depending on party location
 virtual void split_gold();
 virtual void gather_gold();
 bool add_actor(Actor *actor);
 bool remove_actor(Actor *actor, bool keep_party_flag=false);
 bool remove_dead_actor(Actor *actor);
 bool resurrect_dead_members();
 void heal();
 //void set_active(uint8 member_num, bool state) { member[member_num].inactive = !state; }

 // Properties
 uint8 get_party_size();
 virtual uint8 get_party_max() { return(8); } // U6
 sint8 get_leader(); // returns -1 if party has no leader and can't move
 MapCoord get_leader_location();
 MapCoord get_location(uint8 m = 0);
 MapCoord get_formation_coords(uint8 m);
 void set_in_vehicle(bool value);
 void set_in_combat_mode(bool value);
 bool is_in_vehicle() { return in_vehicle; }
 bool is_in_combat_mode() { return in_combat_mode; }
 Actor *get_slowest_actor(); // actor with lowest move count

 // Check specific actors
 uint8 get_actor_num(uint8 member_num); //get actor id_n from party_member num.
 Actor *get_actor(uint8 member_num);
 sint8 get_member_num(Actor *actor);
 sint8 get_member_num(uint8 a);
 char *get_actor_name(uint8 member_num);
 bool is_leader(Actor *actor) { return(get_member_num(actor) == get_leader()); }
 bool contains_actor(Actor *actor);
 bool contains_actor(uint8 actor_num);

 // Check entire party
 bool is_at(uint16 x, uint16 y, uint8 z, uint32 threshold = 0);
 bool is_at(MapCoord &xyz, uint32 threshold = 0);
 bool is_anyone_at(uint16 x, uint16 y, uint8 z, uint32 threshold = 0);
 bool is_anyone_at(MapCoord &xyz, uint32 threshold = 0);
 bool has_obj(uint16 obj_n, uint8 quality, bool match_zero_qual=true);
 bool remove_obj(uint16 obj_n, uint8 quality);
 Actor *who_has_obj(uint16 obj_n, uint8 quality, bool match_zero_qual=true);
 bool is_horsed(); // is anyone on a horse?
 Obj *get_food(); // used while resting

 // Automatic-walking. These methods should be replaced with ActorActions.
 void walk(MapCoord *walkto, MapCoord *teleport, uint32 step_delay = 0);
 void walk(MapCoord *walkto, uint32 step_delay = 0) { walk(walkto, NULL, step_delay); }
 void walk(Obj *moongate, MapCoord *teleport, uint32 step_delay = 0);
 void enter_vehicle(Obj *ship_obj, uint32 step_delay = 0);
 void exit_vehicle(uint16 x, uint16 y, uint16 z);
 void stop_walking();
 bool get_autowalk() { return(autowalk); }
 void rest_gather();
 void rest_sleep(uint8 hours, sint16 guard);
 bool can_rest(std::string &err_str);

 protected:
 void reform_party(); // call when adding or removing members

};

#endif /* _PARTY_H_ */
