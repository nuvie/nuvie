/*
 *  GameSelect.cpp
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
#include "U6misc.h"
#include "GameSelect.h"

GameSelect::GameSelect(Configuration *cfg)
{
 config = cfg;
}

GameSelect::~GameSelect()
{
 
}

uint8 GameSelect::load(Screen *s, uint8 game_type)
{
 std::string cfg_game_string;
 
 screen = s;
 
 if(game_type == NUVIE_GAME_NONE) // if game_type not specified on cmdline
   {
    config->value("config/loadgame",cfg_game_string,""); // attempt to get game_type_string from config
    game_type = get_game_type(cfg_game_string.c_str());
    
    if(game_type == NUVIE_GAME_NONE)
      game_type = NUVIE_GAME_U6; // FIX we should select game from a menu here.
   }
 
 return game_type;
}
