/*
 *  nuvie.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Sun Mar 09 2003.
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
 
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "U6def.h"

#include "Configuration.h"

#include "Screen.h"
#include "GamePalette.h"
#include "U6Bmp.h"
#include "TileManager.h"
#include "Text.h"

#include "Game.h"

#include "nuvie.h"

Nuvie::Nuvie()
{
}

Nuvie::~Nuvie()
{
 delete config;
 delete screen;
 delete game;
}

bool Nuvie::init()
{
  
 config = new Configuration();
 
 // FIX! need to add support for finding nuvie.cfg file.
 
 if(config->readConfigFile("nuvie.cfg","config") == false)
   return false;
  
 screen = new Screen(config);
 if(screen->init(320,200) == false)
   return false;
 
 SDL_WM_SetCaption("Nuvie","Nuvie");
 
 game = new Game(config);
 
 game->loadGame(screen,NUVIE_GAME_ULTIMA6);
 
 game->play();
   
 return true;
}

bool Nuvie::play()
{
 

 return true;
}
