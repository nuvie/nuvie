/*
 *  misc.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Sat Jun 14 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "U6misc.h"

void config_get_path(Configuration *config, std::string filename, std::string &path)
{
 std::string key, game_name;
 
 config->value("config/GameName",game_name);
 
 key.assign("config/");
 key.append(game_name);
 key.append("/gamedir");
    
 config->pathFromValue(key, filename, path);
 
 return;
}
