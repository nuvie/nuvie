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

#include "U6def.h"
#include "Configuration.h"
#include "ObjManager.h"
#include "Map.h"
#include "MsgScroll.h"
#include "Player.h"

#define USE_EVENT_USE     0x01
#define USE_EVENT_LOOK    0x02
#define USE_EVENT_PASS    0x04
#define USE_EVENT_ON      0x08 // post-move/idle ??

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
 Actor *actor_ref;

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

 virtual bool can_use(Obj *obj)  { return(false); }
 virtual bool can_look(Obj *obj) { return(false); }
 virtual bool can_pass(Obj *obj) { return(false); }

 bool is_door(Obj *obj) {return(is_locked_door(obj) || is_unlocked_door(obj));}
 virtual bool is_locked_door(Obj *obj)   { return(false); }
 virtual bool is_unlocked_door(Obj *obj) { return(false); }
 virtual bool is_food(Obj *obj)          { return(false); }

 void set_itemref(sint32 val) { int_ref = val; }
 void set_itemref(Obj *val)   { obj_ref = val; }
 void set_itemref(Actor *val) { actor_ref = val; }

 protected:

 void toggle_frame(Obj *obj);
 bool use_container(Obj *obj);

 virtual sint16 get_ucobject_index(uint16 n, uint8 f = 0) = 0;
 virtual bool uc_event(sint16 uco, uint8 ev, Obj *obj) = 0;
};

#endif /* __UseCode_h__ */
