/*
 *  U6UseCode.cpp
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
#include <cstdlib>
#include "U6UseCode.h"

U6UseCode::U6UseCode(Configuration *cfg) : UseCode(cfg)
{
    init_objects();
}

U6UseCode::~U6UseCode()
{
}


/* Create and fill usecode definition list.
 */
void U6UseCode::init_objects()
{
    memset(&uc_objects, 0, sizeof(uc_obj) * U6USE_NUM_OBJECTS);

    uc_objects[0].obj_n = OBJ_U6_QUEST_GATE;
    uc_objects[0].trigger |= USECODE_EVENT_STEPTO;
    uc_objects[0].ucf = &U6UseCode::pass_quest_barrier;

}


/* Return index of usecode definition in list for object N:F.
 */
sint16 U6UseCode::get_ucobject_index(uint16 n, uint8 f)
{
    for(uint32 o = 0; o < U6USE_NUM_OBJECTS; o++)
        if(uc_objects[o].obj_n == n && uc_objects[o].frame_n == f)
            return(o);
    return(-1);
}


/* Call usecode function `uco' from uc object list, with event `ev', for `obj'.
 * The meaning of the return value is different for each event, but false will
 * also be returned if `uco' isn't a valid index. (check that it is valid first)
 */
bool U6UseCode::uc_event(sint16 uco, uint8 ev, Obj *obj)
{
    if(uco < 0 || uco >= U6USE_NUM_OBJECTS)
        return(false);
//printf("__pfn=%x\n", uc_objects[uco].ucf.__pfn_or_delta2.__pfn);
    if(uc_objects[uco].trigger & ev)
    {
        printf("Usecode #%d (%d:%d), event 0x%02x\n", uco, obj->obj_n, obj->frame_n, ev);
        return((this->*uc_objects[uco].ucf)(obj, ev));
    }
    return(false); // doesn't respond to event
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

    case OBJ_U6_LEVER : use_switch(obj,OBJ_U6_PORTCULLIS);
                        scroll->display_string("\nswitch the lever, you hear a noise.\n");
                        break;

    case OBJ_U6_SWITCH : use_switch(obj,OBJ_U6_ELECTRIC_FIELD);
                         scroll->display_string("\nOperate the switch, you hear a noise.\n");
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
                         
    case OBJ_U6_VORTEX_CUBE : scroll->display_string("\nYou've finished the game!!\nPity we haven't implemented the end sequence yet.\n");
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

// for use with levers and switches, target_obj_n is either OBJ_U6_PORTCULLIS or OBJ_U6_ELECTRIC_FIELD
bool U6UseCode::use_switch(Obj *obj, uint16 target_obj_n)
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
      if(((Obj *)link->data)->obj_n == target_obj_n)
        {
         portc_obj = (Obj *)link->data;
         break;
        }
     }

    if(portc_obj == NULL) //no barrier object, so lets create one.
     {
      portc_obj = obj_manager->copy_obj(doorway_obj);
      portc_obj->obj_n = target_obj_n;
      portc_obj->quality = 0;
      if(target_obj_n == OBJ_U6_PORTCULLIS)
        {
         if(portc_obj->frame_n == 9) //FIX Hack for cream buildings might need one for virt wall. 
           portc_obj->frame_n = 1;
        }
      else
         portc_obj->frame_n = 0;
         
      obj_manager->add_obj(portc_obj,true);
     }
    else //delete barrier object.
     {
      obj_list->remove(portc_obj);
      delete portc_obj;
     }
   }
 
 toggle_frame(obj);
 

 
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


/* Event: Step to.
 * True: If Quest Flag is true, allow normal move.
 * False: Block if Quest Flag is false.
 */
bool U6UseCode::pass_quest_barrier(Obj *obj, uint8 ev)
{
    if(ev == USECODE_EVENT_STEPTO)
        if(player->get_quest_flag() == 0)
        {
            // block everyone, only print message when player attempts to pass
            if(itemref == player->get_actor()->get_actor_num())
            {
                scroll->display_string("\n\"Thou art not upon a Sacred Quest!\n"
                                       "Passage denied!\"\n\n");
                scroll->display_prompt();
            }
            return(false);
        }
    return(true);
}
