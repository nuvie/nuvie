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
#include "U6UseCode.h"
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


/* Returns true if another NPC `n' is in proximity to this one.
 */
bool Actor::is_nearby(Actor *other)
{
    uint16 x, y, x2, y2;
    uint8 l, l2;

    this->get_location(&x, &y, &l);
    other->get_location(&x2, &y2, &l2);
    if(abs(x - x2) <= 18 && abs(y - y2) <= 18 && l == l2)
        return(true);
    return(false);
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


/* Re-orient actor to be facing an x,y location on the map (so they turn to
 * talk to others, etc)
 */
void Actor::face_location(uint16 lx, uint16 ly)
{
    uint16 xdiff = abs(x - lx), ydiff = abs(y - ly);
    if(ydiff)
    {
        if(y < ly && direction != 2)
            set_direction(2); // down
        else if(y > ly && direction != 0)
            set_direction(0); // up
    }
    if(xdiff)
    {
        if(x < lx && direction != 1)
            set_direction(1); // right
        else if(x > lx && direction != 3)
            set_direction(3); // left
    }
}

void Actor::face_actor(Actor *a)
{
 uint16 ax, ay;
 uint8 al;
    
 a->get_location(&ax, &ay, &al);
 face_location(ax, ay);
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
/*
 uint8 new_direction;
 
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

U6LList *Actor::get_inventory_list()
{
 return obj_manager->get_actor_inventory(id_n); 
}


bool Actor::inventory_has_object(uint16 obj_n, uint8 qual)
{
    if(inventory_get_object(obj_n, qual))
        return(true);
    return(false);
}


/* Returns the number of objects in the actor's inventory with matching object
 * number and quality.
 */
uint32 Actor::inventory_count_object(uint16 obj_n, uint8 qual, Obj *container)
{
    uint32 qty = 0;
    U6Link *link = 0;
    Obj *obj = 0;
    U6LList *inv = container ? container->container
                             : get_inventory_list();

    for(link = inv->start(); link != NULL; link = link->next)
    {
        obj = (Obj *)link->data;
        if(obj->container)
            inventory_count_object(obj_n, qual, obj);
        if(obj->obj_n == obj_n && obj->quality == qual)
            qty += obj->qty;
    }
    return(qty);
}


/* Returns object descriptor of object in the actor's inventory, or NULL if no
 * matching object is found.
 */
Obj *Actor::inventory_get_object(uint16 obj_n, uint8 qual, Obj *container)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 
 inventory = container ? container->container
                       : get_inventory_list();
 for(link=inventory->start();link != NULL;link=link->next)
   {
    obj = (Obj *)link->data;
    if(obj->obj_n == obj_n && obj->quality == qual)
      return(obj);
    else if(obj->container)
    {
      if((obj = inventory_get_object(obj_n, qual, obj)))
        return(obj);
    }
   }

 return NULL;
}


bool Actor::inventory_add_object(uint16 obj_n, uint32 qty, uint8 quality)
{
 U6LList *inventory = 0;
 U6Link *link = 0;
 Obj *obj = 0;
 uint32 origqty = 0, newqty = 0;
 
 inventory = get_inventory_list();

 // find same type of object in inventory, don't look in containers
 for(link = inventory->start(); link != NULL; link = link->next)
 {
   obj = (Obj *)link->data;
   if(obj->obj_n == obj_n && obj->quality == quality)
     break;
   obj = 0;
 }
 // define and link (when stacking, quantity of 0 is 1)
 origqty = (obj && obj->qty) ? obj->qty : 1;
 newqty = qty ? qty : 1;
 if(obj && ((origqty + newqty) <= 255))
 {
    origqty += newqty;
    obj->qty = origqty;
 }
 else // don't stack
   while(newqty)
   {
     obj = new Obj;
     obj->obj_n = obj_n;
     obj->quality = quality;
     obj->qty = newqty <= 255 ? newqty : 255;
     inventory->addAtPos(0, obj);
     newqty -= obj->qty;
   }
 return true;
}


uint32
Actor::inventory_del_object(uint16 obj_n, uint32 qty, uint8 quality, Obj *container)
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 uint8 oqty = 0;
 uint32 deleted = 0;
 
 inventory = container ? container->container : get_inventory_list();
 for(link = inventory->start(); deleted < qty && link != NULL; link = link->next)
 {
    obj = (Obj *)link->data;
    if(obj->container)
        deleted += inventory_del_object(obj_n, qty - deleted, quality, obj);
    if((deleted < qty) && (obj->obj_n == obj_n) && (obj->quality == quality))
    {
        oqty = obj->qty ? obj->qty : 1;
        if(oqty <= (qty - deleted))
            inventory->remove(obj);
        else
            obj->qty = oqty - qty;
        deleted += oqty;
    }
 }
 return(deleted);
}


float Actor::get_inventory_weight()
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 float weight = 0;
 
 if(obj_manager->actor_has_inventory(id_n) == false)
   return 0;

 inventory = obj_manager->get_actor_inventory(id_n);

 for(link=inventory->start();link != NULL;link=link->next)
  {
   obj = (Obj *)link->data;
   weight += obj_manager->get_obj_weight(obj);
  }

 weight /= 10;
 
 return weight;
}

float Actor::get_inventory_equip_weight()
{
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 float weight = 0;
 
 if(obj_manager->actor_has_inventory(id_n) == false)
   return 0;

 inventory = obj_manager->get_actor_inventory(id_n);

 for(link=inventory->start();link != NULL;link=link->next)
  {
   obj = (Obj *)link->data;
   if((obj->status & 0x18) == 0x18) //object readied
      weight += obj_manager->get_obj_weight(obj);
  }

 weight /= 10;
 
 return weight;
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
    printf("#%04d %03x,%03x,%x hour %2d worktype %02x\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->worktype);
#endif
   }
   
 sched[i] = NULL;
 
 sched_pos = getSchedulePos(clock->get_hour());
 
 if(sched[sched_pos] != NULL)
    set_worktype(sched[sched_pos]->worktype);

 return;
}

void Actor::updateSchedule()
{
 uint8 hour;
 uint16 new_pos;
 
 hour = clock->get_hour();
 
 new_pos = getSchedulePos(hour);
 
 if(new_pos == sched_pos) // schedules are the same so we do nothing.
  return;

 sched_pos = new_pos;
 
 if(sched[sched_pos] == NULL)
   return;
 
 move(sched[sched_pos]->x,sched[sched_pos]->y,sched[sched_pos]->z);
 
 set_worktype(sched[sched_pos]->worktype);
 
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

void Actor::set_worktype(uint8 new_worktype)
{
 worktype = new_worktype;
 
 return ;
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

