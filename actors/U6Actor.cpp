/*
 *  U6Actor.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jul 27 2003.
 *  Copyright (c) Nuvie Team 2003. All rights reserved.
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

#include "nuvieDefs.h"

#include "Game.h"
#include "U6UseCode.h"
#include "PathFinder.h"
#include "MsgScroll.h"
#include "U6Actor.h"

#include "Party.h"
#include "ActorManager.h"
#include "ViewManager.h"
#include "Converse.h"
#include "Effect.h"

#include "U6ActorTypes.h"
#include "U6WorkTypes.h"

//static const uint8 sleep_objects[8];

//static uint8 walk_frame_tbl[4] = {0,1,2,1}; //FIX


U6Actor::U6Actor(Map *m, ObjManager *om, GameClock *c): Actor(m,om,c)
{
 beg_mode = 0; // beggers are waiting for targets
 walk_frame_inc = 1;
 poison_counter = 0;
}

U6Actor::~U6Actor()
{
}

bool U6Actor::init()
{
 set_actor_obj_n(obj_n); //set actor_type

 base_actor_type = get_actor_type(base_obj_n);
 
 body_armor_class = base_actor_type->body_armor_class;
 
 if(temp_actor)
   {
    strength = base_actor_type->str;
    dex = base_actor_type->dex;
    intelligence = base_actor_type->intelligence;
    hp = base_actor_type->hp;
   }
   
 if(actor_type->tile_type == ACTOR_QT && frame_n == 0) //set the two quad tile actors to correct frame number.
   frame_n = 3;

 discover_direction();

 if(has_surrounding_objs())
   clear_surrounding_objs_list(); //clean up the old list if required.

 switch(obj_n) //gather surrounding objects from map if required
  {
   case OBJ_U6_SHIP : init_ship(); break;

   case OBJ_U6_HYDRA : init_hydra(); break;

   case OBJ_U6_DRAGON : init_dragon(); break;

   case OBJ_U6_GIANT_SCORPION :
   case OBJ_U6_GIANT_ANT :
   case OBJ_U6_COW :
   case OBJ_U6_ALLIGATOR :
   case OBJ_U6_HORSE :
   case OBJ_U6_HORSE_WITH_RIDER : init_splitactor(); break;


   default : break;
  }


 if(actor_type->can_sit) // For some reason U6 starts with actors standing on their chairs.
   {                     // We need to sit them down.
    Obj *obj = obj_manager->get_obj(x,y,z);
    sit_on_chair(obj); // attempt to sit on obj.
   }

 return true;
}

bool U6Actor::init_ship()
{
 Obj *obj;
 uint16 obj1_x, obj1_y, obj2_x, obj2_y;

 obj1_x = x;
 obj1_y = y;
 obj2_x = x;
 obj2_y = y;

 switch(direction)
  {
   case NUVIE_DIR_N : obj1_y = y+1;
                      obj2_y = y-1;
                      break;
   case NUVIE_DIR_E : obj1_x = x+1;
                      obj2_x = x-1;
                      break;
   case NUVIE_DIR_S : obj1_y = y-1;
                      obj2_y = y+1;
                      break;
   case NUVIE_DIR_W : obj1_x = x-1;
                      obj2_x = x+1;
                      break;
  }

 obj = obj_manager->get_obj(obj1_x,obj1_y,z);
 if(obj == NULL)
   return false;
 surrounding_objects.push_back(obj);

 obj = obj_manager->get_obj(obj2_x,obj2_y,z);
 if(obj == NULL)
   return false;
 surrounding_objects.push_back(obj);

 return true;
}

bool U6Actor::init_splitactor()
{
 uint16 obj_x, obj_y;

 obj_x = x;
 obj_y = y;

 switch(direction) //FIX for world wrapping
  {
   case NUVIE_DIR_N : obj_y = y+1;
                      break;
   case NUVIE_DIR_E : obj_x = x-1;
                      break;
   case NUVIE_DIR_S : obj_y = y-1;
                      break;
   case NUVIE_DIR_W : obj_x = x+1;
                      break;
  }

 init_surrounding_obj(obj_x, obj_y, z, obj_n, frame_n + 8); // init back object

 return true;
}

bool U6Actor::init_dragon()
{
 uint16 head_x, head_y, tail_x, tail_y;
 uint16 wing1_x, wing1_y, wing2_x, wing2_y;

 head_x = tail_x = x;
 wing1_x = wing2_x = x;
 head_y = tail_y = y;
 wing1_y = wing2_y = y;

 switch(direction)
  {
   case NUVIE_DIR_N : head_y = y-1;
                      tail_y = y+1;
                      wing1_x = x-1;
                      wing2_x = x+1;
                      break;
   case NUVIE_DIR_E : head_x = x+1;
                      tail_x = x-1;
                      wing1_y = y-1;
                      wing2_y = y+1;
                      break;
   case NUVIE_DIR_S : head_y = y+1;
                      tail_y = y-1;
                      wing1_x = x+1;
                      wing2_x = x-1;
                      break;
   case NUVIE_DIR_W : head_x = x-1;
                      tail_x = x+1;
                      wing1_y = y+1;
                      wing2_y = y-1;
                      break;
  }

 init_surrounding_obj(head_x, head_y, z, obj_n, frame_n + 8);
 init_surrounding_obj(tail_x, tail_y, z, obj_n, frame_n + 16);
 init_surrounding_obj(wing1_x, wing1_y, z, obj_n, frame_n + 24);
 init_surrounding_obj(wing2_x, wing2_y, z, obj_n, frame_n + 32);

 return true;
}

bool U6Actor::init_hydra()
{
 // For some reason a Hydra has a different object number for its tenticles. :-(

 init_surrounding_obj(x,   y-1, z, OBJ_U6_HYDRA_BODY, 0);
 init_surrounding_obj(x+1, y-1, z, OBJ_U6_HYDRA_BODY, 4);
 init_surrounding_obj(x+1, y, z, OBJ_U6_HYDRA_BODY, 8);
 init_surrounding_obj(x+1, y+1, z, OBJ_U6_HYDRA_BODY, 12);
 init_surrounding_obj(x,   y+1, z, OBJ_U6_HYDRA_BODY, 16);
 init_surrounding_obj(x-1, y+1, z, OBJ_U6_HYDRA_BODY, 20);
 init_surrounding_obj(x-1, y, z, OBJ_U6_HYDRA_BODY, 24);
 init_surrounding_obj(x-1, y-1, z, OBJ_U6_HYDRA_BODY, 28);

 return true;
}

uint16 U6Actor::get_downward_facing_tile_num()
{
 uint8 shift = 0;

 if(base_actor_type->frames_per_direction > 1) //we want the second frame for most actor types.
   shift = 1;

 return obj_manager->get_obj_tile_num(base_actor_type->base_obj_n) + base_actor_type->tile_start_offset + (NUVIE_DIR_S * base_actor_type->tiles_per_direction + base_actor_type->tiles_per_frame - 1) + shift;
}

void U6Actor::update()
{
 if(!alive)  //we don't need to update dead actors.
   return;

 Actor::update();
 preform_worktype();

 if(is_poisoned())
  updatePoison();

 return;
}

bool U6Actor::updateSchedule(uint8 hour)
{
 bool ret;
 if((ret = Actor::updateSchedule(hour)) == true) //walk to next schedule location if required.
   {
    if(sched[sched_pos] != NULL && (sched[sched_pos]->x != x || sched[sched_pos]->y != y || sched[sched_pos]->z != z))
       set_worktype(WORKTYPE_U6_WALK_TO_LOCATION);
   }

 return ret;
}

inline void U6Actor::updatePoison()
{
 if(poison_counter == 0)
  {
   poison_counter = NUVIE_RAND() % 35;
   hit(1, ACTOR_FORCE_HIT);
  }
 else
   poison_counter--;
}

// workout our direction based on actor_type and frame_n
inline void U6Actor::discover_direction()
{
 if(actor_type->frames_per_direction != 0)
   direction = (frame_n - actor_type->tile_start_offset ) / actor_type->tiles_per_direction;
 else
   direction = NUVIE_DIR_S;
}

void U6Actor::set_direction(uint8 d)
{
 uint8 frames_per_dir = (actor_type->frames_per_direction != 0)
                         ? actor_type->frames_per_direction : 4;
 if(d >= 4)
   return;

 if(walk_frame == 0)
   walk_frame_inc = 1; // loop forward
 else if(walk_frame == (frames_per_dir - 1))
   walk_frame_inc = -1; // loop backward
 walk_frame = (walk_frame + walk_frame_inc)%frames_per_dir;

 if(has_surrounding_objs())
   {
    if(direction != d)
      set_direction_of_surrounding_objs(d);
    else
     {
      if(can_move && actor_type->twitch_rand) //only twitch actors with a non zero twitch_rand.
        twitch_surrounding_objs();
     }
   }

 direction = d;

 //only change direction frame if the actor can twitch ie isn't sitting or in bed etc.
 if(can_move)
   frame_n = actor_type->tile_start_offset + (direction * actor_type->tiles_per_direction +
             (walk_frame * actor_type->tiles_per_frame ) + actor_type->tiles_per_frame - 1);

}

void U6Actor::clear()
{
 if(has_surrounding_objs())
  {
   remove_surrounding_objs_from_map();
   clear_surrounding_objs_list(REMOVE_SURROUNDING_OBJS);
  }

 Actor::clear();

 return;
}



bool U6Actor::move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move)
{
 bool ret;
 sint16 rel_x, rel_y;
 MsgScroll *scroll = Game::get_game()->get_scroll();
 Player *player = Game::get_game()->get_player();
 Party *party = player->get_party();
 MapCoord old_pos = get_location();

 if(has_surrounding_objs())
   remove_surrounding_objs_from_map();

 rel_x = new_x - x;
 rel_y = new_y - y;

 ret = Actor::move(new_x,new_y,new_z,force_move);

 if(ret == true)
  {
   if(has_surrounding_objs())
      move_surrounding_objs_relative(rel_x, rel_y);

   Obj *obj = obj_manager->get_obj(new_x,new_y,new_z); // Ouch, we get obj in Actor::move() too :(
   if(obj)
     {
      if(actor_type->can_sit)
        {
         sit_on_chair(obj); // make the Actor sit if they are on top of a chair.
        }

      if(obj->obj_n == OBJ_U6_FIRE_FIELD) // ouch
        {
         hit(5); // -?? hp?
         if(in_party)
            scroll->message("Ouch!\n");
        }
      if(obj->obj_n == OBJ_U6_POISON_FIELD && !is_poisoned()) // ick
        {
         set_poisoned(true);
         
         if(in_party)
           {
            scroll->display_string(party->get_actor_name(party->get_member_num(this)));
            scroll->display_string(" poisoned!\n");
            scroll->display_prompt();
           }
//       Avatar:
//       >%s poisoned!
//
//       Avatar:
//       >
        }
      if(obj->obj_n == OBJ_U6_SLEEP_FIELD) // Zzz
        {
         new HitEffect(this); // no hp loss
         //fall asleep (change worktype?)
         if(in_party)
            scroll->message("Zzz...\n");
        }
     }

     // temp. fix; this too should be done with UseCode (and don't move the mirror)
     if(old_pos.y > 0 && new_y > 0)
     {
         Obj *old_mirror = obj_manager->get_obj_of_type_from_location(OBJ_U6_MIRROR,old_pos.x,old_pos.y-1,old_pos.z);
         Obj *mirror = obj_manager->get_obj_of_type_from_location(OBJ_U6_MIRROR,new_x,new_y-1,new_z);
         if(old_mirror) old_mirror->frame_n = 0;
         if(mirror)     mirror->frame_n = 1;
     }

   // Cyclops: shake ground if player is near
   if(actor_type->base_obj_n == OBJ_U6_CYCLOPS && is_nearby(player->get_actor()))
      new QuakeEffect(1, 200, player->get_actor());
  }

 if(has_surrounding_objs()) //add our surrounding objects back onto the map.
   add_surrounding_objs_to_map();

 return ret;
}

bool U6Actor::check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors)
{
 Tile *map_tile;

 if(Actor::check_move(new_x, new_y, new_z, ignore_actors) == false)
    return false;

    switch(actor_type->movetype)
      {
       case MOVETYPE_U6_WATER_HIGH : // for HIGH we only want to move to open water.
                                     // No shorelines.
                                     map_tile = map->get_tile(new_x, new_y, new_z, MAP_ORIGINAL_TILE);
                                     if(map_tile->tile_num >= 16 && map_tile->tile_num <= 47)
                                       return false;
                                    //fall through to MOVETYPE_U6_WATER_LOW
       case MOVETYPE_U6_WATER_LOW : if(!map->is_water(new_x, new_y, new_z))
                                       return false;
                                    break;

       case MOVETYPE_U6_AIR_LOW : map_tile = map->get_tile(new_x, new_y, new_z, MAP_ORIGINAL_TILE);
                                  if(map_tile->flags1 & TILEFLAG_WALL) //low air boundry
                                    return false;

                                  map_tile = obj_manager->get_obj_tile(new_x, new_y, new_z, false);
                                  if(map_tile && (map_tile->flags1 & TILEFLAG_WALL ||
                                     (map_tile->flags2 & (TILEFLAG_DOUBLE_WIDTH | TILEFLAG_DOUBLE_HEIGHT)) == (TILEFLAG_DOUBLE_WIDTH | TILEFLAG_DOUBLE_HEIGHT)))
                                    return false;
                                  break;

                                  //fall through to MOVETYPE_U6_AIR_HIGH
       case MOVETYPE_U6_AIR_HIGH : if(map->is_boundary(new_x, new_y, new_z))
                                    return false; //FIX for proper air boundary
                                  break;
       case MOVETYPE_U6_LAND :
       default : if(map->is_passable(new_x,new_y,new_z) == false)
                    if(obj_n != OBJ_U6_MOUSE /* try to go through mousehole */
                       || obj_manager->get_obj_of_type_from_location(OBJ_U6_MOUSEHOLE,new_x,new_y,new_z) == NULL)
                       return(false);
      }

 return(true);
}

// attempt to sit if obj is a chair.

bool U6Actor::sit_on_chair(Obj *obj)
{
   if(actor_type->can_sit && obj)
     {
         if(obj->obj_n == OBJ_U6_CHAIR)  // make the actor sit on a chair.
           {
            frame_n = (obj->frame_n * 4) + 3;
            direction = obj->frame_n;
            can_move = false;
            return true;
           }

         if(obj->obj_n == OBJ_U6_THRONE  && obj->frame_n == 3) //make actor sit on LB's throne.
           {
            frame_n = 8 + 3; //sitting facing south.
            direction = NUVIE_DIR_S;
            can_move = false;
            return true;
           }
     }

 return false;
}

uint8 U6Actor::get_object_readiable_location(uint16 obj_n)
{
 uint16 i;

 for(i=0;readiable_objects[i].obj_n != OBJ_U6_NOTHING;i++)
   {
    if(obj_n == readiable_objects[i].obj_n)
      return readiable_objects[i].readiable_location;
   }

 return ACTOR_NOT_READIABLE;
}

const CombatType *U6Actor::get_object_combat_type(uint16 obj_n)
{
 uint16 i;

 for(i=0;u6combat_objects[i].obj_n != OBJ_U6_NOTHING;i++)
   {
    if(obj_n == u6combat_objects[i].obj_n)
      return &u6combat_objects[i];
   }

 return NULL;
}

const CombatType *U6Actor::get_hand_combat_type()
{
 return &u6combat_hand;
}

bool U6Actor::weapon_can_hit(const CombatType *weapon, uint16 target_x, uint16 target_y)
{
 sint16 off_x, off_y;

 if(!weapon)
   return false;

 off_x = x - target_x;
 off_y = y - target_y;
 
 if(abs(off_x) > 5 || abs(off_y) > 5)
   return false;
  
 if(weapon->hit_range == 0)
   return true;

 return (bool)u6combat_hitrange_tbl[weapon->hit_range - 1][(5 + off_y) * 11 + (5 + off_x)];
}

void U6Actor::twitch()
{

 if(can_move == false || actor_type->twitch_rand == 0)
   return;

 if(NUVIE_RAND()%actor_type->twitch_rand == 1)
  {
   if(actor_type->frames_per_direction == 0)
     walk_frame = (walk_frame + 1) % 4;
   else
     walk_frame = NUVIE_RAND()%actor_type->frames_per_direction;

   if(has_surrounding_objs())
    {
	 switch(obj_n)
	   {
		case OBJ_U6_HYDRA : twitch_surrounding_hydra_objs(); break;
		case OBJ_U6_DRAGON :
		default : twitch_surrounding_objs(); break;
	   }
	}

   frame_n = actor_type->tile_start_offset + (direction * actor_type->tiles_per_direction + (walk_frame * actor_type->tiles_per_frame)  + actor_type->tiles_per_frame - 1);
  }

 return;
}

void U6Actor::set_poisoned(bool poisoned)
{
 if(poisoned)
  {
   status_flags |= ACTOR_STATUS_POISONED;
   new HitEffect(this); // no direct hp loss
   Game::get_game()->get_view_manager()->update();
  }
 else
  {
   status_flags &= (0xff ^ ACTOR_STATUS_POISONED);
   poison_counter = 0;
  }
}

void U6Actor::preform_worktype()
{
 if(in_party) // do nothing here
  return;
 switch(worktype)
  {
   case WORKTYPE_U6_FACE_NORTH :
   case WORKTYPE_U6_FACE_EAST  :
   case WORKTYPE_U6_FACE_SOUTH :
   case WORKTYPE_U6_FACE_WEST  :
     break;
   case WORKTYPE_U6_IN_PARTY :
   case WORKTYPE_U6_WALK_TO_LOCATION : wt_walk_to_location();
                                      break;

   case WORKTYPE_U6_GUARD_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_GUARD_WALK_EAST_WEST   :
   case WORKTYPE_U6_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_WALK_EAST_WEST   : wt_walk_straight(); break;

   case WORKTYPE_U6_WORK :
   case WORKTYPE_U6_WANDER_AROUND   : wt_wander_around(); break;
   case WORKTYPE_U6_ANIMAL_WANDER : wt_farm_animal_wander(); break;
   case WORKTYPE_U6_BEG : wt_beg(); break;
//   case WORKTYPE_U6_
//                     break;
  }

 return;
}

void U6Actor::set_worktype(uint8 new_worktype)
{
 if(new_worktype == worktype)
   return;

 //reset to base obj_n
 if(worktype > 2) //don't revert for party worktypes as they might be riding a horse.
   set_actor_obj_n(base_actor_type->base_obj_n);

 if(worktype == WORKTYPE_U6_SLEEP || worktype == WORKTYPE_U6_PLAY_LUTE)
   {
    frame_n = old_frame_n;
   }
 Actor::set_worktype(new_worktype);

 //FIX from here.

 switch(worktype)
  {
   case WORKTYPE_U6_FACE_NORTH : set_direction(NUVIE_DIR_N); break;
   case WORKTYPE_U6_FACE_EAST  : set_direction(NUVIE_DIR_E); break;
   case WORKTYPE_U6_FACE_SOUTH : set_direction(NUVIE_DIR_S); break;
   case WORKTYPE_U6_FACE_WEST  : set_direction(NUVIE_DIR_W); break;

   case WORKTYPE_U6_SLEEP : wt_sleep(); break;
   case WORKTYPE_U6_PLAY_LUTE : wt_play_lute(); break;
  }
}


void U6Actor::wt_walk_to_location()
{
 if(pathfinder && pathfinder->reached_goal() && sched[sched_pos] != NULL)
    set_worktype(sched[sched_pos]->worktype);
 return;
}


void U6Actor::wt_walk_straight()
{
 uint8 dir = get_direction();
 set_direction(dir); //update walk frame FIX this!
 if(worktype == WORKTYPE_U6_WALK_NORTH_SOUTH || worktype == WORKTYPE_U6_GUARD_WALK_NORTH_SOUTH)
   {
    if(dir != NUVIE_DIR_N && dir != NUVIE_DIR_S)
      dir = NUVIE_DIR_S;
    if(dir == NUVIE_DIR_N) // move up if blocked face down
       {
        if(moveRelative(0,-1) == false)
          set_direction(NUVIE_DIR_S);
       }
    else // move down if blocked face up
       {
        if(moveRelative(0,1) == false)
          set_direction(NUVIE_DIR_N);
       }
   }
 else //WORKTYPE_U6_WALK_EAST_WEST, WORKTYPE_U6_GUARD_WALK_EAST_WEST
   {
    if(dir != NUVIE_DIR_W && dir != NUVIE_DIR_E)
      dir = NUVIE_DIR_E;
    if(dir == NUVIE_DIR_W) //move left if blocked face right
       {
        if(moveRelative(-1,0) == false)
          set_direction(NUVIE_DIR_E);
       }
    else  //move right if blocked face left
       {
        if(moveRelative(1,0) == false)
          set_direction(NUVIE_DIR_W);
       }
   }
}

void U6Actor::wt_wander_around()
{
 uint8 new_direction;
 sint32 xdist = 0, ydist = 0;
 if(work_location.x || work_location.y)
   {
    xdist = x - work_location.x;
    ydist = y - work_location.y;
   }

 if(NUVIE_RAND()%8 == 1)
   {
    new_direction = NUVIE_RAND()%4;
    set_direction(new_direction);
    switch(new_direction)
      {
       case 0 : if(ydist > -4) moveRelative(0,-1); break;
       case 1 : if(xdist < +4) moveRelative(1,0); break;
       case 2 : if(ydist < +4) moveRelative(0,1); break;
       case 3 : if(xdist > -4) moveRelative(-1,0); break;
      }
   }

 return;
}

// wander around but don't cross boundaries or fences. Used for cows and horses.

void U6Actor::wt_farm_animal_wander()
{
 uint8 new_direction;
 sint8 rel_x = 0, rel_y = 0;

 if(NUVIE_RAND()%8 == 1)
   {
    new_direction = NUVIE_RAND()%4;

    switch(new_direction)
      {
       case NUVIE_DIR_N : rel_y = -1; break;
       case NUVIE_DIR_E : rel_x = 1; break;
       case NUVIE_DIR_S : rel_y = 1; break;
       case NUVIE_DIR_W : rel_x = -1; break;
      }

    if(obj_manager->get_obj_of_type_from_location(OBJ_U6_FENCE,x + rel_x, y + rel_y, z) == NULL)
        {
         if(moveRelative(rel_x,rel_y))
            set_direction(new_direction);
        }

   }

 return;
}


/* Wander around, approach and talk to the player character if visible.
 * Beg modes: 0 = waiting for target, 1 = following, 2 = just loiter
 * FIXME: Might also be done using 3 worktypes.
 */
void U6Actor::wt_beg()
{
    Player *player = Game::get_game()->get_player();
    Actor *actor = player->get_actor();
    uint32 dist_from_start = 0;
    if(work_location.x || work_location.y)
        dist_from_start = get_location().distance(work_location);

    if(beg_mode == 0)
        if(is_nearby(actor)) // look for victi... er, player
            beg_mode = 1;
    if(beg_mode == 1)
    {
        beg_mode = is_nearby(actor) ? 1 : 0; // still visible?
        Party *party = player->get_party();
        MapCoord me(x,y,z), them(0,0,0);
        for(uint32 p = 0; p < party->get_party_size(); p++)
        {
            party->get_actor(p)->get_location(&them.x, &them.y, &them.z);
            if(me.distance(them) <= 1 && z == them.z)
            {
                // talk to me :)
                stop_walking();
                // FIXME: this check should be in Converse
                if(!player->in_party_mode())
                {
                    MsgScroll *scroll = Game::get_game()->get_scroll();
                    scroll->display_string("\nNot in solo mode.\n");
                    scroll->display_string("\n");
                    scroll->display_prompt();
                }
                else if(Game::get_game()->get_converse()->start(this))
                {
                    actor->face_actor(this);
                    face_actor(actor);
                }
                beg_mode = 2;
                return; // done
            }
        }
        // get closer
        if(dist_from_start < 4)
        {
            actor->get_location(&them.x, &them.y, &them.z);
            swalk(them, 1);
        }
        else swalk(work_location, 1);
    }
    else
        wt_wander_around();
}


void U6Actor::wt_sleep()
{
 Obj *obj = obj_manager->get_obj(x,y,z);
 if(obj)
   {
    if(obj->obj_n == OBJ_U6_BED)
      {
       if(obj->frame_n == 1 || obj->frame_n == 5) //horizontal bed
         {
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 0;
         }
       if(obj->frame_n == 7 || obj->frame_n == 10) //vertical bed
         {
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 1;
         }
      }
    else // lay down on the ground using the dead body frame
      {
       if(actor_type->can_laydown)
        {
         old_frame_n = frame_n;
         obj_n = actor_type->dead_obj_n;
         frame_n = actor_type->dead_frame_n;
        }
      }
   }

 can_move = false;

 return;
}

void U6Actor::wt_play_lute()
{
 old_frame_n = frame_n;

 set_actor_obj_n(OBJ_U6_MUSICIAN_PLAYING);

 frame_n = direction * actor_type->tiles_per_direction;

 return;
}

void U6Actor::set_actor_obj_n(uint16 new_obj_n)
{
 old_frame_n = frame_n;

 obj_n = new_obj_n;
 actor_type = get_actor_type(new_obj_n);

 return;
}

inline const U6ActorType *U6Actor::get_actor_type(uint16 new_obj_n)
{
 const U6ActorType *type;

 for(type = u6ActorTypes; type->base_obj_n != OBJ_U6_NOTHING; type++)
  {
   if(type->base_obj_n == new_obj_n)
     break;
  }

 return type;
}

inline bool U6Actor::has_surrounding_objs()
{
 if(actor_type->tile_type == ACTOR_DT || actor_type->tile_type == ACTOR_MT)
   return true;

 return false;
}

inline void U6Actor::remove_surrounding_objs_from_map()
{
 std::list<Obj *>::iterator obj;

 for(obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++)
    obj_manager->remove_obj((*obj));

 return;
}

inline void U6Actor::add_surrounding_objs_to_map()
{
 std::list<Obj *>::iterator obj;

 for(obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++)
    obj_manager->add_obj((*obj),OBJ_ADD_TOP);

 return;
}

inline void U6Actor::move_surrounding_objs_relative(sint16 rel_x, sint16 rel_y)
{
 std::list<Obj *>::iterator obj;

 for(obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++)
    {
     (*obj)->x += rel_x;
     (*obj)->y += rel_y;
    }

 return;
}


inline void U6Actor::set_direction_of_surrounding_objs(uint8 new_direction)
{
 remove_surrounding_objs_from_map();

 switch(obj_n)
   {
    case OBJ_U6_SHIP : set_direction_of_surrounding_ship_objs(new_direction); break;

    case OBJ_U6_GIANT_SCORPION :
    case OBJ_U6_GIANT_ANT :
    case OBJ_U6_COW :
    case OBJ_U6_ALLIGATOR :
    case OBJ_U6_HORSE :
    case OBJ_U6_HORSE_WITH_RIDER : set_direction_of_surrounding_splitactor_objs(new_direction); break;

	case OBJ_U6_DRAGON : set_direction_of_surrounding_dragon_objs(new_direction); break;
   }

 add_surrounding_objs_to_map();

 return;
}

inline void U6Actor::set_direction_of_surrounding_ship_objs(uint8 new_direction)
{
 std::list<Obj *>::iterator obj;
 uint16 pitch = map->get_width(z);

 obj = surrounding_objects.begin();
 if(obj == surrounding_objects.end())
  return;

 (*obj)->x = x;
 (*obj)->y = y;

 (*obj)->frame_n =  new_direction * actor_type->tiles_per_direction + actor_type->tiles_per_frame - 1;
 switch(new_direction)
  {
   case NUVIE_DIR_N : if(y == 0)
                        (*obj)->y = pitch - 1;
                      else
                        (*obj)->y = y - 1;
                      break;

   case NUVIE_DIR_E : if(x == pitch - 1)
                        (*obj)->x = 0;
                      else
                        (*obj)->x = x + 1;
                      break;

   case NUVIE_DIR_S : if(y == pitch - 1)
                        (*obj)->y = 0;
                      else
                        (*obj)->y = y + 1;
                      break;

   case NUVIE_DIR_W : if(x == 0)
                        (*obj)->x = pitch - 1;
                      else
                        (*obj)->x = x - 1;
                      break;
  }

 obj++;
 if(obj == surrounding_objects.end())
  return;

 (*obj)->x = x;
 (*obj)->y = y;

 (*obj)->frame_n =  16 + (new_direction * actor_type->tiles_per_direction + actor_type->tiles_per_frame - 1);
 switch(new_direction)
  {
   case NUVIE_DIR_N : if(y == pitch - 1)
                        (*obj)->y = 0;
                      else
                        (*obj)->y = y + 1;
                      break;

   case NUVIE_DIR_E : if(x == 0)
                        (*obj)->x = pitch - 1;
                      else
                        (*obj)->x = x - 1;
                      break;

   case NUVIE_DIR_S : if(y == 0)
                        (*obj)->y = pitch - 1;
                      else
                        (*obj)->y = y - 1;
                      break;

   case NUVIE_DIR_W : if(x == pitch - 1)
                        (*obj)->x = 0;
                      else
                        (*obj)->x = x + 1;
                      break;
  }

}

inline void U6Actor::set_direction_of_surrounding_splitactor_objs(uint8 new_direction)
{
 Obj *obj;
 uint16 pitch = map->get_width(z);

 if(surrounding_objects.empty())
   return;

 obj = surrounding_objects.back();

 obj->frame_n =  8 + (new_direction * actor_type->tiles_per_direction + actor_type->tiles_per_frame - 1);

 obj->x = x;
 obj->y = y;

 switch(new_direction)
  {
   case NUVIE_DIR_N : if(y == pitch - 1)
                        obj->y = 0;
                      else
                        obj->y = y + 1;
                      break;

   case NUVIE_DIR_E : if(x == 0)
                        obj->x = pitch - 1;
                      else
                        obj->x = x - 1;
                      break;

   case NUVIE_DIR_S : if(y == 0)
                        obj->y = pitch - 1;
                      else
                        obj->y = y - 1;
                      break;

   case NUVIE_DIR_W : if(x == pitch - 1)
                        obj->x = 0;
                      else
                        obj->x = x + 1;
                      break;
  }

}

inline void U6Actor::set_direction_of_surrounding_dragon_objs(uint8 new_direction)
{
 std::list<Obj *>::iterator obj;
 uint8 frame_offset =  (new_direction * actor_type->tiles_per_direction + actor_type->tiles_per_frame - 1);
 Obj *head, *tail, *wing1, *wing2;

 //NOTE! this is dependant on the order the in which the objects are loaded in U6Actor::init_dragon()

 obj = surrounding_objects.begin();
 if(obj == surrounding_objects.end())
  return;
 head = *obj;
 head->frame_n =  8 + frame_offset;
 head->x = x;
 head->y = y;

 obj++;
 if(obj == surrounding_objects.end())
  return;
 tail = *obj;
 tail->frame_n =  16 + frame_offset;
 tail->x = x;
 tail->y = y;

 obj++;
 if(obj == surrounding_objects.end())
  return;
 wing1 = *obj;
 wing1->frame_n =  24 + frame_offset;
 wing1->x = x;
 wing1->y = y;

 obj++;
 if(obj == surrounding_objects.end())
  return;
 wing2 = *obj;
 wing2->frame_n =  32 + frame_offset;
 wing2->x = x;
 wing2->y = y;

 switch(new_direction)
  {
   case NUVIE_DIR_N : head->y = y - 1;
                      tail->y = y + 1;
					  wing1->x = x - 1;
					  wing2->x = x + 1;
					  break;

   case NUVIE_DIR_E : head->x = x + 1;
                      tail->x = x - 1;
					  wing1->y = y - 1;
					  wing2->y = y + 1;
					  break;

   case NUVIE_DIR_S : head->y = y + 1;
                      tail->y = y - 1;
					  wing1->x = x + 1;
					  wing2->x = x - 1;
					  break;

   case NUVIE_DIR_W : head->x = x - 1;
                      tail->x = x + 1;
					  wing1->y = y + 1;
					  wing2->y = y - 1;
					  break;
  }

}

inline void U6Actor::twitch_surrounding_objs()
{
 std::list<Obj *>::iterator obj;

 for(obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++)
   {
    twitch_obj(*obj);
   }

}

inline void U6Actor::twitch_surrounding_dragon_objs()
{
}

inline void U6Actor::twitch_surrounding_hydra_objs()
{
 uint8 i;
 std::list<Obj *>::iterator obj;

 //Note! list order is important here. As it corresponds to the frame order in the tile set. This is definied in init_hydra()

 for(i = 0, obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++, i += 4)
   {
    if(NUVIE_RAND() % 4 == 0)
	   (*obj)->frame_n = i + (((*obj)->frame_n - i + 1) % 4);
   }
}

inline void U6Actor::twitch_obj(Obj *obj)
{
 obj->frame_n = (obj->frame_n / (actor_type->frames_per_direction * 4) * (actor_type->frames_per_direction * 4)) + direction * actor_type->tiles_per_direction +
                       walk_frame * actor_type->tiles_per_frame;

}

inline void U6Actor::clear_surrounding_objs_list(bool delete_objs)
{
 std::list<Obj *>::iterator obj;

 if(surrounding_objects.empty())
   return;

 if(delete_objs == false)
  {
   surrounding_objects.resize(0);
   return;
  }

 obj = surrounding_objects.begin();

 for(;!surrounding_objects.empty();)
  {
   obj_manager->remove_obj(*obj);
   delete *obj;
   obj = surrounding_objects.erase(obj);
  }

 return;
}

inline void U6Actor::init_surrounding_obj(uint16 x, uint16 y, uint8 z, uint16 actor_obj_n, uint16 obj_frame_n)
{
 Obj *obj;

 obj = obj_manager->get_obj(x, y, z);
 if(obj == NULL || actor_obj_n != obj->obj_n || (obj->quality != 0 && obj->quality != id_n))
  {
   obj = new Obj();
   obj->x = x;
   obj->y = y;
   obj->z = z;
   obj->obj_n = actor_obj_n;
   obj->frame_n = obj_frame_n;
   obj_manager->add_obj(obj);
  }

 obj->quality = id_n;
 surrounding_objects.push_back(obj);

 return;
}


void U6Actor::die()
{
 Game *game = Game::get_game();
 Party *party = game->get_party();
 Player *player = game->get_player();
 MapCoord actor_loc = get_location();
 MsgScroll *scroll = game->get_scroll();
 
 Actor::die();
    
 if(in_party)
  {
   if(party->get_member_num(this) == 0) // Avatar
     {
      scroll->display_string("\nAn unending darkness engulfs thee...\n\n");
      scroll->display_string("A voice in the darkness intones, \"KAL LOR!\"\n");
      
      player->set_party_mode(party->get_actor(0)); //set party mode with the avatar as the leader.
      player->move(0x133,0x160,0); //move to LB's castle.
      //move party to LB's castle
     }
   else
     {
      party->remove_actor(this);
      if(player->get_actor() == this)
        player->set_party_mode(party->get_actor(0)); //set party mode with the avatar as the leader.
     }
  }
  

  
    // we don't generate a dead body if the avatar dies because they will be ressurrected.  
    if(actor_type->dead_obj_n != OBJ_U6_NOTHING)
    {
        if(in_party && party->get_member_num(this) == 0) // unready all items on the avatar.
          remove_all_readied_objects();
        else
         { //if not avatar then create a dead body and place on the map.
          Obj *dead_body = new Obj;
          dead_body->obj_n = actor_type->dead_obj_n;
          dead_body->frame_n = actor_type->dead_frame_n;
          dead_body->x = actor_loc.x; dead_body->y = actor_loc.y; dead_body->z = actor_loc.z;
          dead_body->quality = id_n;
          dead_body->status = OBJ_STATUS_OK_TO_TAKE;
          if(temp_actor)
            dead_body->status |= OBJ_STATUS_TEMPORARY;
          
          // FIX: move my inventory into the dead body container
          all_items_to_container(dead_body);

          obj_manager->add_obj(dead_body, true);
         }
    }

 // FIX
//    if I am the Player, fade-out, restore party, move to castle, fade-in
    if(this != game->get_player()->get_actor())
    {
        move(0,0,0,ACTOR_FORCE_MOVE); // ?? not sure if dead actors get moved
        game->get_party()->remove_actor(this);
    }
//    add some blood? or do that in hit?

}

// frozen by worktype or status
bool U6Actor::is_immobile()
{
    return(worktype == WORKTYPE_U6_MOTIONLESS
           || worktype == WORKTYPE_U6_IMMOBILE
           || can_move == false);
}


bool U6Actor::is_sleeping()
{
    return(worktype == WORKTYPE_U6_SLEEP);
}
