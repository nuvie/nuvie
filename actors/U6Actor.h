#ifndef __U6Actor_h__
#define __U6Actor_h__

/*
 *  U6Actor.h
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

#include "U6def.h"
#include "Actor.h"

// Worktype codes
#define WORKTYPE_U6_IN_PARTY 0x1
#define WORKTYPE_U6_WALK_TO_LOCATION 0x84

#define WORKTYPE_U6_FACE_NORTH 0x87
#define WORKTYPE_U6_FACE_EAST 0x88
#define WORKTYPE_U6_FACE_SOUTH 0x89
#define WORKTYPE_U6_FACE_WEST 0x8a

#define WORKTYPE_U6_WALK_NORTH_SOUTH 0x8b
#define WORKTYPE_U6_WALK_EAST_WEST 0xf //0x8c

#define WORKTYPE_U6_WANDER_AROUND 0x8f
#define WORKTYPE_U6_WORK 0x90
#define WORKTYPE_U6_SLEEP 0x91
#define WORKTYPE_U6_BEG 0x96

class U6Actor: public Actor
{
 
 public:
 
 U6Actor(Map *m, ObjManager *om, GameClock *c);
 ~U6Actor();
 
 
 void update();
 bool updateSchedule();
 void set_worktype(uint8 new_worktype);
 void preform_worktype();
 
 protected:
 void wt_walk_to_location();
 void wt_walk_straight();
 void wt_wander_around();
 void wt_beg();
 void wt_sleep();
};

#endif /* __U6Actor_h__ */
