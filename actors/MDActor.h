/*
 *  MDActor.h
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

#ifndef __MDActor_h__
#define __MDActor_h__
#include "Actor.h"


class MDActor: public Actor
{
 protected:

 public:

 MDActor(Map *m, ObjManager *om, GameClock *c);
 ~MDActor();

 bool init();
 bool will_not_talk();
 uint8 get_maxhp() { return(((level * 24 + strength * 2) < 255) ? (level * 24 + strength * 2) : 255); }

 bool check_move(uint16 new_x, uint16 new_y, uint8 new_z, ActorMoveFlags flags=0);
};

#endif /* __MDActor_h__ */
