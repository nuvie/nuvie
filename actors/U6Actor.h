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

#include "Actor.h"

// Worktype codes
#define WORKTYPE_U6_IN_PARTY 0x1
#define WORKTYPE_U6_ANIMAL_WANDER 0xc

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
#define WORKTYPE_U6_PLAY_LUTE 0x95
#define WORKTYPE_U6_BEG 0x96

#define MOVETYPE_U6_LAND       1
#define MOVETYPE_U6_WATER_LOW  2 // skiffs, rafts
#define MOVETYPE_U6_WATER_HIGH 3 // ships
#define MOVETYPE_U6_AIR_LOW    4 // balloon, birds... this movetype cannot cross mountain tops.
#define MOVETYPE_U6_AIR_HIGH   5 // dragons

#define REMOVE_SURROUNDING_OBJS true

typedef struct {
 uint16 base_obj_n;
 uint8 frames_per_direction;
 uint8 tiles_per_direction;
 uint8 tiles_per_frame;
 uint8 tile_start_offset; //used for ships where the frame_n starts at 8
 uint16 dead_obj_n;
 uint8 dead_frame_n;
 bool can_laydown;
 bool can_sit;
 bool has_surrounding_objs;
 uint8 movetype;
 uint16 twitch_rand; //used to control how frequently an actor twitches, lower numbers twitch more
} U6ActorType;

class U6Actor: public Actor
{
 protected:
 
 const U6ActorType *actor_type;
 const U6ActorType *base_actor_type;

 public:
 
 U6Actor(Map *m, ObjManager *om, GameClock *c);
 ~U6Actor();
 
 bool init();
 uint16 get_downward_facing_tile_num();
 void update();
 bool updateSchedule(uint8 hour);
 void set_worktype(uint8 new_worktype);
 void preform_worktype();
 void set_direction(uint8 d);
 void clear();
 bool move(sint16 new_x, sint16 new_y, sint8 new_z, bool force_move=false);
 bool check_move(sint16 new_x, sint16 new_y, sint8 new_z, bool ignore_actors=false);
 void twitch();
 
 uint8 get_object_readiable_location(uint16 obj_n);
 
 protected:
 bool init_ship();
 bool init_splitactor(); //cows, horses etc.
 inline void discover_direction();
 void wt_walk_to_location();
 void wt_walk_straight();
 void wt_wander_around();
 void wt_beg();
 void wt_sleep();
 void wt_play_lute();

 inline const U6ActorType *get_actor_type(uint16 new_obj_n);
 void set_actor_obj_n(uint16 new_obj_n);
 
 inline void remove_surrounding_objs_from_map();
 inline void add_surrounding_objs_to_map();
 inline void move_surrounding_objs_relative(sint16 rel_x, sint16 rel_y);
 inline void set_direction_of_surrounding_objs(uint8 new_direction);
 inline void set_direction_of_surrounding_ship_objs(uint8 new_direction);
 inline void set_direction_of_surrounding_splitactor_objs(uint8 new_direction);
 
 inline void twitch_surrounding_objs();
 inline void clear_surrounding_objs_list(bool delete_objs=false);
};

#endif /* __U6Actor_h__ */
