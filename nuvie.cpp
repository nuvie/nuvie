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
#include "GameSelect.h"

#include "nuvie.h"

Nuvie::Nuvie()
{
 config = NULL;
 screen = NULL;
}

Nuvie::~Nuvie()
{
 if(config != NULL)
   delete config;
 if(screen != NULL)
 delete screen;
 //delete game;
}


bool Nuvie::init()
{
 GameSelect *game_select;
 
 if(initConfig() == false)
   return false;
   
 screen = new Screen(config);
 if(screen->init(320,200) == false)
   {
    printf("Error: initializing screen\n");
    return false;
   }

 SDL_WM_SetCaption("Nuvie","Nuvie");
 
 game_select = new GameSelect(config);
 
 game_select->load(screen);
 
 
 delete game_select;
 
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
 
#ifndef WIN32
 // ~/.nuvierc
 
 config_path.assign(getenv("HOME"));
// config_path.append(U6PATH_DELIMITER);
 config_path.append("/.nuvierc");
  
 if(loadConfigFile(config_path))
   return true;
#endif

 // nuvie.cfg in the working dir
 
 config_path.assign("nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;

#ifndef WIN32
 // standard share locations
 
 config_path.assign("/usr/local/share/nuvie/nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;

 config_path.assign("/usr/share/nuvie/nuvie.cfg");
 
 if(loadConfigFile(config_path))
   return true;
#endif 

 delete config;
 config = NULL;
 
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
