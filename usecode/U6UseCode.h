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
#include "TimedEvent.h"
#include "UseCode.h"
//object numbers
#include "U6objects.h"

#define U6_LORD_BRITISH_ACTOR_NUM 5 /*change to U6_NPC_LORD_BRITISH?*/
#define U6_SHERRY_ACTOR_NUM       9
#define U6_BEHLEM_ACTOR_NUM       164

#define U6_LORD_BRITISH_ORB_CHECK_FLAG 0x20 // this is set if the player has asked LB about the orb of moons

class U6UseCode;
class CallBack;
class Configuration;
class MsgScroll;

// U6ObjectType properties
#define OBJTYPE_NONE      0x0000
#define OBJTYPE_FOOD      0x0001 /* food or drink */
#define OBJTYPE_CONTAINER 0x0002 /* must be set on empty bag,crate,backpack */

typedef struct // object properties & usecode
{
    uint16 obj_n; // type
    uint8 frame_n; // 0xFF matches any frame
    uint8 dist; // distance to trigger (depends on event, usually 0)
    UseCodeEvent trigger; // accepted event(s)
    bool (U6UseCode::*usefunc)(Obj *, UseCodeEvent); // usecode function
    uint16 flags; // properties (OBJTYPE)
} U6ObjectType;

class U6UseCode: public UseCode, public CallBack
{
 public:
 
 U6UseCode(Game *g, Configuration *cfg);
 ~U6UseCode();

 bool use_obj(Obj *obj, Actor *actor);
 bool look_obj(Obj *obj, Actor *actor);
 bool pass_obj(Obj *obj, Actor *actor);
 bool search_obj(Obj *obj, Actor *actor);
 bool move_obj(Obj *obj, sint16 rel_x, sint16 rel_y);
 bool load_obj(Obj *obj);
 bool message_obj(Obj *obj, CallbackMessage msg, void *msg_data);
 bool ready_obj(Obj *obj, Actor *actor);
 bool get_obj(Obj *obj, Actor *actor);
 bool drop_obj(Obj *obj, Actor *actor, uint16 x, uint16 y);

 bool has_usecode(Obj *obj, uint16 ev = USE_EVENT_USE);

 bool is_unlocked_door(Obj *obj) { return(obj->obj_n >= 297 && obj->obj_n <= 300 && obj->frame_n != 9 && obj->frame_n != 11); }
 bool is_locked_door(Obj *obj)   { return(obj->obj_n >= 297 && obj->obj_n <= 300 && (obj->frame_n == 9 || obj->frame_n == 11)); }
 bool is_closed_door(Obj *obj)   { return(obj->obj_n >= 297 && obj->obj_n <= 300 && obj->frame_n > 3); }
 bool is_food(Obj *obj);
 bool is_container(Obj *obj);

 uint16 callback(uint16 msg, CallBack *caller, void *data = NULL);

 protected:
 bool uc_event(const U6ObjectType *type, UseCodeEvent ev, Obj *obj);
 inline const U6ObjectType *get_object_type(uint16 n, uint8 f, UseCodeEvent ev = 0);

 public:
// usecode
 bool use_orb(Obj *obj, UseCodeEvent ev);
 bool use_door(Obj *obj, UseCodeEvent ev);
 bool use_secret_door(Obj *obj, UseCodeEvent ev);
 bool use_ladder(Obj *obj, UseCodeEvent ev);
 bool use_passthrough(Obj *obj, UseCodeEvent ev);
 bool use_switch(Obj *obj, UseCodeEvent ev);
 bool use_crank(Obj *obj, UseCodeEvent ev);
 bool use_food(Obj *obj, UseCodeEvent ev);
 bool use_firedevice(Obj *obj, UseCodeEvent ev);
 bool use_fan(Obj *obj, UseCodeEvent ev);
 bool use_sextant(Obj *obj, UseCodeEvent ev);
 bool use_container(Obj *obj, UseCodeEvent ev);
 bool use_rune(Obj *obj, UseCodeEvent ev);
 bool use_vortex_cube(Obj *obj, UseCodeEvent ev);
 bool use_key(Obj *obj, UseCodeEvent ev);
 bool use_boat(Obj *obj, UseCodeEvent ev);
 inline Obj *use_boat_find_center(Obj *obj);
 inline bool use_boat_find_land(uint16 *x, uint16 *y, uint8 *z);
 bool use_cow(Obj *obj, UseCodeEvent ev);
 bool use_well(Obj *obj, UseCodeEvent ev);
 bool fill_bucket(uint16 filled_bucket_obj_n);
 bool use_churn(Obj *obj, UseCodeEvent ev);
 bool use_beehive(Obj *obj, UseCodeEvent ev);
 bool use_horse(Obj *obj, UseCodeEvent ev);
 bool use_potion(Obj *obj, UseCodeEvent ev);
 bool use_bell(Obj *obj, UseCodeEvent ev);
 bool use_shovel(Obj *obj, UseCodeEvent ev);
 bool use_fountain(Obj *obj, UseCodeEvent ev);
 bool use_rubber_ducky(Obj *obj, UseCodeEvent ev);
 bool play_instrument(Obj *obj, UseCodeEvent ev);
 bool look_mirror(Obj *obj, UseCodeEvent ev);
 bool look_sign(Obj *obj, UseCodeEvent ev);
 bool look_clock(Obj *obj, UseCodeEvent ev);
 bool pass_quest_barrier(Obj *obj, UseCodeEvent ev);
 bool enter_dungeon(Obj *obj, UseCodeEvent ev);
 bool enter_red_moongate(Obj *obj, UseCodeEvent ev);
// bool search_container(Obj *obj, UseCodeEvent ev);
 bool use_powder_keg(Obj *obj, UseCodeEvent ev);
 bool use_cannon(Obj *obj, UseCodeEvent ev);
 bool use_egg(Obj *obj, UseCodeEvent ev);
 bool sundial(Obj *obj, UseCodeEvent ev);
 bool torch(Obj *obj, UseCodeEvent ev);

 protected:
// supplementary
 Obj *drawbridge_find(Obj *crank_obj);
 void drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width);
 bool use_firedevice_message(Obj *obj, bool lit);
 void lock_door(Obj *obj);
 void unlock_door(Obj *obj);
 Obj *bell_find(Obj *chain_obj);
 void sundial_set_shadow(Obj *sundial, uint8 hour);
 void extinguish_torch(Obj *obj);

};

#endif /* __U6UseCode_h__ */
