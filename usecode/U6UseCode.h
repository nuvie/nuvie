#ifndef __U6UseCode_h__
#define __U6UseCode_h__
/*
 *  U6UseCode.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri May 30 2003.
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
#include "UseCode.h"
//object numbers
#include "U6objects.h"

#define U6_LORD_BRITISH_ACTOR_NUM 5 /*change to U6_NPC_LORD_BRITISH?*/
#define U6_SHERRY_ACTOR_NUM       9
#define U6_BEHLEM_ACTOR_NUM       164

#define U6_LORD_BRITISH_ORB_CHECK_FLAG 0x20 // this is set if the player has asked LB about the orb of moons

class U6UseCode;
class Configuration;
class MsgScroll;

struct u6_uc_s // usecode definition (object)
{
 uint16 obj_n; // obj and frame numbers must match
 uint8 frame_n; // 255 matches any frame
 uint8 dist; // distance
 uint8 trigger; // event(s) to call ucf with
 bool (U6UseCode::*ucf)(Obj *, uint8); // usecode function
 void set(uint16 o, uint8 f, uint8 d, uint8 e, bool (U6UseCode::*u)(Obj *, uint8))
 { obj_n = o; frame_n = f; dist = d; trigger = e; ucf = u; }
};

class U6UseCode: public UseCode
{
 typedef struct u6_uc_s uc_obj;
 uc_obj *uc_objects;
 uint32 uc_objects_size; // size of uc_objects array
 uint16 uc_object_count; // count of game object<->usecode objects

 public:
 
 U6UseCode(Configuration *cfg);
 ~U6UseCode();
 void init_objects();
 void add_usecode(uint16 obj, uint8 frame, uint8 dist, uint8 ev, bool (U6UseCode::*func)(Obj *, uint8));

 bool use_obj(Obj *obj, Actor *actor);
 bool look_obj(Obj *obj, Actor *actor);
 bool pass_obj(Obj *obj, Actor *actor);
 bool search_obj(Obj *obj, Actor *actor);

 bool has_usecode(Obj *obj);
 bool has_lookcode(Obj *obj);
 bool has_passcode(Obj *obj);

 bool is_unlocked_door(Obj *obj) { return(obj->obj_n >= 297 && obj->obj_n <= 300 && obj->frame_n != 9 && obj->frame_n != 11); }
 bool is_locked_door(Obj *obj)   { return(obj->obj_n >= 297 && obj->obj_n <= 300 && (obj->frame_n == 9 || obj->frame_n == 11)); }
 bool is_closed_door(Obj *obj)   { return(obj->obj_n >= 297 && obj->obj_n <= 300 && obj->frame_n > 3); }
 bool is_food(Obj *obj);

 protected:
 sint16 get_ucobject_index(uint16 n, uint8 f, uint8 ev);
 bool uc_event(sint16 uco, uint8 ev, Obj *obj);

// usecode
 bool use_orb(Obj *obj, uint8 ev);
 bool use_door(Obj *obj, uint8 ev);
 bool use_secret_door(Obj *obj, uint8 ev);
 bool use_ladder(Obj *obj, uint8 ev);
 bool use_passthrough(Obj *obj, uint8 ev);
 bool use_switch(Obj *obj, uint8 ev);
 bool use_crank(Obj *obj, uint8 ev);
 bool use_food(Obj *obj, uint8 ev);
 bool use_firedevice(Obj *obj, uint8 ev);
 bool use_container(Obj *obj, uint8 ev);
 bool use_vortex_cube(Obj *obj, uint8 ev);
 bool use_key(Obj *obj, uint8 ev);
 bool use_boat(Obj *obj, uint8 ev);
 inline Obj *use_boat_find_center(Obj *obj);
 inline bool use_boat_find_land(uint16 *x, uint16 *y, uint8 *z);
 bool use_cow(Obj *obj, uint8 ev);
 bool use_well(Obj *obj, uint8 ev);
 bool fill_bucket(uint16 filled_bucket_obj_n);
 bool use_churn(Obj *obj, uint8 ev);
 bool use_beehive(Obj *obj, uint8 ev);
 bool use_horse(Obj *obj, uint8 ev);
 bool use_potion(Obj *obj, uint8 ev);
 bool use_bell(Obj *obj, uint8 ev);
 bool use_shovel(Obj *obj, uint8 ev);
 bool use_fountain(Obj *obj, uint8 ev);
 bool use_rubber_ducky(Obj *obj, uint8 ev);
 bool play_instrument(Obj *obj, uint8 ev);
 bool look_mirror(Obj *obj, uint8 ev);
 bool look_sign(Obj *obj, uint8 ev);
 bool look_clock(Obj *obj, uint8 ev);
 bool pass_quest_barrier(Obj *obj, uint8 ev);
 bool enter_dungeon(Obj *obj, uint8 ev);
 bool enter_red_moongate(Obj *obj, uint8 ev);
// bool search_container(Obj *obj, uint8 ev);

// supplementary
 Obj *drawbridge_find(Obj *crank_obj);
 void drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width);
 bool use_firedevice_message(Obj *obj, bool lit);
 void lock_door(Obj *obj);
 void unlock_door(Obj *obj);

#if 0 /* names for other events? */
 bool approach_mirror(Obj *obj, uint8 ev); // change to reflect frame
 bool attack_mirror(Obj *obj, uint8 ev); // break mirror
 bool drop_mirror(Obj *obj, uint8 ev); // call after drop (break mirror)
#endif
};

#endif /* __U6UseCode_h__ */
