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

#include "Player.h"

Player::Player(Configuration *cfg)
{
 config = cfg;
}

bool Player::init(Actor *a, ActorManager *am, MapWindow *mw)
{
 actor = a;
 actor_manager = am;
 map_window = mw;
 
 a->set_in_party(true);
 actor_manager->set_player(actor);
 
 return true;
}

Actor *Player::get_actor()
{
 return actor;
}

void Player::get_location(uint16 *ret_x, uint16 *ret_y, uint8 *ret_level)
{
 actor->get_location(ret_x,ret_y,ret_level);
}

void Player::moveRelative(sint16 rel_x, sint16 rel_y)
{
 if(rel_x < 0)
   actor->set_direction(3);
 if(rel_x > 0)
   actor->set_direction(1);
 if(rel_y < 0)
   actor->set_direction(0);
 if(rel_y > 0)
   actor->set_direction(2);
   
 if(actor->moveRelative(rel_x,rel_y))
   map_window->moveMapRelative(rel_x,rel_y);
}

void Player::move(sint16 new_x, sint16 new_y, uint8 new_level)
{
 if(actor->move(new_x,new_y,new_level))
   {
    //map_window->moveMap(new_x,new_y,new_level);
    map_window->centerMapOnActor(actor);
   } 
}

void Player::moveLeft()
{
 moveRelative(-1,0);
}

void Player::moveRight()
{
 moveRelative(1,0);
}

void Player::moveUp()
{
 moveRelative(0,-1);
}

void Player::moveDown()
{
 moveRelative(0,1);
}
