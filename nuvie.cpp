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
#include "U6misc.h"
#include "Screen.h"
#include "Game.h"
#include "GameSelect.h"

#include "nuvie.h"

Nuvie::Nuvie()
{
 config = NULL;
 screen = NULL;
 game = NULL;
}

Nuvie::~Nuvie()
{
 if(config != NULL)
   delete config;

 if(screen != NULL)
   delete screen;
   
 if(game != NULL)
   delete game;
}


bool Nuvie::init(int argc, char **argv)
{
 GameSelect *game_select;
 uint8 game_type;
 
 if(argc > 1)
   game_type = get_game_type(argv[1]);
 else
   game_type = NUVIE_GAME_NONE;

 //find and load config file
 if(initConfig() == false)
   {
    printf("Error: No config file found!\n");
    return false;
   }

 //load SDL screen and scaler if selected.
 screen = new Screen(config);
 if(screen->init(320,200) == false)
   {
    printf("Error: Initializing screen!\n");
    return false;
   }

 SDL_WM_SetCaption("Nuvie","Nuvie");
 
 game_select = new GameSelect(config);
 
 // select game from graphical menu if required
 game_type = game_select->load(screen,game_type);
 delete game_select;
 if(game_type == NUVIE_GAME_NONE)
   return false;
 
 //setup various game related config variables.
 assignGameConfigValues(game_type);
 
 //check for a vaild path to the selected game.
 if(checkGameDir(game_type) == false)
   return false;

 game = new Game(config);
 
 if(game->loadGame(screen,game_type) == false)
   {
    delete game;
    return false;
   }

 return true;
}

bool Nuvie::play()
{
 if(game)
  game->play();

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
 printf("Loading Config from '%s': ", filename.c_str());

 if(stat(filename.c_str(),&sb) == 0)
  {
    if(config->readConfigFile(filename,"config") == true)
      {
       printf("Done.\n");
       return true;
      }
  }

 printf("Failed.\n");
 return false;
}

void Nuvie::assignGameConfigValues(uint8 game_type)
{
 std::string game_name, game_id;
 
 config->set("config/GameType",game_type);

 switch(game_type)
  {
   case NUVIE_GAME_U6 : game_name.assign("ultima6");
                        game_id.assign("u6");
                        break;
   case NUVIE_GAME_MD : game_name.assign("martian");
                        game_id.assign("md");
                        break;
   case NUVIE_GAME_SE : game_name.assign("savage");
                        game_id.assign("se");
                        break;
  }

 config->set("config/GameName",game_name);
 config->set("config/GameID",game_id);
 
 return;
}

bool Nuvie::checkGameDir(uint8 game_type)
{
 struct stat sb;
 std::string path;

#ifndef WIN32 
 config_get_path(config, "", path);

 if(stat(path.c_str(),&sb) == 0 && sb.st_mode & S_IFDIR)
  {
   return true;
  }

 printf("Error: Invalid gamedir! '%s'\n", path.c_str());
 
 return false;
#endif

 return true;
}
