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

#define USECODE_EVENT_USE     0x01
#define USECODE_EVENT_LOOK    0x02
#define USECODE_EVENT_STEPTO  0x04


class UseCode
{
 protected:
 
 Configuration *config;
 ObjManager *obj_manager;
 Map *map;
 Player *player;
 MsgScroll *scroll;

 sint32 itemref; // pass integers to usecode functions

 public:
 
 UseCode(Configuration *cfg);
 virtual ~UseCode();
 
 virtual bool init(ObjManager *om, Map *m, Player *p, MsgScroll *ms);
 
 bool use_obj(uint16 x, uint16 y, uint8 z, Obj *src_obj=NULL);
 virtual bool use_obj(Obj *obj, Obj *src_obj=NULL)=0;

 virtual void init_objects() = 0;
 virtual sint16 get_ucobject_index(uint16 n, uint8 f = 0) = 0;
 virtual bool uc_event(sint16 uco, uint8 ev, Obj *obj) = 0;

 void set_itemref(sint32 val) { itemref = val; }

 protected:
 
 void toggle_frame(Obj *obj);
 bool use_container(Obj *obj);
};

#endif /* __UseCode_h__ */
