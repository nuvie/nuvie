/*
 *  MDUseCode.cpp
 *  Nuive
 *
 *  Created by Markus Niemistö on Wed Jun 18 2003.
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

#include "MDUseCode.h"

MDUseCode::MDUseCode(Configuration *cfg) : UseCode(cfg)
{

}

MDUseCode::~MDUseCode()
{
}

bool MDUseCode::use_obj(Obj *obj, Obj *src_obj)
{
 
 if(obj == NULL)
   return false;

  switch(obj->obj_n)
   {
    case OBJ_MD_BRASS_TRUNK :
                      toggle_frame(obj); //open / close object
    case OBJ_MD_BAG : use_container(obj);
                      break;
                      
    default : scroll->display_string("\nnot usable\n");
              break;
   }


 printf("Use Obj #%d Frame #%d\n",obj->obj_n, obj->frame_n);

 return true;
}

bool MDUseCode::use_ladder(Obj *obj)
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
       //we use obj->quality to tell us which surface chunk to come up in.
      player->move(obj->x / 8 * 8 * 4 + ((obj->quality & 0x03) * 8) + (obj->x - obj->x / 8 * 8),
                    obj->y / 8 * 8 * 4 + ((obj->quality >> 2 & 0x03) * 8) + (obj->y - obj->y / 8 * 8),
                    obj->z-1);
      }
    else
       player->move(obj->x,obj->y,obj->z-1);
   }

 return true;
}

bool MDUseCode::use_container(Obj *obj)
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

