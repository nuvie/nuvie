#ifndef __Actor_h__
#define __Actor_h__

/*
 *  Actor.h
 *  Nuive
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

#include "U6def.h"
#include "Map.h"

typedef struct {
uint16 x;
uint16 y;
uint8 z;
uint8 hour;
uint8 worktype;
} Schedule;

class Actor
{
 friend class ActorManager;
 friend class MapWindow;
 
 uint8 id_n;
 
 Map *map;
 
 uint16 x;
 uint16 y;
 uint16 z;
 
 uint16 a_num;
 uint16 frame_n;
 uint8 direction;
 uint8 walk_frame;
 
 bool standing;
 bool alive;
 bool met_player;
 
 bool in_party;
 
 uint8 strength;
 uint8 dex;
 uint8 inteligence;
 uint8 hp;
 uint8 flags;
 
 char *name;
 
 Schedule **sched;
 
 public:
 
 Actor(Map *m);
 ~Actor();
 
 bool is_alive();
 void get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level);
 uint16 get_tile_num();
 uint8 get_actor_num() { return(id_n); }
 uint8 get_flags() { return(flags); }

 void set_direction(uint8 d);
 void set_flags(uint8 newflags) { flags = newflags; }
 void set_flag(uint8 bitflag);
 void clear_flag(uint8 bitflag);

 bool moveRelative(sint16 rel_x, sint16 rel_y);
 bool move(sint16 new_x, sint16 new_y, sint8 new_z);
 
 void update();
 void set_in_party(bool state);
 
 protected:
 
 void loadSchedule(unsigned char *schedule_data, uint16 num);
};

#endif /* __Actor_h__ */
