#ifndef __UseCode_h__
#define __UseCode_h__
/*
 *  UseCode.h
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
#include "Player.h"

// The game triggers one of these events on an object to activate its UseCode
// function(s). The return value meaning is different for each event.
#define USE_EVENT_USE     0x01 /* return value undefined */
#define USE_EVENT_LOOK    0x02 /* true: allow search, false: disallow search */
#define USE_EVENT_PASS    0x04 /* true: do normal move, false: object blocks */
//#define USE_EVENT_ON      0x08 /* post-move/idle, chairs/traps ?? */
#define USE_EVENT_SEARCH  0x10 /*undefined (true = had objects?); might remove*/
//#define USE_EVENT_TIMED   0x00 /* fumaroles, earthquakes, powder kegs, clocks */
//#define USE_EVENT_NEAR    0x00 /* mirrors; might use ON with distance val */
//#define USE_EVENT_LOAD    0x00 /* eggs */
//will need to increase trigger size for...
//#define USE_EVENT_ATTACK  0x00 /* doors, chests, mirrors */
//#define USE_EVENT_MOVE    0x00 /* cannons */
//#define USE_EVENT_DROP    0x00 /* anything breakable */

class Configuration;
class Map;
class MsgScroll;
class MapCoord;
class UseCode
{
 protected:
 
 Configuration *config;
 ObjManager *obj_manager;
 Map *map;
 Player *player;
 MsgScroll *scroll;

 sint32 int_ref; // pass objects to usecode functions
 Obj *obj_ref;
 Actor *actor_ref, *actor2_ref;
 MapCoord *mapcoord_ref;

 public:
 
 UseCode(Configuration *cfg);
 virtual ~UseCode();
 
 virtual bool init(ObjManager *om, Map *m, Player *p, MsgScroll *ms);
 virtual void init_objects() = 0;
 
 bool use_obj(uint16 x, uint16 y, uint8 z, Obj *src_obj=NULL);
 bool use_obj(Obj *obj, Obj *src_obj = NULL) { return(use_obj(obj, player->get_actor())); } // ??

 virtual bool use_obj(Obj *obj, Actor *actor)  { return(false); }
 virtual bool look_obj(Obj *obj, Actor *actor) { return(false); }
 virtual bool pass_obj(Obj *obj, Actor *actor) { return(false); }
 virtual bool search_obj(Obj *obj, Actor *actor) { return(false); }

 virtual bool can_use(Obj *obj)  { return(false); }
 virtual bool can_look(Obj *obj) { return(false); }
 virtual bool can_pass(Obj *obj) { return(false); }

 bool is_door(Obj *obj) {return(is_locked_door(obj) || is_unlocked_door(obj));}
 virtual bool is_locked_door(Obj *obj)   { return(false); }
 virtual bool is_unlocked_door(Obj *obj) { return(false); }
 virtual bool is_food(Obj *obj)          { return(false); }

 void set_itemref(sint32 val) { int_ref = val; }
 void set_itemref(Obj *val)   { obj_ref = val; }
 void set_itemref(Actor *val, Actor *val2 = NULL) { actor_ref = val; actor2_ref = val2; }
 void set_itemref(MapCoord *val)   { mapcoord_ref = val; }
 
 protected:

 void toggle_frame(Obj *obj);
 bool use_container(Obj *obj, bool print = true);

 virtual sint16 get_ucobject_index(uint16 n, uint8 f = 0) = 0;
 virtual bool uc_event(sint16 uco, uint8 ev, Obj *obj) = 0;
};

#endif /* __UseCode_h__ */
