/*
 *  player.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Sun Mar 23 2003.
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

#include "player.h"

Player::Player(Configuration *cfg)
{
 config = cfg;
}

bool Player::init(Actor *a, ActorManager *am, MapWindow *mw)
{
 actor = a;
 actor_manager = am;
 map_window = mw;
 
 return true;
}

Actor *Player::get_actor()
{
 return actor;
}

void Player::moveLeft()
{
 actor->set_direction(3);
 if(actor->moveRelative(-1,0))
   map_window->moveRelative(-1,0);
}

void Player::moveRight()
{
 actor->set_direction(1);
 if(actor->moveRelative(1,0))
   map_window->moveRelative(1,0);
}

void Player::moveUp()
{
 actor->set_direction(0);
 if(actor->moveRelative(0,-1))
   map_window->moveRelative(0,-1);
}

void Player::moveDown()
{
 actor->set_direction(2);
 if(actor->moveRelative(0,1))
   map_window->moveRelative(0,1);
}
