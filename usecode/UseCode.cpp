/*
 *  UseCode.cpp
 *  Nuive
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

#include "UseCode.h"

UseCode::UseCode(Configuration *cfg)
{
 config = cfg;
}

UseCode::~UseCode()
{
}

bool UseCode::init(ObjManager *om, Player *p, MsgScroll *ms)
{
 obj_manager = om;
 player = p;
 scroll = ms;
 
 return true;
}

 bool UseCode::use_obj(uint16 x, uint16 y, uint8 z, Obj *src_obj)
 {
  Obj *obj;
  
  obj = obj_manager->get_obj(x,y,z,true);
  
  if(obj == NULL)
    return false;
  
  return use_obj(obj,src_obj);
 }
 
void UseCode::toggle_frame(Obj *obj)
{
 if(obj->frame_n > 0)
   obj->frame_n--;
 else
   obj->frame_n = 1;
}
 