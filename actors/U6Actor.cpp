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

#include "U6Actor.h"


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

void U6Actor::preform_worktype()
{
 switch(worktype)
  {
   case WORKTYPE_U6_WALK_TO_LOCATION : 
                                      break;
                                      
   case WORKTYPE_U6_FACE_NORTH : set_direction(ACTOR_DIR_U); break;
   case WORKTYPE_U6_FACE_EAST  : set_direction(ACTOR_DIR_R); break;
   case WORKTYPE_U6_FACE_SOUTH : set_direction(ACTOR_DIR_D); break;
   case WORKTYPE_U6_FACE_WEST  : set_direction(ACTOR_DIR_L); break;

   case WORKTYPE_U6_WALK_NORTH_SOUTH :
   case WORKTYPE_U6_WALK_EAST_WEST   : wt_walk_straight(); break;

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

 Actor::set_worktype(new_worktype);

 //FIX from here.

 switch(worktype)
  {
   case WORKTYPE_U6_SLEEP : break;
  }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
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
