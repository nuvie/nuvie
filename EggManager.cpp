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
#include "NuvieIOFile.h"
#include "GameClock.h"
#include "Game.h"
#include "U6objects.h" //needed for silver serpent exception

/* ALWAYS means the time is unset, is unknown, or day and night are both set */
typedef enum
{
    EGG_HATCH_ALWAYS, EGG_HATCH_DAY, EGG_HATCH_NIGHT
} egg_hatch_time;
#define EGG_DAY_HOUR   06 /* first hour of the day */
#define EGG_NIGHT_HOUR 19 /* first hour of night */
#define EGG_HATCH_TIME(EQ) (EQ < 10) ? EGG_HATCH_ALWAYS \
                            : (EQ < 20) ? EGG_HATCH_DAY \
                            : (EQ < 30) ? EGG_HATCH_NIGHT : EGG_HATCH_ALWAYS;


EggManager::EggManager(Configuration *cfg, nuvie_game_t type, Map *m)
{
 config = cfg;
 gametype = type;
 map = m;
}

EggManager::~EggManager()
{

}

void EggManager::clean(bool keep_obj)
{
 Egg *egg;
 std::list<Egg *>::iterator egg_iter;

 for(egg_iter = egg_list.begin(); egg_iter != egg_list.end();)
   {
    egg = *egg_iter;

   // eggs are always on the map now.
   // if(keep_obj == false)
   //   delete_obj(egg->obj);

    delete *egg_iter;
    egg_iter = egg_list.erase(egg_iter);
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


void EggManager::remove_egg(Obj *egg_obj, bool keep_obj)
{
 std::list<Egg *>::iterator egg_iter;

 for(egg_iter = egg_list.begin(); egg_iter != egg_list.end(); egg_iter++)
   {
    if((*egg_iter)->obj == egg_obj)
       {
        //if(keep_obj == false) eggs always on map now.

        obj_manager->unlink_from_engine((*egg_iter)->obj);
        delete_obj((*egg_iter)->obj);

        delete *egg_iter;
        egg_list.erase(egg_iter);

        break;
       }
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
    uint8 quality = (*egg)->obj->quality;
    dist_x = abs((sint16)(*egg)->obj->x - x);
    dist_y = abs((sint16)(*egg)->obj->y - y);

    if(dist_x <= 32 && dist_y <= 32 && (*egg)->obj->z == z)
      {

       if(((*egg)->obj->status & OBJ_STATUS_EGG_ACTIVE) == 0)
         {
          (*egg)->obj->status |= OBJ_STATUS_EGG_ACTIVE;

          hatch_probability = NUVIE_RAND()%100;
          PERR("Checking Egg (%x,%x,%x). Rand: %d Probability: %d%%",(*egg)->obj->x, (*egg)->obj->y, (*egg)->obj->z,hatch_probability,(*egg)->obj->qty);

          PERR(" Align: %s", get_actor_alignment_str(quality % 10));

          if(quality < 10)      PERR(" (always)");    // 0-9
          else if(quality < 20) PERR(" (day)");       // 10-19
          else if(quality < 30) PERR(" (night)");     // 20-29
          else if(quality < 40) PERR(" (day+night)"); // 30-39
          PERR("\n");
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
 uint16 qty;
 uint8 hour = Game::get_game()->get_clock()->get_hour();
 uint8 alignment = egg->quality % 10;
 
    // check time that the egg will hach
    egg_hatch_time period = EGG_HATCH_TIME(egg->quality);
    if( period==EGG_HATCH_ALWAYS
        || (period==EGG_HATCH_DAY && hour>=EGG_DAY_HOUR && hour<EGG_NIGHT_HOUR)
        || (period==EGG_HATCH_NIGHT && !(hour>=EGG_DAY_HOUR && hour<EGG_NIGHT_HOUR)) )
    {
          // check random probability that the egg will hatch
          if(egg->qty == 100 || hatch_probability <= egg->qty)  // Hatch the egg.
            {
             assert(egg->container);
             for(link = egg->container->start(); link != NULL; link = link->next)
               {
                obj = (Obj *)link->data;
                qty = obj->qty;

                if(gametype == NUVIE_GAME_U6 && obj->obj_n == OBJ_U6_SILVER_SERPENT) //U6 silver serpents only hatch once per egg.
                  qty = 1;

                for(i = 0; i < qty; i++)
                 {
				  if(obj->quality != 0) /* spawn temp actor we know it's an actor if it has a non-zero worktype. */
				  {
				  	// group new actors randomly if egg space already occupied
				  	Actor *prev_actor = actor_manager->get_actor(egg->x, egg->y, egg->z);
				  	Actor *new_actor = NULL;
				  	if(actor_manager->create_temp_actor(obj->obj_n,egg->x,egg->y,egg->z,alignment,obj->quality,&new_actor) && prev_actor)
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
					 //spawned_obj->x = egg->x+i; // line objects up in a row
					 spawned_obj->x = egg->x; // regeants all grow at the same location
					 spawned_obj->y = egg->y;
					 spawned_obj->z = egg->z;
					 spawned_obj->qty = 1; // (it already spawns qty objects with the loop)
					 spawned_obj->status |= OBJ_STATUS_TEMPORARY | OBJ_STATUS_OK_TO_TAKE;

					 obj_manager->add_obj(spawned_obj, true); // addOnTop
					}
                 }
               }
             return true;
            }
    }
 return false;
}
