/*
 *  EggManager.cpp
 *  Nuvie
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

#include <list>
#include <cassert>
#include "nuvieDefs.h"
#include "Configuration.h"
 
#include "Actor.h"
#include "TileManager.h"
#include "ActorManager.h"
#include "U6misc.h"
#include "U6LList.h"
#include "EggManager.h"
#include "misc.h"
#include "NuvieIOFile.h"
#include "GameClock.h"


EggManager::EggManager(Configuration *cfg, Map *m)
{
 config = cfg;
 map = m;
}

EggManager::~EggManager()
{
 std::list<Egg *>::iterator egg;
 std::list<Egg *>::iterator next_egg;
 
 for(egg = egg_list.begin(); egg != egg_list.end();)
   {
    next_egg = egg_list.erase(egg);
    delete *egg;
    egg = next_egg;
   }

}
 
void EggManager::add_egg(Obj *egg_obj)
{
 Egg *egg;
 
 if(egg_obj == NULL)
  return;

 egg = new Egg();
 egg->obj = egg_obj;
 
 egg_list.push_back(egg);

 return;
}


void EggManager::remove_egg(Obj *egg_obj)
{
 std::list<Egg *>::iterator egg;

 for(egg = egg_list.begin(); egg != egg_list.end();)
   {
    if((*egg)->obj == egg_obj)
       {
        egg_list.remove(*egg);
        delete *egg;
        
        break;
       }

    egg++;
   }

 return;
}

void EggManager::spawn_eggs(uint16 x, uint16 y, uint8 z)
{
 std::list<Egg *>::iterator egg;
 sint16 dist_x, dist_y;
 uint8 hatch_probability;

 for(egg = egg_list.begin(); egg != egg_list.end();)
   {
    dist_x = abs((sint16)(*egg)->obj->x - x);
    dist_y = abs((sint16)(*egg)->obj->y - y);

    if(dist_x <= 32 && dist_y <= 32 && (*egg)->obj->z == z)
      {
       
       if(((*egg)->obj->status & OBJ_STATUS_EGG_ACTIVE) == 0 || ((*egg)->obj->status & OBJ_STATUS_SEEN_EGG) == 0)
         {
          (*egg)->obj->status |= OBJ_STATUS_SEEN_EGG;
		  (*egg)->obj->status |= OBJ_STATUS_EGG_ACTIVE;
          
          hatch_probability = NUVIE_RAND()%100;
          printf("Checking Egg (%x,%x,%x). Rand: %d Probability: %d%%\n",(*egg)->obj->x, (*egg)->obj->y, (*egg)->obj->z,hatch_probability,(*egg)->obj->qty);
          spawn_egg((*egg)->obj, hatch_probability);
         }
      }
    else
      (*egg)->obj->status &= (0xff ^ OBJ_STATUS_EGG_ACTIVE);

    egg++;
   }
 
 return;
}


bool EggManager::spawn_egg(Obj *egg, uint8 hatch_probability)
{
 U6Link *link;
 uint16 i;
 Obj *obj, *spawned_obj;
          // check random probability that the egg will hatch
          if(egg->qty == 100 || hatch_probability <= egg->qty)  // Hatch the egg.
            {
             assert(egg->container);
             for(link = egg->container->start(); link != NULL; link = link->next)
               {
                obj = (Obj *)link->data;
                for(i = 0; i < obj->qty; i++)
                 {
				  if(obj->quality != 0) /* spawn temp actor we know it's an actor if it has a non-zero worktype. */
                     // line actors up in a row
//                     actor_manager->create_temp_actor(obj->obj_n,egg->x+i,egg->y,egg->z,obj->quality);
				  {
				  	// group new actors randomly if egg space already occupied
				  	Actor *prev_actor = actor_manager->get_actor(egg->x, egg->y, egg->z);
				  	Actor *new_actor = NULL;
				  	if(actor_manager->create_temp_actor(obj->obj_n,egg->x,egg->y,egg->z,obj->quality,&new_actor) && prev_actor)
					{
						// try to group actors of the same type first (FIXME: maybe this should use alignment/quality)
						if(prev_actor->get_obj_n() != new_actor->get_obj_n() || !actor_manager->toss_actor(new_actor, 3, 2) || !actor_manager->toss_actor(new_actor, 2, 3))
							actor_manager->toss_actor(new_actor, 4, 4);
						hatch_probability = NUVIE_RAND()%100;
						if(hatch_probability > egg->qty)
							break; // chance to stop spawning actors
					}
				  }
				  else
					{ /* spawn temp object */
					 spawned_obj = new Obj();
					 spawned_obj->obj_n = obj->obj_n;
//					 spawned_obj->x = egg->x+i; // line objects up in a row
					 spawned_obj->x = egg->x;
					 spawned_obj->y = egg->y;
					 spawned_obj->z = egg->z;
//					 spawned_obj->qty = obj->qty;
					 spawned_obj->qty = 1; // (it already spawns qty objects with the loop)
					 spawned_obj->status |= OBJ_STATUS_TEMPORARY | OBJ_STATUS_OK_TO_TAKE;

					 obj_manager->add_obj(spawned_obj);
					}
                 }
               }
             return true;
            }
 return false;
}
