/*
 *  ActorManager.cpp
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

#include "ActorManager.h"


ActorManager::ActorManager(Configuration *cfg, Map *m, TileManager *tm, ObjManager *om, GameClock *c)
{
 config = cfg;
 map = m;
 tile_manager = tm;
 obj_manager = om;
 clock = c;
 
 player_actor = 1;
}

ActorManager::~ActorManager()
{
 uint16 i;
 
 for(i = 0;i < 256;i++) //we assume actors[] have been created by a call to loadActors()
   delete actors[i];

}
 
bool ActorManager::loadActors()
{
 uint16 i;
 uint8 b1, b2, b3;
 std::string filename;
 U6File objlist;
 U6File schedule;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename,"rb") == false)
   return false;


 objlist.seek(0x100); // Start of Actor position info
 
 for(i=0; i < 256; i++)
   {
    actors[i] = new Actor(map,obj_manager, clock);
    
    b1 = objlist.read1();
    b2 = objlist.read1();
    b3 = objlist.read1();
    
    actors[i]->x = b1;
    actors[i]->x += (b2 & 0x3) << 8; 
   
    actors[i]->y = (b2 & 0xfc) >> 2;
    actors[i]->y += (b3 & 0xf) << 6;
   
    actors[i]->z = (b3 & 0xf0) >> 4;
    actors[i]->id_n = (uint8)i;
/* Force avatar to gargoyle world
    if(i == 1)
     {
      actors[i]->x = 607;
      actors[i]->y = 285;
      actors[i]->z = 0;
     }
*/     
   }

 for(i=0;i < 256; i++)
   {
    b1 = objlist.read1();
    b2 = objlist.read1();
    actors[i]->a_num = b1;
    actors[i]->a_num += (b2 & 0x3) << 8;
    
    actors[i]->frame_n = (b2 & 0xfc) >> 2;
    actors[i]->direction = actors[i]->frame_n / 4;
   }



 // Experience
 
 objlist.seek(0xc00);
 
 for(i=0;i < 256; i++)
   {
    actors[i]->exp = objlist.read2();
   }
 
 // Health
 
 objlist.seek(0xe00);

 for(i=0;i < 256; i++)
   {
    actors[i]->hp = objlist.read1();
   }
 
 objlist.seek(0x17f1); // Start of Actor flags

 for(i=0;i < 256; i++)
   {
    actors[i]->flags = objlist.read1();
   }

 loadActorSchedules();
 
 return true;
}
 
Actor *ActorManager::get_actor(uint8 actor_num)
{
 return actors[actor_num];
}

Actor *ActorManager::get_actor(uint16 x, uint16 y, uint8 z)
{
 uint16 i;

 for(i=0;i<256;i++)
  {
   if(actors[i]->x == x && actors[i]->y == y && actors[i]->z == z)
     return actors[i];
  }

 return NULL;
}

Actor *ActorManager::get_player()
{
 return actors[player_actor]; //FIX here for dead party leader etc.
}

void ActorManager::set_player(Actor *a)
{
 player_actor = a->id_n;
}

void ActorManager::updateActors()
{
 uint16 i;
 
 for(i=0;i<256;i++)
  actors[i]->update();
  
}

bool ActorManager::loadActorSchedules()
{
 std::string filename;
 U6File schedule;
 uint16 i;
 uint16 index[256];
 uint16 s_num;
 uint32 bytes_read;
 unsigned char *sched_data;
 unsigned char *s_ptr;
 
 config->pathFromValue("config/ultima6/gamedir","schedule",filename);
 if(schedule.open(filename,"rb") == false)
   return false;
 
 for(i=0;i<256;i++)
   {
    index[i] = schedule.read2();
   }
 
 sched_data = schedule.readBuf(schedule.filesize() - 0x202, &bytes_read);
 
 s_ptr = sched_data + 2;

 for(i=0;i<256;i++)
  {
   if(i == 255) //Hmm a bit of a hack. might want to check if there are and scheduled events for Actor 255
     s_num = 0;
   else  
     s_num = index[i+1] - index[i];

   actors[i]->loadSchedule(s_ptr,s_num);
   s_ptr += s_num * 5;
  }
 
 free(sched_data);
 
 return true;
}
