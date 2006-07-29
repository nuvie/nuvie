/*
 *  ActorManager.cpp
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
#include <cassert>
#include "nuvieDefs.h"
#include "Configuration.h"

#include "Actor.h"
#include "U6Actor.h"
#include "U6WorkTypes.h"
#include "TileManager.h"
#include "U6LList.h"
#include "ActorManager.h"
#include "NuvieIOFile.h"
#include "GameClock.h"
#include "Game.h"
#include "Party.h"

#define TEMP_ACTOR_OFFSET 224
#define ACTOR_TEMP_INIT 255
#define SCHEDULE_SIZE 5

void config_get_path(Configuration *config, std::string filename, std::string &path);

ActorManager::ActorManager(Configuration *cfg, Map *m, TileManager *tm, ObjManager *om, GameClock *c)
{
 uint16 i;

 config = cfg;
 map = m;
 tile_manager = tm;
 obj_manager = om;
 clock = c;

 for(i = 0; i < 256; i++)
   actors[i] = NULL;

 init();
}

ActorManager::~ActorManager()
{
 clean();
}

void ActorManager::init()
{
 game_hour = 0;

 player_actor = 1;

 last_obj_blk_x = cur_x = 0;
 last_obj_blk_y = cur_y = 0;
 last_obj_blk_z = cur_z = OBJ_TEMP_INIT;
 cmp_actor_loc = 0;

 update = true;
 wait_for_player = true;
 combat_movement = false;

 return;
}

void ActorManager::clean()
{
 uint16 i;

 //delete all actors
 for(i = 0;i < 256;i++)
  {
   if(actors[i])
     {
      delete actors[i];
      actors[i] = NULL;
     }
  }

 init();

 return;
}

bool ActorManager::load(NuvieIO *objlist)
{
 uint16 i;
 uint8 b1, b2, b3;
 int game_type;

 clean();

 config->value("config/GameType",game_type);

 objlist->seek(0x100); // Start of Actor position info

 for(i=0; i < 256; i++)
   {
    switch(game_type)
     {
      case NUVIE_GAME_U6 : actors[i] = new U6Actor(map,obj_manager,clock); break;
      case NUVIE_GAME_MD : actors[i] = new Actor(map,obj_manager,clock); break;
      case NUVIE_GAME_SE : actors[i] = new Actor(map,obj_manager,clock); break;
     }

    b1 = objlist->read1();
    b2 = objlist->read1();
    b3 = objlist->read1();

    actors[i]->x = b1;
    actors[i]->x += (b2 & 0x3) << 8;

    actors[i]->y = (b2 & 0xfc) >> 2;
    actors[i]->y += (b3 & 0xf) << 6;

    actors[i]->z = (b3 & 0xf0) >> 4;
    actors[i]->id_n = (uint8)i;
   }

// objlist.seek(0x15f1);

 for(i=0;i < 256; i++)
   {
    b1 = objlist->read1();
    b2 = objlist->read1();
    actors[i]->obj_n = b1;
    actors[i]->obj_n += (b2 & 0x3) << 8;

    actors[i]->frame_n = (b2 & 0xfc) >> 2;
    actors[i]->direction = actors[i]->frame_n / 4;
   }

 // Object flags.

 objlist->seek(0x000);

 for(i=0; i < 256; i++)
   {
    actors[i]->obj_flags = objlist->read1();
   }
 
 // Actor status flags.
 
 objlist->seek(0x800);
 
 for(i=0;i < 256; i++)
   {
    actors[i]->status_flags = objlist->read1();
    actors[i]->alive = (bool)!(actors[i]->status_flags & ACTOR_STATUS_DEAD);
    actors[i]->alignment = ((actors[i]->status_flags & ACTOR_STATUS_ALIGNMENT_MASK) >> 5) + 1;
   }

 //old obj_n & frame_n values
 
 objlist->seek(0x15f1);

 for(i=0;i < 256; i++)
   {
    b1 = objlist->read1();
    b2 = objlist->read1();
    actors[i]->base_obj_n = b1;
    actors[i]->base_obj_n += (b2 & 0x3) << 8;

    actors[i]->old_frame_n = (b2 & 0xfc) >> 2;

    if(actors[i]->obj_n == 0)
     {
      //actors[i]->obj_n = actors[i]->base_obj_n;
      //actors[i]->frame_n = actors[i]->old_frame_n;
      actors[i]->hide();
     }

    if(actors[i]->base_obj_n == 0)
     {
      actors[i]->base_obj_n = actors[i]->obj_n;
      actors[i]->old_frame_n = actors[i]->frame_n;
     }
   }
 // Strength

 objlist->seek(0x900);

 for(i=0;i < 256; i++)
   {
    actors[i]->strength = objlist->read1();
   }

 // Dexterity

 objlist->seek(0xa00);

 for(i=0;i < 256; i++)
   {
    actors[i]->dex = objlist->read1();
   }

 // Intelligence

 objlist->seek(0xb00);

 for(i=0;i < 256; i++)
   {
    actors[i]->intelligence = objlist->read1();
   }

  // Experience

 objlist->seek(0xc00);

 for(i=0;i < 256; i++)
   {
    actors[i]->exp = objlist->read2();
   }

 // Health

 objlist->seek(0xe00);

 for(i=0;i < 256; i++)
   {
    actors[i]->hp = objlist->read1();
   }

 // Experience Level

 objlist->seek(0xff1);

 for(i=0;i < 256; i++)
   {
    actors[i]->level = objlist->read1();
   }


 // Combat mode

 objlist->seek(0x12f1);

 for(i=0;i < 256; i++)
   {
    actors[i]->combat_mode = objlist->read1();
   }

 // Magic Points

 objlist->seek(0x13f1);

 for(i=0;i < 256; i++)
   {
    actors[i]->magic = objlist->read1();
   }

 objlist->seek(0x17f1); // Start of Talk flags

 for(i=0;i < 256; i++)
   {
    actors[i]->talk_flags = objlist->read1();
   }

 loadActorSchedules();

 for(i=0;i < 256; i++)
   {
    actors[i]->inventory_parse_readied_objects();

    actors[i]->init(); //let the actor object do some init
   }

 // Moves

 objlist->seek(0x14f1);

 for(i=0;i < 256; i++)
   {
    actors[i]->moves = objlist->read1();
   }

 // Current Worktype

 objlist->seek(0x11f1);

 for(i=0;i < 256; i++)
   {
    actors[i]->set_worktype(objlist->read1());
   }

 return true;
}

bool ActorManager::save(NuvieIO *objlist)
{
 uint16 i;
 uint8 b;
 int game_type;

 config->value("config/GameType",game_type);

 objlist->seek(0x100); // Start of Actor position info

 for(i=0; i < 256; i++)
   {
    objlist->write1(actors[i]->x & 0xff);

    b = actors[i]->x >> 8;
    b += actors[i]->y << 2;
    objlist->write1(b);

    b = actors[i]->y >> 6;
    b += actors[i]->z << 4;
    objlist->write1(b);
   }

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->obj_n & 0xff);
    b = actors[i]->obj_n >> 8;
    b += actors[i]->frame_n << 2;
    objlist->write1(b);
   }

 //old obj_n & frame_n values

 objlist->seek(0x15f1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->base_obj_n & 0xff);
    b = actors[i]->base_obj_n >> 8;
    b += actors[i]->old_frame_n << 2;
    objlist->write1(b);
   }

 // Strength

 objlist->seek(0x900);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->strength);
   }

 // Dexterity

 objlist->seek(0xa00);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->dex);
   }

 // Intelligence

 objlist->seek(0xb00);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->intelligence);
   }

  // Experience

 objlist->seek(0xc00);

 for(i=0;i < 256; i++)
   {
    objlist->write2(actors[i]->exp);
   }

 // Health

 objlist->seek(0xe00);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->hp);
   }

 // Experience Level

 objlist->seek(0xff1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->level);
   }


 // Combat mode

 objlist->seek(0x12f1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->combat_mode);
   }

 // Magic Points

 objlist->seek(0x13f1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->magic);
   }

 // Moves

 objlist->seek(0x14f1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->moves);
   }

 objlist->seek(0); // Start of Obj flags

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->obj_flags);
   }

 objlist->seek(0x800); // Start of Status flags
 
 for(i=0;i < 256; i++)
   {
    actors[i]->status_flags |= (actors[i]->alignment-1)<<5;
    objlist->write1(actors[i]->status_flags);
   }

 objlist->seek(0x17f1); // Start of Talk flags

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->talk_flags);
   }

/*
 for(i=0;i < 256; i++)
   {
    actors[i]->inventory_parse_readied_objects();

    actors[i]->init(); //let the actor object do some init
   }
*/
 // Current Worktype

 objlist->seek(0x11f1);

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->get_worktype());
   }

 return true;
}

ActorList *ActorManager::get_actor_list()
{
    ActorList *_actors = new ActorList(256);
    for(uint16 i=0;i<256;i++)
        (*_actors)[i] = actors[i];
    return _actors;
}

ActorList *ActorManager::get_active_actors()
{
    if(active_actors.empty())
        update_active_actors(cur_x, cur_y, cur_z);
    ActorList *_actors = new ActorList(active_actors);
    return _actors;
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

/* Returns an actor's "look-string," a general description of their occupation
 * or appearance. (the tile description)
 */
const char *ActorManager::look_actor(Actor *a, bool show_prefix)
{
    uint16 tile_num = obj_manager->get_obj_tile_num(a->get_tile_num());
    if(tile_num == 0)
      {
       uint8 actor_num = a->id_n;
       if(actor_num == 188) // U6: Statue of Exodus
         return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(399), 0, show_prefix);
       else if(actor_num == 189) // Statue of Mondain
         return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(397), 0, show_prefix);
       else if(actor_num == 190) // Statue of Minax
         return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(398), 0, show_prefix);
       else if(actor_num >= 191 && actor_num <= 198) // shrines
         return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(393), 0, show_prefix);
       else if(actor_num == 199) // Altar of singularity
         return tile_manager->lookAtTile(obj_manager->get_obj_tile_num(329), 0, show_prefix);
      }
    return tile_manager->lookAtTile(tile_num,0,show_prefix);
}

// Update area, and spawn or remove actors.
void ActorManager::updateActors(uint16 x, uint16 y, uint8 z)
{
// uint8 cur_hour;
// uint16 i;

 if(!update)
  return;
//printf("updateActors()\n");

 cur_x = x; cur_y = y; cur_z = z;

 uint16 cur_blk_x = x >> 5; // x / 32;
 uint16 cur_blk_y = y >> 5; // y / 32;

 update_temp_actors(x,y,z); // Remove out of range temp actors

 // Add new actors to active list, Remove out of range actors
 if(active_actors.empty() || cur_blk_x != last_obj_blk_x || cur_blk_y != last_obj_blk_y || z != last_obj_blk_z)
    update_active_actors(cur_x,cur_y,cur_z);
 // The active actors list will remain empty if nobody can move, and time will
 // be added in moveActors().

 last_obj_blk_x = cur_blk_x; // moved from update_temp_actors() (SB-X)
 last_obj_blk_y = cur_blk_y;
 last_obj_blk_z = z;

/*// moved to updateTime() (SB-X)
 cur_hour = clock->get_hour();

 if(cur_hour != game_hour)
   {
    game_hour = cur_hour;

    for(i=0;i<256;i++)
      if(!actors[i]->in_party) // don't do scheduled activities while partying
        actors[i]->updateSchedule(cur_hour);
   }*/

 return;
}

// After player/party moves, continue moving actors.
void ActorManager::startActors()
{
//printf("startActors()\n");
    // add player after they move (if they have any moves left)
    if(combat_movement == true)
        activate_actor(active_actors.begin(), actors[player_actor]);

    wait_for_player = false;
    Game::get_game()->pause_user();
}

// After all actors move, refresh move counts and add time.
void ActorManager::updateTime()
{
    if(!update)
        return;

//printf("updateTime(): ");
    for(int i=0; i<256; i++)
        actors[i]->update_time(); // **UPDATE MOVES LEFT**
    clock->inc_minute(); // **UPDATE TIME**
//printf("%d:%02d\n",clock->get_hour(),clock->get_minute());
    uint8 cur_hour = clock->get_hour();
    if(cur_hour != game_hour) // moved from updateActors() (SB-X)
    {
        game_hour = cur_hour;

        for(int i=0;i<256;i++)
            if(!actors[i]->in_party) // don't do scheduled activities while partying
                actors[i]->updateSchedule(cur_hour);
    }

}

// Return control to player.
void ActorManager::stopActors()
{
//printf("stopActors()\n\n\n\n\n");
    Game::get_game()->unpause_user();
    wait_for_player = true;
}

void ActorManager::twitchActors()
{
 uint16 i;

 // while Actors are part of the world, their twitching is considered animation
 if(Game::get_game()->anims_paused())
  return;

 for(i=0;i<256;i++)
  actors[i]->twitch();

}

// Update actors. StopActors() if no one can move.
void ActorManager::moveActors()
{
    if(!update)
        return;// nothing to do

    while(!wait_for_player)
    {
        if(can_party_move())
            stopActors(); // wait_for_player=true
        else
        {
            if(!active_actors.empty())
            {
                Actor *actor = active_actors.front();
                if(!update_actor(actor)) // actor==player: wait_for_player=true
                    break;

                // resort by moves after update
                deactivate_actor(actor);
                if(actor->id_n != player_actor)
                    activate_actor(active_actors.begin(), actor);
            // We don't re-add the player because they didn't update, or their update
            // doesn't use up any moves, and only needs to be called once per turn.
            // If stopActors() was called, the player will be re-added after moving.
            }
            else // just ran out of actors, or never found any
            {
                updateTime(); // refresh moves
                update_active_actors(cur_x,cur_y,cur_z);
                if(active_actors.empty())
                {
                    // make sure display updates when player isn't moving
                    Game::get_game()->time_changed();
                    break; // break instead of continue, so main loop can update world
                }
            }
        }
    }
}

inline void ActorManager::deactivate_actor(Actor *actor)
{
    ActorIterator a = active_actors.begin();
    while(a != active_actors.end())
        if((*a)->id_n == actor->id_n)
        {
            active_actors.erase(a);
            return; // assume the actor is only listed once
        }
        else ++a;
}

// Update actor. Return false if actor can't move yet.
inline bool ActorManager::update_actor(Actor *actor)
{
    sint8 moves_pre_update = actor->moves;
    if(actor->id_n != player_actor)
        if(actor->get_location().is_visible()
           && (clock->get_ticks()-actor->move_time) < (combat_movement?250:66)) // FIXME: Replace with animation.
            return false; // Don't move again so soon, and block others.
//printf("update_actor(%d) %d moves",actor->id_n,actor->moves);
    actor->update(); // *UPDATE*
    if(actor->id_n == player_actor)
    {
//printf(" -> player\n");
        stopActors(); // Player's turn
    }
    else if(actor->moves == moves_pre_update && actor->moves > 0)
        actor->set_moves_left(0); // Pass - use up moves (prevents endless loop)
//else printf(" -> %d moves left\n",actor->moves);
    return true;
}

inline ActorIterator ActorManager::activate_actor(const ActorIterator &start_at, Actor *actor)
{
    ActorIterator a = start_at;
    if(actor->moves > 0)
    {
        struct Actor::cmp_move_fraction cmpfunc; // comparison function object
        while(a != active_actors.end() && cmpfunc(*a, actor)) ++a;
        a = active_actors.insert(a, actor);
    }
    return a;
}

/* Returns true if the party can move before the next actor in active_actors.
 * (switching to player in combat is handled after actor update) */
inline bool ActorManager::can_party_move()
{
    Party *party = Game::get_game()->get_party();
    Actor *pActor = party->get_slowest_actor();
    sint8 party_moves_left = pActor ? pActor->get_moves_left() : 0;

    if(combat_movement || party_moves_left <= 0)
        return false;
    if(active_actors.empty())
        return true;

    Actor *actor = active_actors.front();
    // pM/pD > M/D
    if((actor->in_party
        || party_moves_left*actor->dex > actor->moves*pActor->get_dexterity()))
        return true; // Player's turn
    return false;
}

// Sort actors by order of movement.
void ActorManager::update_active_actors(uint16 x, uint16 y, uint8 z)
{
//printf("update_active_actors(): ");
    ActorList *new_active_actors = get_actor_list(); // sorted by actor number
    filter_active_actors(new_active_actors, x,y,z);
//printf("%d can move\n",new_active_actors->size());
    stable_sort(new_active_actors->begin(), new_active_actors->end(),
                Actor::cmp_move_fraction()); // sorted by movement order

    active_actors = *new_active_actors;
    delete new_active_actors;
}

bool ActorManager::loadActorSchedules()
{
 std::string filename;
 NuvieIOFileRead schedule;
 uint16 i;
 uint16 total_schedules;
 uint16 num_schedules[256]; // an array to hold the number of schedule entries for each Actor.
 uint32 bytes_read;
 unsigned char *sched_data;
 uint16 *sched_offsets;
 unsigned char *s_ptr;

 config_get_path(config,"schedule",filename);
 if(schedule.open(filename) == false)
   return false;

 sched_offsets = (uint16 *)malloc(256*sizeof(uint16));

 for (i = 0; i < 256; i++)
  sched_offsets[i] = schedule.read2();

 total_schedules = schedule.read2();

 for (i = 0; i < 256; i++)
 {
    //if (sched_offsets[i] == 0)
    //    num_schedules[i] = 0;
    //else
    if (sched_offsets[i] > (total_schedules-1))
        num_schedules[i] = 0;
    else
    // sched_offsets[i] is valid
    {
        if (i == 255)
            num_schedules[i] = total_schedules - sched_offsets[i];
        else if (sched_offsets[i+1] > (total_schedules-1))
            num_schedules[i] = total_schedules - sched_offsets[i];
        else
            // sched_offsets[i+1] is valid
            num_schedules[i] = sched_offsets[i+1] - sched_offsets[i];
    }
 }

 sched_data = schedule.readBuf(total_schedules * SCHEDULE_SIZE, &bytes_read);

 if(bytes_read != (uint32)(total_schedules * SCHEDULE_SIZE))
   {
    printf("Error: Reading schedules!\n");
    return false;
   }

 for(i=0;i<256;i++)
  {
   s_ptr = sched_data + (sched_offsets[i] * SCHEDULE_SIZE);
   actors[i]->loadSchedule(s_ptr,num_schedules[i]);
  }

 free(sched_data);
 free(sched_offsets);

 return true;
}

void ActorManager::clear_actor(Actor *actor)
{
 if(is_temp_actor(actor))
   clean_temp_actor(actor);
 else
   actor->clear();

 return;
}

bool ActorManager::resurrect_actor(Obj *actor_obj, MapCoord new_position)
{
 Actor *actor;
 U6Link *link;
 
 if(!is_temp_actor(actor_obj->quality))
  {
   actor = get_actor(actor_obj->quality);
   actor->alive = true;
   actor->status_flags = actor->status_flags ^ ACTOR_STATUS_DEAD;
   
   actor->show();

   actor->x = new_position.x;
   actor->y = new_position.y;
   actor->z = new_position.z;
   actor->obj_n = actor->base_obj_n;
   actor->init();
   
   actor->frame_n = 0;
   
   actor->set_direction(NUVIE_DIR_N);

   actor->set_hp(1);
   //actor->set_worktype(0x1);
   
   if((actor->status_flags & ACTOR_STATUS_IN_PARTY) == ACTOR_STATUS_IN_PARTY) //actor in party
     Game::get_game()->get_party()->add_actor(actor);
   
   //add body container objects back into actor's inventory.
   if(actor_obj->container)
     {
      for(link = actor_obj->container->start(); link != NULL; link = link->next)
        actor->inventory_add_object((Obj *)link->data);

      actor_obj->container->removeAll();
     }

  }

 return true;
}

bool ActorManager::is_temp_actor(Actor *actor)
{
 if(actor)
  return is_temp_actor(actor->id_n);

 return false;
}

bool ActorManager::is_temp_actor(uint8 id_n)
{
 if(id_n >= TEMP_ACTOR_OFFSET)
  return true;

 return false;
}

bool ActorManager::create_temp_actor(uint16 obj_n, uint16 x, uint16 y, uint8 z, uint8 alignment, uint8 worktype, Actor **new_actor)
{
 Actor *actor;

 actor = find_free_temp_actor();

 if(actor)
  {
   actor->base_obj_n = obj_n;
   actor->obj_n = obj_n;
   actor->frame_n = 0;

   actor->x = x;
   actor->y = y;
   actor->z = z;
   
   actor->temp_actor = true;

   actor->init();

   if(alignment != ACTOR_ALIGNMENT_DEFAULT)
    actor->set_alignment(alignment);

   // spawn double-tiled actors, like cows, facing west (SB-X)
   if(actor->get_tile_type() == ACTOR_DT)
    actor->set_direction(-1, 0);
   actor->set_worktype(worktype);
   actor->show();

   printf("Adding Temp Actor #%d: %s (%x,%x,%x).\n", actor->id_n,tile_manager->lookAtTile(obj_manager->get_obj_tile_num(actor->obj_n)+actor->frame_n,0,false),actor->x,actor->y,actor->z);

   if(new_actor)
    *new_actor = actor;
   return true;
  }
 else
  printf("***All Temp Actor Slots Full***\n");

 if(new_actor)
  *new_actor = NULL;
 return false;
}

inline Actor *ActorManager::find_free_temp_actor()
{
 uint16 i;

 for(i = TEMP_ACTOR_OFFSET;i<256;i++)
  {
   if(actors[i]->obj_n == 0)
     return actors[i];
  }

 return NULL;
}

//FIX? should this be in Player??
// NO!
void ActorManager::update_temp_actors(uint16 x, uint16 y, uint8 z)
{
 uint16 cur_blk_x, cur_blk_y;

 // We're changing levels so clean out all temp actors on the current level.
 if(last_obj_blk_z != z)
   {
    if(last_obj_blk_z != ACTOR_TEMP_INIT) //don't clean actors on startup.
      clean_temp_actors_from_level(last_obj_blk_z);

//    last_obj_blk_z = z;

    return;
   }

 cur_blk_x = x >> 5; // x / 32;
 cur_blk_y = y >> 5; // y / 32;

 if(cur_blk_x != last_obj_blk_x || cur_blk_y != last_obj_blk_y)
   {
//    last_obj_blk_x = cur_blk_x;
//    last_obj_blk_y = cur_blk_y;

    clean_temp_actors_from_area(x,y);
   }

 return;
}

void ActorManager::clean_temp_actors_from_level(uint8 level)
{
 uint16 i;

 for(i=TEMP_ACTOR_OFFSET;i<256;i++)
   {
    if(actors[i]->is_visible() && actors[i]->z == level)
      clean_temp_actor(actors[i]);
   }

 return;
}

void ActorManager::clean_temp_actors_from_area(uint16 x, uint16 y)
{
 uint16 i;
 uint16 dist_x, dist_y;

 for(i=TEMP_ACTOR_OFFSET;i<256;i++)
   {
    if(actors[i]->is_visible())
      {
       dist_x = abs((sint16)actors[i]->x - x);
       dist_y = abs((sint16)actors[i]->y - y);

       if(dist_x > 32 || dist_y > 32)
         {
          clean_temp_actor(actors[i]);
         }
      }
   }

 return;
}

inline void ActorManager::clean_temp_actor(Actor *actor)
{
 printf("Removing Temp Actor #%d: %s (%x,%x,%x).\n", actor->id_n,tile_manager->lookAtTile(obj_manager->get_obj_tile_num(actor->obj_n)+actor->frame_n,0,false),actor->x,actor->y,actor->z);
 actor->obj_n = 0;
 actor->clear();

 return;
}


/* Move an actor to a random location within range.
 * Returns true when tossed.
 */
bool ActorManager::toss_actor(Actor *actor, uint16 xrange, uint16 yrange)
{
    // maximum number of tries
    const uint32 toss_max = MAX(xrange, yrange) * MIN(xrange, yrange) * 2;
    uint32 t = 0;
    LineTestResult lt;
    if(xrange > 0) --xrange; // range includes the starting location
    if(yrange > 0) --yrange;
    while(t++ < toss_max) // TRY RANDOM LOCATION
    {
        sint16 x = (actor->x-xrange) + (NUVIE_RAND() % ((actor->x+xrange) - (actor->x-xrange) + 1)),
               y = (actor->y-yrange) + (NUVIE_RAND() % ((actor->y+yrange) - (actor->y-yrange) + 1));
        if(!map->lineTest(actor->x, actor->y, x, y, actor->z, LT_HitUnpassable, lt))
            if(!get_actor(x, y, actor->z))
                return(actor->move(x, y, actor->z));
    }
    // TRY ANY LOCATION
    for(int y = actor->y-yrange; y < actor->y+yrange; y++)
        for(int x = actor->x-xrange; x < actor->x+xrange; x++)
            if(!map->lineTest(actor->x, actor->y, x, y, actor->z, LT_HitUnpassable, lt))
                if(!get_actor(x, y, actor->z))
                    return(actor->move(x, y, actor->z));
    return(false);
}


/* Returns the actor whose inventory contains an object.
 */
Actor *ActorManager::get_actor_holding_obj(Obj *obj)
{
    assert(obj->is_in_inventory());
    return(get_actor(obj->x));
}

// Remove list actors who fall out of a certain range from a location.
ActorList *ActorManager::filter_distance(ActorList *list, uint16 x, uint16 y, uint8 z, uint16 dist)
{
    ActorIterator i=list->begin();
    while(i != list->end())
    {
        Actor *actor = *i;
        MapCoord loc(x, y, z);
        MapCoord actor_loc(actor->x, actor->y, actor->z);
        if(loc.distance(actor_loc) > dist || loc.z != actor_loc.z)
            i = list->erase(i);
        else ++i;
    }
    return list;
}

// Remove actors who don't need to move in this turn. That includes anyone not
// in a certain range of xyz, and actors that are already out of moves.
inline ActorList *ActorManager::filter_active_actors(ActorList *list, uint16 x, uint16 y, uint8 z)
{
    const uint8 dist = 24;
    ActorIterator i=list->begin();
    while(i != list->end())
    {
        Actor *actor = *i;
        MapCoord loc(x, y, z);
        MapCoord actor_loc(actor->x, actor->y, actor->z);
        if(!actor->in_party)
        {
            if((loc.distance(actor_loc) > dist || loc.z != actor_loc.z)
               && actor->worktype != WORKTYPE_U6_WALK_TO_LOCATION)
                actor->set_moves_left(0);
            if(actor->is_sleeping() || actor->is_immobile() || !actor->alive)
                actor->set_moves_left(0);
        }
        if((actor->in_party == true && combat_movement == false) || actor->moves <= 0)
            i = list->erase(i);
        else ++i;
    }
    return list;
}

// Sort list by distance to a location. Remove actors on different planes.
ActorList *ActorManager::sort_nearest(ActorList *list, uint16 x, uint16 y, uint8 z)
{
    struct Actor::cmp_distance_to_loc cmp_func; // comparison function object
    MapCoord loc(x, y, z);
    cmp_func(loc); // set location in function object
    sort(list->begin(), list->end(), cmp_func);

    ActorIterator a = list->begin();
    while(a != list->end())
        if((*a)->z != z)
            a = list->erase(a); // only return actors on the same map
        else ++a;
    return list;
}

void ActorManager::print_actor(Actor *actor)
{
    actor->print();
}

// Remove actors with a certain alignment from the list. Returns the same list.
ActorList *ActorManager::filter_alignment(ActorList *list, uint8 align)
{
    ActorIterator i=list->begin();
    while(i != list->end())
    {
        Actor *actor = *i;
        if(actor->alignment == align)
            i = list->erase(i);
        else ++i;
    }
    return list;
}

// Remove actors in the party. Returns the original list pointer.
ActorList *ActorManager::filter_party(ActorList *list)
{
    ActorIterator i=list->begin();
    while(i != list->end())
    {
        Actor *actor = *i;
        if(actor->in_party == true || actor->id_n == 0) // also remove vehicle
            i = list->erase(i);
        else ++i;
    }
    return list;
}

void ActorManager::set_combat_movement(bool c)
{
    combat_movement = c;
    update_active_actors(cur_x,cur_y,cur_z);
}
