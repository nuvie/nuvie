#ifndef __Nuvie_h__
#define __Nuvie_h__

/*
 *  nuvie.h
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

#include <string>
#include "Configuration.h"

class Screen;
class Script;
class Game;

class Nuvie
{
 Configuration *config;
 Screen *screen;
 Script *script;
 Game *game;

 public:

   Nuvie();
   ~Nuvie();


   bool init(int argc, char **argv);
   bool play();

   protected:

   bool initConfig();
   bool initDefaultConfigMacOSX(const char *home_env);
   bool loadConfigFile(std::string filename, bool readOnly = NUVIE_CONF_READONLY);
   void assignGameConfigValues(uint8 game_type);
   bool checkGameDir(uint8 game_type);
};


#endif /* __Nuvie_h__ */

