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
#include "U6misc.h"
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

 last_obj_blk_x = 0;
 last_obj_blk_y = 0;
 last_obj_blk_z = OBJ_TEMP_INIT;

 update = true;

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

 // Actor status flags.
 
 objlist->seek(0x800);
 
 for(i=0;i < 256; i++)
   {
    actors[i]->status_flags = objlist->read1();
    actors[i]->alive = (bool)(actors[i]->status_flags & ACTOR_STATUS_DEAD);
    
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

 objlist->seek(0x17f1); // Start of Actor flags

 for(i=0;i < 256; i++)
   {
    actors[i]->flags = objlist->read1();
   }

 loadActorSchedules();

 for(i=0;i < 256; i++)
   {
    actors[i]->inventory_parse_readied_objects();

    actors[i]->init(); //let the actor object do some init
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

 objlist->seek(0x17f1); // Start of Actor flags

 for(i=0;i < 256; i++)
   {
    objlist->write1(actors[i]->flags);
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
 * or appearance.
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


void ActorManager::updateActors(uint16 x, uint16 y, uint8 z)
{
 uint8 cur_hour;
 uint16 i;

 if(!update || (Game::get_game()->get_pause_flags() & PAUSE_WORLD))
  return;

 update_temp_actors(x,y,z); // Remove out of range temp actors

 cur_hour = clock->get_hour();

 if(cur_hour != game_hour)
   {
    game_hour = cur_hour;

    for(i=0;i<256;i++)
      if(!actors[i]->in_party) // don't do scheduled activities while partying
        actors[i]->updateSchedule(cur_hour);
   }

 for(i=0;i<256;i++)
  actors[i]->update();

 return;
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


/* Print Actor data to stdout.
 */
void ActorManager::print_actor(Actor *actor)
{
    printf("\n");
    printf("%s at %x, %x, %x\n", look_actor(actor), actor->x, actor->y, actor->z);
    printf("id_n: %d\n", actor->id_n);

    printf("obj_n: %03d    frame_n: %d\n", actor->obj_n, actor->frame_n);
    printf("base_obj_n: %03d    old_frame_n: %d\n", actor->base_obj_n, actor->old_frame_n);

    uint8 direction = actor->direction;
    printf("direction: %d (%s)\n", direction, (direction==ACTOR_DIR_U)?"north":
                                              (direction==ACTOR_DIR_R)?"east":
                                              (direction==ACTOR_DIR_D)?"south":
                                              (direction==ACTOR_DIR_L)?"west":"???");
    printf("walk_frame: %d\n", actor->walk_frame);

    printf("can_move: %s\n", actor->can_move ? "true" : "false");
//    printf("alive: %s\n", actor->alive ? "true" : "false");
    printf("in_party: %s\n", actor->in_party ? "true" : "false");
    printf("visible_flag: %s\n", actor->visible_flag ? "true" : "false");
//    printf("met_player: %s\n", actor->met_player ? "true" : "false");

    printf("moves: %d\n", actor->moves);

    const char *wt_string = get_worktype_string(actor->worktype);
    if(!wt_string) wt_string = "???";
    printf("worktype: 0x%02x/%03d (%s)\n", actor->worktype, actor->worktype, wt_string);

    printf("NPC stats:\n");
    printf(" level: %d    exp: %d    hp: %d / %d\n", actor->level, actor->exp,
           actor->hp, actor->get_maxhp());
    printf(" strength: %d    dex: %d    int: %d\n", actor->strength, actor->dex,
           actor->intelligence);
    printf(" magic: %d\n", actor->magic);

    uint8 combat_mode = actor->combat_mode;
    printf("combat_mode: %d (%s)\n", combat_mode,
           (combat_mode == 0x02) ? "command"
           : (combat_mode == 0x03) ? "front"
           : (combat_mode == 0x04) ? "rear"
           : (combat_mode == 0x05) ? "flank"
           : (combat_mode == 0x06) ? "berserk"
           : (combat_mode == 0x07) ? "retreat"
           : (combat_mode == 0x08) ? "assault" : "???");

    printf("NPC flags: ");
    print_b(actor->flags);
    printf("\n");

    printf("Status flags: ");
    print_b(actor->status_flags);
    printf("\n");

    uint32 inv = actor->inventory_count_objects(true);
    if(inv)
    {
        printf("Inventory (+readied): %d objects\n", inv);
        U6LList *inv_list = actor->get_inventory_list();
        for(U6Link *link = inv_list->start(); link != NULL; link=link->next)
        {
            Obj *obj = (Obj *)link->data;
            printf(" %24s (%03d:%d) qual=%d qty=%d    (weighs %f)\n",
                   obj_manager->look_obj(obj), obj->obj_n, obj->frame_n, obj->quality,
                   obj->qty, obj_manager->get_obj_weight(obj, false));
        }
        printf("(weight %f / %f)\n", actor->get_inventory_weight(),
               actor->inventory_get_max_weight());
    }
    if(actor->sched && *actor->sched)
    {
        printf("Schedule:\n");
        Schedule **s = actor->sched;
        uint32 sp = 0;
        do
        {
            wt_string = get_worktype_string(s[sp]->worktype);
            if(!wt_string) wt_string = "???";
            if(sp == actor->sched_pos && s[sp]->worktype == actor->worktype)
                printf("*%d: location=0x%03x,0x%03x,0x%x  time=%02d:00  day=%d  worktype=0x%02x(%s)*\n", sp, s[sp]->x, s[sp]->y, s[sp]->z, s[sp]->hour, s[sp]->day_of_week, s[sp]->worktype, wt_string);
            else
                printf(" %d: location=0x%03x,0x%03x,0x%x  time=%02d:00  day=%d  worktype=0x%02x(%s)\n", sp, s[sp]->x, s[sp]->y, s[sp]->z, s[sp]->hour, s[sp]->day_of_week, s[sp]->worktype, wt_string);
        } while(s[++sp]);
    }

    if(!actor->surrounding_objects.empty())
        printf("Actor has multiple tiles\n");
    if(actor->pathfinder)
        printf("Actor is walking\n");
    printf("\n");
}


/* Returns name of NPC worktype/activity (game specific) or NULL.
 */
const char *ActorManager::get_worktype_string(uint32 wt)
{
    const char *wt_string = NULL;
    if(wt == WORKTYPE_U6_IN_PARTY) wt_string = "in_party";
    else if(wt == WORKTYPE_U6_ANIMAL_WANDER) wt_string = "a_wander";
    else if(wt == WORKTYPE_U6_WALK_TO_LOCATION) wt_string = "walkto";
    else if(wt == WORKTYPE_U6_FACE_NORTH) wt_string = "face_n";
    else if(wt == WORKTYPE_U6_FACE_SOUTH) wt_string = "face_s";
    else if(wt == WORKTYPE_U6_FACE_EAST) wt_string = "face_e";
    else if(wt == WORKTYPE_U6_FACE_WEST) wt_string = "face_w";
    else if(wt == WORKTYPE_U6_WALK_NORTH_SOUTH) wt_string = "v_walk";
    else if(wt == WORKTYPE_U6_WALK_EAST_WEST) wt_string = "h_walk";
    else if(wt == WORKTYPE_U6_WANDER_AROUND) wt_string = "wander";
    else if(wt == WORKTYPE_U6_WORK) wt_string = "work_move";
    else if(wt == WORKTYPE_U6_SLEEP) wt_string = "sleep";
    else if(wt == WORKTYPE_U6_PLAY_LUTE) wt_string = "play_lute";
    else if(wt == WORKTYPE_U6_BEG) wt_string = "beg";
    else if(wt == 0x02) wt_string = "player";
    else if(wt >= 0x03 && wt <= 0x08) wt_string = "combat";
    else if(wt == 0x8e) wt_string = "waiter?";
    else if(wt == 0x92) wt_string = "work_still";
    else if(wt == 0x93) wt_string = "eat";
    else if(wt == 0x94) wt_string = "farmer";
    else if(wt == 0x98) wt_string = "bell";
    else if(wt == 0x99) wt_string = "spar";
    else if(wt == 0x9a) wt_string = "mousing";
    return(wt_string);
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
   
   actor->set_direction(ACTOR_DIR_U);

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

bool ActorManager::create_temp_actor(uint16 obj_n, uint16 x, uint16 y, uint8 z, uint8 worktype, Actor **new_actor)
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

//FIX? should this be in Player??

void ActorManager::update_temp_actors(uint16 x, uint16 y, uint8 z)
{
 uint16 cur_blk_x, cur_blk_y;

 // We're changing levels so clean out all temp actors on the current level.
 if(last_obj_blk_z != z)
   {
    if(last_obj_blk_z != ACTOR_TEMP_INIT) //don't clean actors on startup.
      clean_temp_actors_from_level(last_obj_blk_z);

    last_obj_blk_z = z;

    return;
   }

 cur_blk_x = x >> 5; // x / 32;
 cur_blk_y = y >> 5; // y / 32;

 if(cur_blk_x != last_obj_blk_x || cur_blk_y != last_obj_blk_y)
   {
    last_obj_blk_x = cur_blk_x;
    last_obj_blk_y = cur_blk_y;

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
    uint32 toss_max = MAX(xrange, yrange) * MIN(xrange, yrange) * 2;
    LineTestResult lt;
    if(toss_max == 0)
        return(false);
    if(xrange > 0) --xrange; // range includes the starting location
    if(yrange > 0) --yrange;
    while(toss_max--)
    {
        sint16 x = (actor->x-xrange) + (NUVIE_RAND() % ((actor->x+xrange) - (actor->x-xrange) + 1)),
               y = (actor->y-yrange) + (NUVIE_RAND() % ((actor->y+yrange) - (actor->y-yrange) + 1));
        if(!map->lineTest(actor->x, actor->y, x, y, actor->z, LT_HitUnpassable, lt))
            if(!get_actor(x, y, actor->z))
                return(actor->move(x, y, actor->z));
    }
    return(false);
}


/* Returns the actor whose inventory contains an object.
 */
Actor *ActorManager::get_actor_holding_obj(Obj *obj)
{
    assert(obj->is_in_inventory());
    return(get_actor(obj->x));
}
