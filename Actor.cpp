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

#include "Map.h"

#include "Actor.h"

Actor::Actor(Map *m)
{
 map = m;
 direction = 0;
 walk_frame = 0;
}
 
Actor::~Actor()
{
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

void Actor::set_direction(uint8 d)
{
 if(d < 4)
   direction = d;

 walk_frame = (walk_frame + 1) % 4;
}

bool Actor::moveRelative(sint16 rel_x, sint16 rel_y)
{
 uint16 pitch;
 sint16 new_x, new_y;
 
 pitch = map->get_width(z);
 new_x = x + rel_x;
 new_y = y + rel_y;
 
 if(new_x < 0 || new_x >= pitch)
   return false;

 if(new_y < 0 || new_y >= pitch)
   return false;

 if(map->is_passable(new_x,new_y,z) ==  false)
   return false;
 
 x = new_x;
 y = new_y;
 
 return true;
}

void Actor::update()
{
 // do actor stuff here.
}

void Actor::loadSchedule(unsigned char *sched_data, uint16 num)
{
 uint16 i;
 
 if(num == 0)
     return;

 sched = (Schedule**)malloc(sizeof(Schedule*) * num+1);
 
 for(i=0;i<num;i++)
   {
    sched[i] = (Schedule *)malloc(sizeof(Schedule));

    sched[i]->hour = sched_data[0];
    sched[i]->unknown = sched_data[1];
    
    sched[i]->x = sched_data[2];
    sched[i]->x += (sched_data[3] & 0x3) << 8; 
    
    sched[i]->y = (sched_data[3] & 0xfc) >> 2;
    sched[i]->y += (sched_data[4] & 0xf) << 6;
    
    sched[i]->z = (sched_data[4] & 0xf0) >> 4;
    sched_data += 5;

    printf("#%04d %d,%d,%d hour %d unknown %d\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->unknown);
   }
   
 sched[i] = NULL;
 
 return;
}
