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
#include "Converse.h"

#include "U6ActorTypes.h"

//static const uint8 sleep_objects[8];

static uint8 walk_frame_tbl[4] = {0,1,2,1}; //FIX 


U6Actor::U6Actor(Map *m, ObjManager *om, GameClock *c): Actor(m,om,c)
{
 
}

U6Actor::~U6Actor()
{
}

bool U6Actor::init()
{
 set_actor_obj_n(obj_n); //set actor_type
 
 base_actor_type = get_actor_type(base_obj_n);
  
 discover_direction();

 if(actor_type->has_surrounding_objs)
   clear_surrounding_objs_list(); //clean up the old list if required.

 switch(obj_n) //gather surrounding objects from map if required
  {
   case OBJ_U6_SHIP : init_ship(); break;

   case OBJ_U6_GIANT_SCORPION :
   case OBJ_U6_GIANT_ANT :
   case OBJ_U6_COW :
   case OBJ_U6_ALLIGATOR :
   case OBJ_U6_HORSE :
   case OBJ_U6_HORSE_WITH_RIDER : init_splitactor(); break;
 

   default : break;
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
   case ACTOR_DIR_U : obj1_y = y+1;
                      obj2_y = y-1;
                      break;
   case ACTOR_DIR_R : obj1_x = x+1;
                      obj2_x = x-1;
                      break;
   case ACTOR_DIR_D : obj1_y = y-1;
                      obj2_y = y+1;
                      break;
   case ACTOR_DIR_L : obj1_x = x-1;
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
 Obj *obj;
 uint16 obj_x, obj_y;
 
 obj_x = x;
 obj_y = y;
   
 switch(direction) //FIX for world wrapping
  {
   case ACTOR_DIR_U : obj_y = y+1;
                      break;
   case ACTOR_DIR_R : obj_x = x-1;
                      break;
   case ACTOR_DIR_D : obj_y = y-1;
                      break;
   case ACTOR_DIR_L : obj_x = x+1;
                      break;
  }
  
 obj = obj_manager->get_obj(obj_x,obj_y,z);
 if(obj == NULL || obj->obj_n != obj_n) // create a new back object
   {
    obj = new Obj();
    obj->x = obj_x;
    obj->y = obj_y;
    obj->z = z;
    obj->obj_n = obj_n;
    obj->frame_n = frame_n + 8;
    obj_manager->add_obj(obj);
   }
   
 obj->quality = id_n; //associate actor id with obj.
 surrounding_objects.push_back(obj);

 return true;
}

uint16 U6Actor::get_downward_facing_tile_num()
{
 uint8 shift = 0;
 
 if(base_actor_type->frames_per_direction > 1) //we want the second frame for most actor types.
   shift = 1;
   
 return obj_manager->get_obj_tile_num(base_actor_type->base_obj_n) + base_actor_type->tile_start_offset + (ACTOR_DIR_D * base_actor_type->tiles_per_direction + base_actor_type->tiles_per_frame - 1) + shift;
}

void U6Actor::update()
{
 Actor::update();
 preform_worktype();
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

// workout our direction based on actor_type and frame_n
inline void U6Actor::discover_direction()
{
 if(actor_type->frames_per_direction != 0) 
   direction = (frame_n - actor_type->tile_start_offset ) / actor_type->tiles_per_direction;
 else
   direction = ACTOR_DIR_D;
}

void U6Actor::set_direction(uint8 d)
{
 if(d >= 4)
   return;
   

 if(actor_type->frames_per_direction == 0)
   walk_frame = (walk_frame + 1) % 4;
 else
   walk_frame = (walk_frame + 1) % actor_type->frames_per_direction;


 if(actor_type->has_surrounding_objs)
   {
    if(direction != d)
      set_direction_of_surrounding_objs(d);
    else
     {
      if(can_twitch && actor_type->twitch_rand) //only twitch actors with a non zero twitch_rand.
        twitch_surrounding_objs();
     }
   }

 direction = d;

 //only change direction frame if the actor can twitch ie isn't sitting or in bed etc.
 if(can_twitch)
   frame_n = actor_type->tile_start_offset + (direction * actor_type->tiles_per_direction + 
             (walk_frame_tbl[walk_frame] * actor_type->tiles_per_frame ) + actor_type->tiles_per_frame - 1);
 
}

void U6Actor::clear()
{
 if(actor_type->has_surrounding_objs)
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
 Party *party = Game::get_game()->get_party();
 
 if(actor_type->has_surrounding_objs)
   remove_surrounding_objs_from_map();

 rel_x = new_x - x;
 rel_y = new_y - y;
  
 ret = Actor::move(new_x,new_y,new_z,force_move);
 
 if(ret == true)
  {
   if(actor_type->has_surrounding_objs)
      move_surrounding_objs_relative(rel_x, rel_y);
   
   if(actor_type->can_sit)
     {
      Obj *obj = obj_manager->get_obj(new_x,new_y,new_z); // Ouch, we get obj in Actor::move() too :(
      if(obj)
        {
         if(obj->obj_n == OBJ_U6_CHAIR)  // make the actor sit on a chair.
           {
            frame_n = (obj->frame_n * 4) + 3;
            direction = obj->frame_n;
            can_twitch = false;
           }
       
         if(obj->obj_n == OBJ_U6_THRONE  && obj->frame_n == 3) //make actor sit on LB's throne.
           {
            frame_n = 8 + 3; //sitting facing south.
            direction = 2;
            can_twitch = false;
           }
         if(obj->obj_n == OBJ_U6_FIRE_FIELD) // ouch
           {
            //hit animation
            //-?? hp
            if(in_party)
               scroll->message("Ouch!\n");
           }
         if(obj->obj_n == OBJ_U6_POISON_FIELD/* && not poisoned*/) // ick
           {
            //hit animation
            if(in_party)
              {
               scroll->display_string(party->get_actor_name(party->get_member_num(this)));
               scroll->display_string(" poisoned!\n");
               scroll->display_prompt();
              }
//          Avatar:
//          >%s poisoned!
//
//          Avatar:
//          >
           }
         if(obj->obj_n == OBJ_U6_SLEEP_FIELD) // Zzz
           {
            //hit animation
            //fall asleep (change worktype?)
            if(in_party)
               scroll->message("Zzz...\n");
           }
        }
     }
  }

 if(actor_type->has_surrounding_objs) //add our surrounding objects back onto the map.
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
       case MOVETYPE_U6_AIR_LOW : 
       case MOVETYPE_U6_AIR_HIGH : if(map->is_boundary(new_x, new_y, new_z))
                                    return false; //FIX for proper air boundary
                                  break;
       case MOVETYPE_U6_LAND :
       default : if(map->is_passable(new_x,new_y,new_z) == false)
                    return(false);
      }

 return(true);
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

void U6Actor::twitch()
{
 
 if(can_twitch == false || actor_type->twitch_rand == 0)
   return;

 if(NUVIE_RAND()%actor_type->twitch_rand == 1)
  {
   if(actor_type->frames_per_direction == 0)
     walk_frame = (walk_frame + 1) % 4;
   else
     walk_frame = NUVIE_RAND()%actor_type->frames_per_direction;

   if(actor_type->has_surrounding_objs)
      twitch_surrounding_objs();

   frame_n = actor_type->tile_start_offset + (direction * actor_type->tiles_per_direction + (walk_frame * actor_type->tiles_per_frame)  + actor_type->tiles_per_frame - 1);
  }

 return;
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
                                      

   case WORKTYPE_U6_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_WALK_EAST_WEST   : wt_walk_straight(); break;

   case WORKTYPE_U6_WORK :
   case WORKTYPE_U6_ANIMAL_WANDER :
   case WORKTYPE_U6_WANDER_AROUND   : wt_wander_around(); break;
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
   case WORKTYPE_U6_FACE_NORTH : set_direction(ACTOR_DIR_U); break;
   case WORKTYPE_U6_FACE_EAST  : set_direction(ACTOR_DIR_R); break;
   case WORKTYPE_U6_FACE_SOUTH : set_direction(ACTOR_DIR_D); break;
   case WORKTYPE_U6_FACE_WEST  : set_direction(ACTOR_DIR_L); break;

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
 if(worktype == WORKTYPE_U6_WALK_NORTH_SOUTH)
   {
    if(dir != ACTOR_DIR_U && dir != ACTOR_DIR_D)
      dir = ACTOR_DIR_D;
    if(dir == ACTOR_DIR_U) // move up if blocked face down
       {
        if(moveRelative(0,-1) == false)
          set_direction(ACTOR_DIR_D);
       }
    else // move down if blocked face up
       {
        if(moveRelative(0,1) == false)
          set_direction(ACTOR_DIR_U);
       }
   }
 else //WORKTYPE_U6_WALK_EAST_WEST
   {
    if(dir != ACTOR_DIR_L && dir != ACTOR_DIR_R)
      dir = ACTOR_DIR_R;
    if(dir == ACTOR_DIR_L) //move left if blocked face right
       {
        if(moveRelative(-1,0) == false)
          set_direction(ACTOR_DIR_R);
       }
    else  //move right if blocked face left
       {
        if(moveRelative(1,0) == false)
          set_direction(ACTOR_DIR_L);
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


/* Wander around, approach and talk to the player character if visible.
 */
void U6Actor::wt_beg()
{
    static uint8 mode = 0;// 0 = waiting for target, 1 = following,
    Player *player = Game::get_game()->get_player();          // 2 = just loiter
    Actor *actor = player->get_actor();
    uint32 dist_from_start = 0;
    if(work_location.x || work_location.y)
        dist_from_start = get_location().distance(work_location);

    if(mode == 0)
        if(is_nearby(actor)) // look for victi... er, player
            mode = 1;
    if(mode == 1)
    {
        mode = is_nearby(actor) ? 1 : 0; // still visible?
        Party *party = player->get_party();
        MapCoord me(x,y,z), them(0,0,0);
        for(uint32 p = 0; p < party->get_party_size(); p++)
        {
            party->get_actor(p)->get_location(&them.x, &them.y, &them.z);
            if(me.distance(them) <= 1 && z == them.z)
            {
                // talk to me :)
                stop_walking();
                if(Game::get_game()->get_converse()->start(this))
                {
                    actor->face_actor(this);
                    face_actor(actor);
                }
                mode = 2;
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

 can_twitch = false;

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
   case ACTOR_DIR_U : if(y == 0)
                        (*obj)->y = pitch - 1;
                      else
                        (*obj)->y = y - 1;
                      break;

   case ACTOR_DIR_R : if(x == pitch - 1)
                        (*obj)->x = 0;
                      else
                        (*obj)->x = x + 1;
                      break;

   case ACTOR_DIR_D : if(y == pitch - 1)
                        (*obj)->y = 0;
                      else
                        (*obj)->y = y + 1;
                      break;

   case ACTOR_DIR_L : if(x == 0)
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
   case ACTOR_DIR_U : if(y == pitch - 1)
                        (*obj)->y = 0;
                      else
                        (*obj)->y = y + 1;
                      break;

   case ACTOR_DIR_R : if(x == 0)
                        (*obj)->x = pitch - 1;
                      else
                        (*obj)->x = x - 1;
                      break;

   case ACTOR_DIR_D : if(y == 0)
                        (*obj)->y = pitch - 1;
                      else
                        (*obj)->y = y - 1;
                      break;

   case ACTOR_DIR_L : if(x == pitch - 1)
                        (*obj)->x = 0;
                      else
                        (*obj)->x = x + 1;
                      break;
  }                   

}

inline void U6Actor::set_direction_of_surrounding_splitactor_objs(uint8 new_direction)
{
 std::list<Obj *>::iterator obj;
 uint16 pitch = map->get_width(z);
 
 obj = surrounding_objects.begin();
 if(obj == surrounding_objects.end())
  return;

 (*obj)->frame_n =  8 + (new_direction * actor_type->tiles_per_direction + actor_type->tiles_per_frame - 1);

 (*obj)->x = x;
 (*obj)->y = y;

 switch(new_direction)
  {
   case ACTOR_DIR_U : if(y == pitch - 1)
                        (*obj)->y = 0;
                      else
                        (*obj)->y = y + 1;
                      break;

   case ACTOR_DIR_R : if(x == 0)
                        (*obj)->x = pitch - 1;
                      else
                        (*obj)->x = x - 1;
                      break;

   case ACTOR_DIR_D : if(y == 0)
                        (*obj)->y = pitch - 1;
                      else
                        (*obj)->y = y - 1;
                      break;

   case ACTOR_DIR_L : if(x == pitch - 1)
                        (*obj)->x = 0;
                      else
                        (*obj)->x = x + 1;
                      break;
  }                   

}

inline void U6Actor::twitch_surrounding_objs()
{
 std::list<Obj *>::iterator obj;
 
 for(obj = surrounding_objects.begin(); obj != surrounding_objects.end(); obj++)
   {
    (*obj)->frame_n = ((*obj)->frame_n / (actor_type->frames_per_direction * 4) * (actor_type->frames_per_direction * 4)) + direction * actor_type->tiles_per_direction +
                       walk_frame_tbl[walk_frame] * actor_type->tiles_per_frame;
   }

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
