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
 
#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "U6def.h"
#include "Configuration.h"
#include "Screen.h"
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
  
 if(initConfig() == false)
   return false;
   
 screen = new Screen(config);
 if(screen->init(320,200) == false)
   {
    printf("Error: initializing screen\n");
    return false;
   }

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


bool Nuvie::initConfig()
{
 std::string config_path;

 
 config = new Configuration();
 
 // ~/.nuvierc
 
 config_path.assign(getenv("HOME"));
// config_path.append(U6PATH_DELIMITER);
 config_path.append("/.nuvierc");
  
 if(loadConfigFile(config_path))
   return true;

 // nuvie.cfg in the working dir
 
 config_path.assign("nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;

 // standard share locations
 
 config_path.assign("/usr/local/share/nuvie/nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;

 config_path.assign("/usr/share/nuvie/nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;

 delete config;
 
 return false;
}

bool Nuvie::loadConfigFile(std::string filename)
{
 struct stat sb;
  
 // FIX! need to add support for finding nuvie.cfg file.
 if(stat(filename.c_str(),&sb) == 0)
  {
    if(config->readConfigFile(filename,"config") == true)
      {
       return true;
      }
  }
  
 return false;
}
