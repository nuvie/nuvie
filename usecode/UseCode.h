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
#define USE_EVENT_ON      0x08 /* post-move/idle */
#define USE_EVENT_SEARCH  0x10 /*undefined (true = had objects?); might remove*/
#define USE_EVENT_TIMED   0x20 /* return value undefined */
#define USE_EVENT_MOVE    0x40 /* true: move object, false: don't move object */
#define USE_EVENT_LOAD    0x80 /* return value undefined */
//#define USE_EVENT_NEAR    0x00 /* mirrors; might use ON with distance val */
//#define USE_EVENT_ATTACK  0x00 /* doors, chests, mirrors */
//#define USE_EVENT_DROP    0x00 /* anything breakable */
//#define USE_EVENT_ENTER   0x00 /* object enters view (clocks) */
//#define USE_EVENT_LEAVE   0x00 /* object leaves view */
//will need to increase trigger size...

/* Events:
 * USE
 * Returns: undefined
 * Use the object. actor_ref is the actor using it (the player actor)
 *
 * PASS (Quest Barrier)
 * Returns: True if actor may move, False if object blocks
 * Called when an actor attempts to step onto an object.
 *
 * LOOK (signs)
 * Returns: True if an object can be searched
 * Called when someone looks at the object. Some objects aren't searched (books
 * for example) and should return false.
 *
 * TIMED (fumaroles, earthquakes?, powder kegs, clocks)
 * Returns: nothing
 * A timed event has been activated for the object. It must have already been
 * started by something else.
 *
 * MOVE (cannons)
 * Returns: True to push object to the new position
 * Use this to perform special move functions for some objects. A cannon can be
 * aimed with MOVE.
 *
 * (UN)LOAD
 * Returns: undefined
 * Called when the object is cached in or out (and when new objects are 
 * created.) It can be used to start timers, or to hatch eggs.
 *
 * Actor NEAR
 *
 * Actor ON (chairs, traps)
 * Returns: undefined
 * Called each turn for any objects in the view-area with actors standing on
 * them.
 *
 * ENTER view-area
 *
 * LEAVE view-area
 *
 * (UN)READY (Amulet of Submission)
 * Returns: True if the object may be worn, or removed
 * This is called before the object is is equipped or removed. Check the
 * object's flags to determine if it is being equipped or not. (if its readied
 * flag is set it is being removed).
 *
 * ATTACK (doors, chests)
 *
 * DROP (breakables)
 *
 */


class ActorManager;
class Configuration;
class Event;
class Game;
class Map;
class MsgScroll;
class MapCoord;
class Party;
class Player;

class UseCode
{
 protected:
 Game *game;
 Configuration *config;
 ObjManager *obj_manager;
 Map *map;
 Player *player;
 MsgScroll *scroll;
 ActorManager *actor_manager;
 Party *party;

 sint32 int_ref; // pass objects to usecode functions
 Obj *obj_ref;
 Actor *actor_ref, *actor2_ref;
 MapCoord *mapcoord_ref;

 public:
 
 UseCode(Game *g, Configuration *cfg);
 virtual ~UseCode();
 
 virtual bool init(ObjManager *om, Map *m, Player *p, MsgScroll *ms);
 
 bool use_obj(uint16 x, uint16 y, uint8 z, Obj *src_obj=NULL);
 bool use_obj(Obj *obj, Obj *src_obj = NULL) { return(use_obj(obj, player->get_actor())); } // ??

 virtual bool use_obj(Obj *obj, Actor *actor)  { return(false); }
 virtual bool look_obj(Obj *obj, Actor *actor) { return(false); }
 virtual bool pass_obj(Obj *obj, Actor *actor) { return(false); }
 virtual bool search_obj(Obj *obj, Actor *actor) { return(false); }
 virtual bool move_obj(Obj *obj, sint16 rel_x, sint16 rel_y) { return(false); }
 virtual bool load_obj(Obj *obj) { return(false); }

 virtual bool has_usecode(Obj *obj, uint16 ev = USE_EVENT_USE)  { return(false); }
 virtual bool has_lookcode(Obj *obj) { return(has_usecode(obj, USE_EVENT_LOOK)); }
 virtual bool has_passcode(Obj *obj) { return(has_usecode(obj, USE_EVENT_PASS)); }
 virtual bool has_movecode(Obj *obj) { return(has_usecode(obj, USE_EVENT_MOVE)); }
 virtual bool has_loadcode(Obj *obj) { return(has_usecode(obj, USE_EVENT_LOAD)); }

 bool is_door(Obj *obj) {return(is_locked_door(obj) || is_unlocked_door(obj));}
 virtual bool is_locked_door(Obj *obj)   { return(false); }
 virtual bool is_unlocked_door(Obj *obj) { return(false); }
 virtual bool is_food(Obj *obj)          { return(false); }

 void set_itemref(sint32 val) { int_ref = val; }
 void set_itemref(Obj *val)   { obj_ref = val; }
 void set_itemref(Actor *val, Actor *val2 = NULL) { actor_ref = val; actor2_ref = val2; }
 void set_itemref(MapCoord *val)   { mapcoord_ref = val; }

 Obj *get_obj_from_container(Obj *obj);
 bool search_container(Obj *obj);

 protected:

 void toggle_frame(Obj *obj);

};

#endif /* __UseCode_h__ */
