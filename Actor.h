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

class Actor
{
 friend class ActorManager;
 
 Map *map;
 
 uint16 x;
 uint16 y;
 uint16 z;
 
 uint16 a_num;
 uint16 frame_n;
 
 bool alive;
 bool met_player;
 
 bool in_party;
 
 uint8 max_str;
 uint8 max_hp;
 
 char *name;
 
 public:
 
 Actor(Map *m);
 ~Actor();
 
 bool is_alive();
 
};

#endif /* __Actor_h__ */
