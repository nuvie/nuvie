/*
 *  nuvie.cpp
 *  Nuvie
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

#include "Actor.h"
#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "NuvieIOFile.h"
#include "Screen.h"
#include "Script.h"
#include "Game.h"
#include "GameSelect.h"
#include "GUI.h"
#include "Console.h"

#include "nuvie.h"

Nuvie::Nuvie()
{
 config = NULL;
 screen = NULL;
 script = NULL;
 game = NULL;
}

Nuvie::~Nuvie()
{
 if(config != NULL)
   delete config;

 if(screen != NULL)
   delete screen;

 if(script != NULL)
    delete script;

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
    DEBUG(0,LEVEL_ERROR,"No config file found!\n");
    return false;
   }

 //load SDL screen and scaler if selected.
 screen = new Screen(config);
 if(screen->init(320,200) == false)
   {
    DEBUG(0,LEVEL_ERROR,"Initializing screen!\n");
    return false;
   }

 SDL_WM_SetCaption("Nuvie","Nuvie");

 GUI *gui = new GUI(config, screen);
 ConsoleInit(config, screen, gui, 320, 200);
 ConsoleAddInfo("\n Nuvie: ver 0.1 rev 1173 \n");
 ConsoleAddInfo("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
 ConsoleAddInfo("Config file: \"" + config->filename() + "\"");
 game_select = new GameSelect(config);

 // select game from graphical menu if required
 game_type = game_select->load(screen,game_type);
 delete game_select;
 if(game_type == NUVIE_GAME_NONE)
   return false;


 //setup various game related config variables.
 assignGameConfigValues(game_type);

 //check for a valid path to the selected game.
 if(checkGameDir(game_type) == false)
   return false;

 script = new Script(config, game_type);
 game = new Game(config, script, gui);

 if(game->loadGame(screen,game_type) == false)
   {
    delete game;
    return false;
   }

 ConsoleDelete();

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

 char *home_env = getenv("HOME");
 if(home_env != NULL)
 {
	 config_path.assign(home_env);
	// config_path.append(U6PATH_DELIMITER);
	 config_path.append("/.nuvierc");

	 if(loadConfigFile(config_path))
	   return true;

#ifdef MACOSX
	 config_path.assign(home_env);
	 config_path.append("/Library/Preferences/Nuvie Preferences");

	 if(loadConfigFile(config_path))
	   return true;
#endif

 }
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

#ifdef MACOSX
 if(home_env != NULL)
 {
	 if(initDefaultConfigMacOSX(home_env))
		 return true;
 }
#endif

#ifdef WIN32
	 if(initDefaultConfigWin32())
		 return true;
#else //Unix
	 if(home_env != NULL && initDefaultConfigUnix(home_env))
		 return true;
#endif

 delete config;
 config = NULL;

 return false;
}

bool Nuvie::initDefaultConfigWin32()
{
	const unsigned char cfg_stub[] = "<config></config>";
	std::string config_path("./nuvie.cfg");

	NuvieIOFileWrite cfgFile;

	if(cfgFile.open(config_path) == false)
		return false;

	cfgFile.writeBuf(cfg_stub, sizeof(cfg_stub));
	cfgFile.close();

	if(loadConfigFile(config_path, NUVIE_CONF_READWRITE) == false)
		return false;

	config->set("config/loadgame", "ultima6");
	config->set("config/datadir", "./data");

	config->set("config/video/scale_method", "point");
	config->set("config/video/scale_factor", "2");

	config->set("config/audio/enabled", true);
	config->set("config/audio/enable_music", true);
	config->set("config/audio/enable_sfx", true);
	config->set("config/audio/music_volume", 100);
	config->set("config/audio/sfx_volume", 255);

	config->set("config/general/lighting", "original");
	config->set("config/general/dither_mode", "none");
	config->set("config/general/enable_cursors", false);
	config->set("config/general/show_console", true);

	config->set("config/ultima6/gamedir", "c:\\ultima6");
	config->set("config/ultima6/townsdir", "c:\\fmtownsU6");
	config->set("config/ultima6/savedir", "./u6_save");
	config->set("config/ultima6/sfxdir", "./custom_sfx");
	config->set("config/ultima6/music", "native");
	config->set("config/ultima6/sfx", "pcspeaker");
	config->set("config/ultima6/skip_intro", false);
	config->set("config/ultima6/show_eggs", true);

	config->write();

	return true;
}

bool Nuvie::initDefaultConfigMacOSX(const char *home_env)
{
	const unsigned char cfg_stub[] = "<config></config>";
	std::string config_path;
   std::string home(home_env);
	config_path = home;
	config_path.append("/Library/Preferences/Nuvie Preferences");

	NuvieIOFileWrite cfgFile;

	if(cfgFile.open(config_path) == false)
		return false;

	cfgFile.writeBuf(cfg_stub, sizeof(cfg_stub));
	cfgFile.close();

	if(loadConfigFile(config_path, NUVIE_CONF_READWRITE) == false)
		return false;

	config->set("config/loadgame", "ultima6");
	config->set("config/datadir", "./data");

	config->set("config/video/scale_method", "point");
	config->set("config/video/scale_factor", "2");

	config->set("config/audio/enabled", true);
	config->set("config/audio/enable_music", true);
	config->set("config/audio/enable_sfx", true);
	config->set("config/audio/music_volume", 100);
	config->set("config/audio/sfx_volume", 255);

	config->set("config/general/lighting", "original");
	config->set("config/general/dither_mode", "none");
	config->set("config/general/enable_cursors", false);
	config->set("config/general/show_console", true);

	config->set("config/ultima6/gamedir", "/Library/Application Support/Nuvie Support/ultima6");
	config->set("config/ultima6/townsdir", "/Library/Application Support/Nuvie Support/townsU6");
	config->set("config/ultima6/savedir", home + "/Library/Application Support/Nuvie/savegames");
	config->set("config/ultima6/sfxdir", home + "/Library/Application Support/Nuvie/custom_sfx");
	config->set("config/ultima6/music", "native");
	config->set("config/ultima6/sfx", "pcspeaker");
	config->set("config/ultima6/skip_intro", false);
	config->set("config/ultima6/show_eggs", true);

	config->write();

	return true;
}

bool Nuvie::initDefaultConfigUnix(const char *home_env)
{
	const unsigned char cfg_stub[] = "<config></config>";
	std::string config_path;
   std::string home(home_env);
	config_path = home;
	config_path.append("/.nuvierc");

	NuvieIOFileWrite cfgFile;

	if(cfgFile.open(config_path) == false)
		return false;

	cfgFile.writeBuf(cfg_stub, sizeof(cfg_stub));
	cfgFile.close();

	if(loadConfigFile(config_path, NUVIE_CONF_READWRITE) == false)
		return false;

	config->set("config/loadgame", "ultima6");
	config->set("config/datadir", "./data");

	config->set("config/video/scale_method", "point");
	config->set("config/video/scale_factor", "2");

	config->set("config/audio/enabled", true);
	config->set("config/audio/enable_music", true);
	config->set("config/audio/enable_sfx", true);
	config->set("config/audio/music_volume", 100);
	config->set("config/audio/sfx_volume", 255);

	config->set("config/general/lighting", "original");
	config->set("config/general/dither_mode", "none");
	config->set("config/general/enable_cursors", false);
	config->set("config/general/show_console", true);

	config->set("config/ultima6/gamedir", "./ultima6");
	config->set("config/ultima6/townsdir", "./townsU6");
	config->set("config/ultima6/savedir", home + "/.nuvie/savegames");
	config->set("config/ultima6/sfxdir", home + "/.nuvie/custom_sfx");
	config->set("config/ultima6/music", "native");
	config->set("config/ultima6/sfx", "pcspeaker");
	config->set("config/ultima6/skip_intro", false);
	config->set("config/ultima6/show_eggs", true);

	config->write();

	return true;
}

bool Nuvie::loadConfigFile(std::string filename, bool readOnly)
{
 struct stat sb;
 DEBUG(0,LEVEL_INFORMATIONAL,"Loading Config from '%s': ", filename.c_str());

 if(stat(filename.c_str(),&sb) == 0)
  {
    if(config->readConfigFile(filename,"config", readOnly) == true)
      {
       DEBUG(1,LEVEL_INFORMATIONAL,"Done.\n");
       return true;
      }
  }

 DEBUG(1,LEVEL_INFORMATIONAL,"Failed.\n", filename.c_str());
 DEBUG(0,LEVEL_ERROR,"Failed to load config from '%s'.\n", filename.c_str());
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
 std::string path;

 config_get_path(config, "", path);
 ConsoleAddInfo("gamedir: \"" + path + "\"");

#ifndef WIN32
 struct stat sb;

 if(stat(path.c_str(),&sb) == 0 && sb.st_mode & S_IFDIR)
  {   
   return true;
  }

 ConsoleAddError("Cannot open gamedir!");
 ConsoleAddError("\"" + path + "\"");

 return false;
#endif

 return true;
}
