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

#include "U6def.h"
#include "Configuration.h"
#include "U6LList.h"

#include "ActorManager.h"
#include "Actor.h"

class Game;

struct PartyMember {
char name[14];
Actor *actor;
uint8 combat_position;
sint8 form_x; // relative position left or right of leader
sint8 form_y; // relative position in front or in back of leader
              // (leader is at 0,0)
};

/* party walking formations: */
#define PARTY_FORM_STANDARD 0
#define PARTY_FORM_COLUMN   1
#define PARTY_FORM_ROW      2
#define PARTY_FORM_PHALANX  3
#define PARTY_FORM_DELTA    4
/*   0 <- standard *
 *  1 2            *
 * 4 3 5           *
 *  6 7            *
 *                 *
 * 0 <- column     * 3210 <- row
 * 1               * 7654
 * 2               *
 * 3...            *
 *                 *
 * 415             *    0 <- delta
 * 203 <- phalanx  *   1 2
 * 6 7             *  3   4
 *                 * 6     7
 */

class Party {
 Game *game; // get pointers here to avoid construct order issues in loadGame()
 Configuration *config;
 ActorManager *actor_manager;
 
 PartyMember member[16];
 uint8 num_in_party; // number of party members.
 uint8 formation; // walking formation
 bool autowalk; // party is automatically walking to a destination
 
 public:
 
 Party(Configuration *cfg);
 ~Party();
 
 bool init(Game *g, ActorManager *am);
 
 bool contains_actor(Actor *actor)
 {
  if(get_member_num(actor) >= 0)
   return(true);
  return(false);
 }
 bool contains_actor(uint8 a)
 {
  return(contains_actor(actor_manager->get_actor(a)));
 }
 bool add_actor(Actor *actor);
 bool remove_actor(Actor *actor);
 
 void split_gold();
 void gather_gold();
 
 uint8 get_party_size();
 uint8 get_party_max() { return(8); } // U6
 Actor *get_actor(uint8 member_num);
 char *get_actor_name(uint8 member_num);
 sint8 get_member_num(Actor *actor);
 sint8 get_member_num(uint8 a)
 {
  return(get_member_num(actor_manager->get_actor(a)));
 }
 //get actor id_n from party_member num.
 uint8 get_actor_num(uint8 member_num);
 
 void reform_party();
 void follow();
 bool has_obj(uint16 obj_n, uint8 quality);

 void show();
 void hide();
 
 bool move(uint16 dx, uint16 dy, uint8 dz);

 void walk(MapCoord *walkto, MapCoord *teleport);
 void stop_walking();
 bool get_autowalk() { return(autowalk); } 

 protected:

 bool loadParty();
};

#endif /* _PARTY_H_ */
