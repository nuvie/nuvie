/*
 *  GameSelect.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu May 29 2003.
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

#include "Configuration.h"
#include "Screen.h"
#include "Game.h"

class GameSelect
{
 protected:
 
 Configuration *config;
 Screen *screen;
 Game *game;
 
 public:
 
 GameSelect(Configuration *cfg);
 ~GameSelect();
 
 uint8 load(Screen *s, uint8 game_type=NUVIE_GAME_NONE);
 
};
