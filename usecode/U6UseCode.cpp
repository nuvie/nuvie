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

  switch(obj->obj_n)
   {
    case OBJ_U6_OAKEN_DOOR    :
    case OBJ_U6_WINDOWED_DOOR :
    case OBJ_U6_CEDAR_DOOR    :
    case OBJ_U6_STEEL_DOOR    : use_door(obj);
                                break;

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

    case OBJ_U6_SWITCH : toggle_frame(obj); //FIX hookup switch action.
                        break;

    case OBJ_U6_CRANK : use_crank(obj);
                        break;
                        
    case OBJ_U6_FIREPLACE : if(obj->frame_n == 1 || obj->frame_n == 3)
                                {
                                 use_firedevice_message(obj,false);
                                 obj->frame_n--;
                                }
                              else
                                {
                                 use_firedevice_message(obj,true);
                                 obj->frame_n++;
                                }
                              break;
                              
    case OBJ_U6_SECRET_DOOR : if(obj->frame_n == 1 || obj->frame_n == 3)
                                obj->frame_n--;
                              else
                                obj->frame_n++;
                              break;
    case OBJ_U6_CANDLE :
    case OBJ_U6_CANDELABRA :
    case OBJ_U6_BRAZIER :
                         toggle_frame(obj);
                         use_firedevice_message(obj,(bool)obj->frame_n);
                         break;
                         
    default : scroll->display_string("\nnot usable\n");
              break;
   }


 printf("Use Obj #%d Frame #%d\n",obj->obj_n, obj->frame_n);

 return true;
}

bool U6UseCode::use_door(Obj *obj)
{
 Obj *key_obj;
 
 if(obj->frame_n == 9 || obj->frame_n == 11) // locked door
   {
    key_obj = player->get_actor()->inventory_get_object(OBJ_U6_KEY, obj->quality);
    if(key_obj != NULL) // we have the key for this door so lets unlock it.
      {
       obj->frame_n -= 4;
       scroll->display_string("\nunlocked\n");
      }
    else
       scroll->display_string("\nlocked\n");

    return true;
   }
  
 if(obj->frame_n <= 3) //open door
   {
    obj->frame_n += 4;
    scroll->display_string("\nclosed!\n");
   }
 else
   {
    obj->frame_n -= 4;
    scroll->display_string("\nopened!\n");
   }
   
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
 Obj *doorway_obj;
 Obj *portc_obj;
 U6LList *obj_list;
 U6Link *link;
 
 doorway_obj = obj_manager->find_obj(OBJ_U6_DOORWAY, obj->quality, obj->z);
 
 for(;doorway_obj != NULL;doorway_obj = obj_manager->find_next_obj(doorway_obj))
   {
    obj_list = obj_manager->get_obj_list(doorway_obj->x,doorway_obj->y,doorway_obj->z);

    for(portc_obj=NULL,link=obj_list->start();link != NULL;link=link->next) // find existing portcullis.
     {
      if(((Obj *)link->data)->obj_n == OBJ_U6_PORTCULLIS)
        {
         portc_obj = (Obj *)link->data;
         break;
        }
     }

    if(portc_obj == NULL) //no portcullis object, so lets create one.
     {
      portc_obj = obj_manager->copy_obj(doorway_obj);
      portc_obj->obj_n = OBJ_U6_PORTCULLIS;
      portc_obj->quality = 0;
      if(portc_obj->frame_n == 9) //FIX Hack for cream buildings might need one for virt wall. 
        portc_obj->frame_n = 1;
      obj_manager->add_obj(portc_obj,true);
     }
    else //delete portcullis object.
     {
      obj_list->remove(portc_obj);
      delete portc_obj;
     }
   }
 
 toggle_frame(obj);
 
 scroll->display_string("\nswitch the lever, you hear a noise.\n");
 
 return true;
}

//cranks control drawbridges.

bool U6UseCode::use_crank(Obj *obj)
{
 uint16 x,y;
 uint8 level;
 uint16 b_width;
 bool bridge_open;
 Obj *start_obj;
 
 start_obj = drawbridge_find(obj);
 
 if(start_obj->frame_n == 3) // bridge open
    bridge_open = true;
 else
    bridge_open = false;

 x = start_obj->x;
 y = start_obj->y;
 level = start_obj->z;
 
 drawbridge_remove(x, y, level, &b_width);

 if(bridge_open)
   drawbridge_close(x, y, level, b_width);
 else
   drawbridge_open(x, y, level, b_width);
    
 return true;
}

Obj *U6UseCode::drawbridge_find(Obj *crank_obj)
{
 uint16 i,j;
 Obj *start_obj, *tmp_obj;
 bool closed;
 
 for(i=0;i<6;i++) // search on right side of crank.
  {
   start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, crank_obj->x+1, crank_obj->y+i,  crank_obj->z);
   if(start_obj != NULL) // this means we are using the left crank.
     return start_obj;
  }

 for(i=0;i<6;i++) // search on left side of crank.
  {
   tmp_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, crank_obj->x-1, crank_obj->y+i,  crank_obj->z);
   
   if(tmp_obj != NULL) // this means we are using the right crank.
     {
      //find the start of the drawbridge on the left.
      // we do this by searching to the left of the crank till we hit the crank on the otherside.
      // we then move right 1 tile and down 'i' tiles to the start object. :) 
      for(j=1; j < crank_obj->x; j++)
        {
         tmp_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_CRANK, crank_obj->x-j, crank_obj->y,  crank_obj->z);
         if(tmp_obj && tmp_obj->obj_n == OBJ_U6_CRANK)
           {
            start_obj = obj_manager->get_obj_of_type_from_location(OBJ_U6_DRAWBRIDGE, tmp_obj->x+1, tmp_obj->y+i,  tmp_obj->z);
            return start_obj;
           }
        }
     }
  }

 return NULL;
}

void U6UseCode::drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width)
{
 uint16 i,j;
 Obj *obj;
 
 y++;
 
 for(i=0;;i++)
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,3,x,y+i,level); //left side chain
   obj_manager->add_obj(obj,true);
     
   obj = new_obj(OBJ_U6_DRAWBRIDGE,5,x+b_width-1,y+i,level); //right side chain
   obj_manager->add_obj(obj,true);

   for(j=0;j<b_width-2;j++)
    {     
     obj = new_obj(OBJ_U6_DRAWBRIDGE,4,x+1+j,y+i,level);
     obj_manager->add_obj(obj,true);
    }
    
   if(map->is_passable(x,y+i+1,level)) //we extend the drawbridge until we hit a passable tile.
    break; 
  }
 
 i++;
 
 for(j=0;j<b_width-2;j++) //middle bottom tiles
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,1,x+1+j,y+i,level);  
   obj_manager->add_obj(obj,true);
  }
  
 obj = new_obj(OBJ_U6_DRAWBRIDGE,0,x,y+i,level); //bottom left
 obj_manager->add_obj(obj,true);
     
 obj = new_obj(OBJ_U6_DRAWBRIDGE,2,x+b_width-1,y+i,level);  // bottom right
 obj_manager->add_obj(obj,true);

 scroll->display_string("\nOpen the drawbridge.\n");
 
 return;
}

void U6UseCode::drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width)
{
 uint16 i;
 Obj *obj;
 
 y--;
 
 obj = new_obj(OBJ_U6_DRAWBRIDGE,6,x-1,y,level);  //left side
 obj_manager->add_obj(obj,true);
 
 obj = new_obj(OBJ_U6_DRAWBRIDGE,8,x+b_width-1,y,level);  //right side     
 obj_manager->add_obj(obj,true);
 
 for(i=0;i<b_width-1;i++)
  {
   obj = new_obj(OBJ_U6_DRAWBRIDGE,7,x+i,y,level);  //middle        
   obj_manager->add_obj(obj,true);
  }

 scroll->display_string("\nClose the drawbridge.\n");
}

void U6UseCode::drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width)
{ 
 uint16 w,h;
 Obj *obj;

 //remove end of closed drawbridge.
 // if present.
 if(x > 0)
   obj_manager->remove_obj_type_from_location(OBJ_U6_DRAWBRIDGE,x-1,y,level);
 
 *bridge_width = 0;
 
 //remove the rest of the bridge.
 for(h=0,w=1;w != 0;h++)
  {
   for(w=0;;w++)
    {
     if(obj_manager->remove_obj_type_from_location(OBJ_U6_DRAWBRIDGE,x+w,y+h,level) == false)
       {
        if(w != 0)
          *bridge_width = w;
        break;
       }
    }
  }

 return;
}

bool U6UseCode::use_firedevice_message(Obj *obj, bool lit)
{
 scroll->display_string("\n");
 scroll->display_string(obj_manager->get_obj_name(obj));
 if(lit)
   scroll->display_string(" is lit.\n");
 else
   scroll->display_string(" is doused.\n");

 return true;
}
