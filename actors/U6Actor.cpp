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

#include "U6UseCode.h"
#include "pathfinder.h"
#include "U6Actor.h"

//static const uint8 sleep_objects[8];


U6Actor::U6Actor(Map *m, ObjManager *om, GameClock *c): Actor(m,om,c)
{
 
}

U6Actor::~U6Actor()
{
}

void U6Actor::update()
{
 Actor::update();
 preform_worktype();
 return;
}

bool U6Actor::updateSchedule()
{
 bool ret;
 if((ret = Actor::updateSchedule()) == true) //walk to next schedule location if required.
   {
    if(sched[sched_pos] != NULL && (sched[sched_pos]->x != x || sched[sched_pos]->y != y || sched[sched_pos]->z != z))
       set_worktype(WORKTYPE_U6_WALK_TO_LOCATION);
   }

 return ret;
}

void U6Actor::preform_worktype()
{
 switch(worktype)
  {
   case WORKTYPE_U6_IN_PARTY :
   case WORKTYPE_U6_WALK_TO_LOCATION : wt_walk_to_location();
                                      break;
                                      

   case WORKTYPE_U6_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_WALK_EAST_WEST   : wt_walk_straight(); break;

   case WORKTYPE_U6_WORK :
   case WORKTYPE_U6_WANDER_AROUND   : wt_wander_around(); break;
//   case WORKTYPE_U6_
//                     break;
  }

 return;
}

void U6Actor::set_worktype(uint8 new_worktype)
{
 if(new_worktype == worktype)
   return;

 if(worktype == WORKTYPE_U6_SLEEP)
   { 
    obj_n = old_obj_n;
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
  }
}

void U6Actor::wt_walk_to_location()
{
 if(pathfinder)
 {
    pathfinder->walk_path();
    if(pathfinder->reached_goal())
        {
         stop_walking();
         if(sched[sched_pos] != NULL)
           set_worktype(sched[sched_pos]->worktype);
        }
 }

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
 
 if(NUVIE_RAND()%8 == 1)
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

 return;
}

void U6Actor::wt_sleep()
{
 if(id_n == 5)
    printf("%d Sleep\n",id_n);
 
 Obj *obj = obj_manager->get_obj(x,y,z);
 if(obj)
   {
    if(obj->obj_n == OBJ_U6_BED)
      {
       if(obj->frame_n == 1 || obj->frame_n == 5) //horizontal bed
         {
          old_obj_n = obj_n;
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 0;
         }
       if(obj->frame_n == 7 || obj->frame_n == 10) //vertical bed
         {
          old_obj_n = obj_n;
          old_frame_n = frame_n;
          obj_n = OBJ_U6_PERSON_SLEEPING;
          frame_n = 1;
         }
      }
   }

 return;
}
