/*
 *  U6UseCode.cpp
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

#include "U6UseCode.h"

U6UseCode::U6UseCode(Configuration *cfg) : UseCode(cfg)
{

}

U6UseCode::~U6UseCode()
{
}

bool U6UseCode::use_obj(Obj *obj, Obj *src_obj)
{
 
 if(obj == NULL)
   return false;

 if(obj->obj_n >= 297 && obj->obj_n <= 300) // door objects
   {
    if(obj->frame_n <= 3)
      obj->frame_n += 4;
    else
      obj->frame_n -= 4;
   }

  switch(obj->obj_n)
   {
    case OBJ_U6_LADDER :
    case OBJ_U6_HOLE : use_ladder(obj);
                       break;

    case OBJ_U6_CHEST :
    case OBJ_U6_CRATE :
    case OBJ_U6_BARREL : toggle_frame(obj); //open / close object
    case OBJ_U6_DRAWER :
    case OBJ_U6_BAG : use_container(obj);
                      break;
                      
    case OBJ_U6_V_PASSTHROUGH :
    case OBJ_U6_H_PASSTHROUGH : use_passthrough(obj);
                                break;

    case OBJ_U6_LEVER : use_lever(obj);
                        break;

    case OBJ_U6_FIREPLACE :
    case OBJ_U6_SECRET_DOOR : if(obj->frame_n == 1 || obj->frame_n == 3)
                                obj->frame_n--;
                              else
                                obj->frame_n++;
                              break;
    case OBJ_U6_CANDLE :
    case OBJ_U6_BRAZIER :
                         toggle_frame(obj);
                         break;
   }


 printf("Use Obj #%d Frame #%d\n",obj->obj_n, obj->frame_n);

 return true;
}


bool U6UseCode::use_ladder(Obj *obj)
{
 if(obj->frame_n == 0) // DOWN
   {
    if(obj->z == 0) //handle the transition from the surface to the first dungeon level
      {
       player->move(((obj->x & 0x07) | (obj->x >> 2 & 0xF8)), ((obj->y & 0x07) | (obj->y >> 2 & 0xF8)), obj->z+1);
      }
    else
       player->move(obj->x,obj->y,obj->z+1); //dungeon ladders line up so we simply drop straight down
   }
 else //UP
   {
    if(obj->z == 1)
      {
       //FIX clean his up a bit. :)
       player->move(obj->x / 8 * 8 * 4 + ((obj->quality & 0x03) * 8) + (obj->x - obj->x / 8 * 8),
                    obj->y / 8 * 8 * 4 + ((obj->quality >> 2 & 0x03) * 8) + (obj->y - obj->y / 8 * 8),
                    obj->z-1);

      }
    else
       player->move(obj->x,obj->y,obj->z-1);
   }

 return true;
}

bool U6UseCode::use_container(Obj *obj)
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
      }

     /* Remove objects from the container. */
     obj->container->removeAll();
     return true;
    }

 return false;
}

bool U6UseCode::use_passthrough(Obj *obj)
{
 if(obj->obj_n == OBJ_U6_V_PASSTHROUGH)
  {
   if(obj->frame_n < 2)
    {
     obj_manager->move(obj,obj->x,obj->y-1,obj->z);
     obj->frame_n = 2;
    }
   else
    {
     obj_manager->move(obj,obj->x,obj->y+1,obj->z);
     obj->frame_n = 0;
    }
  }
 else // OBJ_U6_H_PASSTHROUGH
  {
   if(obj->frame_n < 2)
    {
     obj_manager->move(obj,obj->x-1,obj->y,obj->z);
     obj->frame_n = 2;
    }
   else
    {
     obj_manager->move(obj,obj->x+1,obj->y,obj->z);
     obj->frame_n = 0;
    }
  }
  
 return true;
}

bool U6UseCode::use_lever(Obj *obj)
{
 toggle_frame(obj);
 
 //FIX do lever work here.
 
 return true;
}