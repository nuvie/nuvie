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

#include "Actor.h"

static uint8 walk_frame_tbl[4] = {0,1,2,1};

Actor::Actor(Map *m)
{
 map = m;
 direction = 0;
 walk_frame = 0;
 standing = true;
 in_party = false;
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

uint16 Actor::get_tile_num()
{
 return a_num;// + frame_n;
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
 
 return true;
}

void Actor::update()
{
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

}

void Actor::set_in_party(bool state)
{
 in_party = state;
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
    sched[i]->worktype = sched_data[1];
    
    sched[i]->x = sched_data[2];
    sched[i]->x += (sched_data[3] & 0x3) << 8; 
    
    sched[i]->y = (sched_data[3] & 0xfc) >> 2;
    sched[i]->y += (sched_data[4] & 0xf) << 6;
    
    sched[i]->z = (sched_data[4] & 0xf0) >> 4;
    sched_data += 5;
#ifdef DEBUG
    printf("#%04d %d,%d,%d hour %d unknown %d\n",id_n,sched[i]->x,sched[i]->y,sched[i]->z,sched[i]->hour,sched[i]->unknown);
#endif
   }
   
 sched[i] = NULL;
 
 return;
}
