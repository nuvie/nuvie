/*
 *  Actor.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 20 2003.
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

#include "Map.h"
#include "ObjManager.h"

#include "Actor.h"

static uint8 walk_frame_tbl[4] = {0,1,2,1};

Actor::Actor(Map *m, ObjManager *om, GameClock *c)
:sched(NULL)
{
 map = m;
 obj_manager = om;
 
 clock = c;
 direction = 0;
 walk_frame = 0;
 standing = true;
 in_party = false;
 worktype = 0;
 sched_pos = 0;
}
 
Actor::~Actor()
{
 // free sched array
 if (sched != NULL)
 {
    Schedule** cursched = sched;
    while(*cursched != NULL)
       free(*cursched++);

    free(sched);
 }
}

bool Actor::is_alive()
{
 return alive;
}

void Actor::get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level)
{
 *ret_x = x;
 *ret_y = y;
 *ret_level = z;
}

uint16 Actor::get_tile_num()
{
 return obj_n;// + frame_n;
}

uint8 Actor::get_worktype()
{
 return worktype;
}

void Actor::set_direction(uint8 d)
{
 if(d < 4)
   direction = d;

 walk_frame = (walk_frame + 1) % 4;
 
 frame_n = direction * 4 + walk_frame_tbl[walk_frame];
 
}

bool Actor::moveRelative(sint16 rel_x, sint16 rel_y)
{
 return move(x + rel_x, y + rel_y, z);
}

bool Actor::move(sint16 new_x, sint16 new_y, sint8 new_z)
{
 uint16 pitch;
 Obj *obj;
 
 if(z > 5)
   return false;
  
 pitch = map->get_width(new_z);
 
 if(new_x < 0 || new_x >= pitch)
   return false;

 if(new_y < 0 || new_y >= pitch)
   return false;

 if(map->is_passable(new_x,new_y,new_z) ==  false)
   return false;
 
 x = new_x;
 y = new_y;
 z = new_z;
 
 obj = obj_manager->get_obj(x,y,z);
 if(obj)
  {
   if(obj->obj_n == OBJ_U6_CHAIR)  // make the actor sit on a chair.
     frame_n = (obj->frame_n * 4) + 3;
  }
 return true;
}

void Actor::update()
{
 uint8 new_direction;
/* 
 // do actor stuff here.
 if(standing)
  {
   if(NUVIE_RAND()%50 == 1)
     {
      walk_frame = NUVIE_RAND()%4;
      frame_n = direction * 4 + walk_frame_tbl[walk_frame];
     }
   else
    {
     if(!in_party)
       {       
        if(NUVIE_RAND()%80 == 1)
          {
           new_direction = NUVIE_RAND()%4;
           set_direction(new_direction);
           switch(new_direction)
            {
             case 0 : moveRelative(0,-1); break;
             case 1 : moveRelative(1,0); break;
             case 2 : moveRelative(0,1); break;
             case 3 : moveRelative(-1,0); break;
            }
          }
       }
    }
  }
*/
 updateSchedule();

}

void Actor::set_in_party(bool state)
{
 in_party = state;
}

bool Actor::inventory_has_object(uint16 obj_n)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 
 inventory = obj_manager->get_actor_inventory(id_n);
 
 for(link=inventory->start();link != NULL;link=link->next)
   {
    obj = (Obj *)link->data;
    if(obj->obj_n == obj_n)
      return true;
   }
   
 return false;
}

bool Actor::inventory_add_object(uint16 obj_n, uint8 qty, uint8 quality)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 
 inventory = obj_manager->get_actor_inventory(id_n);
 obj = new Obj;
 
 obj->obj_n = obj_n;
 obj->qty = qty;
 obj->quality = quality;
 
 inventory->addAtPos(0,obj);
 
 return true;
}

bool Actor::inventory_del_object(uint16 obj_n, uint8 qty, uint8 quality)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 
 inventory = obj_manager->get_actor_inventory(id_n);

 //FIX ME here!
 
 return false;
}

void Actor::loadSchedule(unsigned char *sched_data, uint16 num)
{
 uint16 i;
 unsigned char *sched_data_ptr;
 
 sched = (Schedule**)malloc(sizeof(Schedule*) * (num+1));
 
 sched_data_ptr = sched_data;
 
 for(i=0;i<num;i++)
   {
    sched[i] = (Schedule *)malloc(sizeof(Schedule));

    sched[i]->hour = sched_data_ptr[0];
    sched[i]->worktype = sched_data_ptr[1];
    
    sched[i]->x = sched_data_ptr[2];
    sched[i]->x += (sched_data_ptr[3] & 0x3) << 8; 
    
    sched[i]->y = (sched_data_ptr[3] & 0xfc) >> 2;
    sched[i]->y += (sched_data_ptr[4] & 0xf) << 6;
    
    sched[i]->z = (sched_data_ptr[4] & 0xf0) >> 4;
    sched_data_ptr += 5;
#ifdef DEBUG
    printf("#%04d %d,%d,%d hour %d worktype %d\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->worktype);
#endif
   }
   
 sched[i] = NULL;
 
 sched_pos = getSchedulePos(clock->get_hour());
 
 if(sched[sched_pos] != NULL)
    setWorktype(sched[sched_pos]->worktype);

 return;
}

void Actor::updateSchedule()
{
 uint8 hour;
 uint16 new_pos;
 
 hour = clock->get_hour();
 
 new_pos = getSchedulePos(hour);
 
 if(new_pos == sched_pos)
  return;

 sched_pos = new_pos;
 
 if(sched[sched_pos] == NULL)
   return;
 
 move(sched[sched_pos]->x,sched[sched_pos]->y,sched[sched_pos]->z);
 
 setWorktype(sched[sched_pos]->worktype);
 
}

// returns the current schedule entry based on hour
uint16 Actor::getSchedulePos(uint8 hour)
{
 uint16 i;
  
 for(i=0;sched[i] != NULL;i++)
  {
   if(sched[i]->hour > hour)
     {
      if(i != 0)
        return i-1;
      else // i == 0 this means we are in the last schedule entry
        for(;sched[i+1] != NULL;)
          i++;
     }
  }
  
 if(i==0)
  return 0;

 return i-1;
}

bool Actor::setWorktype(uint8 new_worktype)
{
 worktype = new_worktype;
 
 return true;
}

/* Set NPC flag `bitflag' to 1.
 */
void Actor::set_flag(uint8 bitflag)
{
    if(bitflag > 7)
        return;
    this->set_flags(this->get_flags() | (1 << bitflag));
}


/* Set NPC flag `bitflag' to 0.
 */
void Actor::clear_flag(uint8 bitflag)
{
    if(bitflag > 7)
        return;
    this->set_flags(this->get_flags() & ~(1 << bitflag));
}

