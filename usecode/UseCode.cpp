/*
 *  UseCode.cpp
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

#include "U6LList.h"
#include "MsgScroll.h"
#include "UseCode.h"

UseCode::UseCode(Configuration *cfg)
{
 config = cfg;

 int_ref = 0;
 actor_ref = NULL;
 obj_ref = NULL;
 actor2_ref = NULL;
}

UseCode::~UseCode()
{
}

bool UseCode::init(ObjManager *om, Map *m, Player *p, MsgScroll *ms)
{
 obj_manager = om;
 map = m;
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

bool UseCode::use_container(Obj *obj, bool print)
{
 Obj *temp_obj;
 U6Link *obj_link;

 /* Test whether this object has items inside it. */
 if((obj->container != NULL) &&
   ((obj_link = obj->container->end()) != NULL))
    {
     U6LList *obj_list = obj_manager->get_obj_list(obj->x, obj->y, obj->z);

     /* Add objects to obj_list. */
     for(; obj_link != NULL; obj_link = obj_link->prev)
      {
       temp_obj = (Obj*)obj_link->data;
       obj_list->add(temp_obj);
       temp_obj->status = OBJ_STATUS_OK_TO_TAKE;
       temp_obj->x = obj->x;
       temp_obj->y = obj->y;
       temp_obj->z = obj->z;
       if(print)
        {
         scroll->display_string(obj_manager->look_obj(temp_obj,true));
         if(obj_link->prev) // more objects left
           scroll->display_string(obj_link->prev->prev ? ", " : ", and ");
        }
      }

     /* Remove objects from the container. */
     obj->container->removeAll();
     return true;
    }
 return false;
}
