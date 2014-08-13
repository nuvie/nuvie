/*
 *  MDActor.cpp
 *  Nuvie
 *
 *  Created by the Nuvie Team on Thur Jul 12 2012.
 *  Copyright (c) 2012. All rights reserved.
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

#include "MDActor.h"

#define MD_DOWNWARD_FACING_FRAME_N 9

MDActor::MDActor(Map *m, ObjManager *om, GameClock *c): WOUActor(m,om,c)
{
}

MDActor::~MDActor()
{
}

bool MDActor::init()
{
 Actor::init();
 return true;
}

bool MDActor::will_not_talk()
{
	if(worktype == 0xa0)
		return true;
	return false;
}

bool MDActor::check_move(uint16 new_x, uint16 new_y, uint8 new_z, ActorMoveFlags flags)
{
 if(ethereal)
   return true;

 if(Actor::check_move(new_x, new_y, new_z, flags) == false)
    return false;

 return map->is_passable(new_x,new_y,new_z);
}

uint16 MDActor::get_downward_facing_tile_num()
{
 return get_tile_num(obj_n) + MD_DOWNWARD_FACING_FRAME_N;
}
