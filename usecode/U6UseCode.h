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

#include "U6def.h"
#include "Configuration.h"
#include "ObjManager.h"
#include "MsgScroll.h"
#include "Player.h"
#include "UseCode.h"

//object numbers
#define OBJ_U6_VORTEX_CUBE     62
#define OBJ_U6_KEY             64
#define OBJ_U6_CHEST           98
#define OBJ_U6_CANDLE         122
#define OBJ_U6_PICTURE        143
#define OBJ_U6_CANDELABRA     145
#define OBJ_U6_BOOK           151
#define OBJ_U6_SCROLL         152
#define OBJ_U6_FIREPLACE      164
#define OBJ_U6_SWITCH         174
#define OBJ_U6_ELECTRIC_FIELD 175
#define OBJ_U6_DRAWER         176
#define OBJ_U6_BARREL         186
#define OBJ_U6_BAG            188
#define OBJ_U6_CRATE          192
#define OBJ_U6_BRAZIER        206
#define OBJ_U6_MOUSEHOLE      213
#define OBJ_U6_CHAIR          252
#define OBJ_U6_CROSS          254
#define OBJ_U6_TOMBSTONE      255
#define OBJ_U6_LEVER          268
#define OBJ_U6_DRAWBRIDGE     269
#define OBJ_U6_V_PASSTHROUGH  278
#define OBJ_U6_H_PASSTHROUGH  280
#define OBJ_U6_CRANK          288

#define OBJ_U6_OAKEN_DOOR     297
#define OBJ_U6_WINDOWED_DOOR  298
#define OBJ_U6_CEDAR_DOOR     299
#define OBJ_U6_STEEL_DOOR     300

#define OBJ_U6_DOORWAY        301
#define OBJ_U6_LADDER         305
#define OBJ_U6_HOLE           308
#define OBJ_U6_PORTCULLIS     310
#define OBJ_U6_CAVE           326

#define OBJ_U6_SIGN           332
#define OBJ_U6_SECRET_DOOR    334

class U6UseCode: public UseCode
{
 
 public:
 
 U6UseCode(Configuration *cfg);
 ~U6UseCode();
 
 bool use_obj(Obj *obj, Obj *src_obj=NULL);
 
 protected:
 
 bool use_door(Obj *obj);
 bool use_ladder(Obj *obj);
 bool use_passthrough(Obj *obj);
 bool use_switch(Obj *obj, uint16 target_obj_n);
 bool use_crank(Obj *obj);
 
 Obj *drawbridge_find(Obj *crank_obj);
 void drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width);

 bool use_firedevice_message(Obj *obj, bool lit);
};

#endif /* __U6UseCode_h__ */
