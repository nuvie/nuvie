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

//return the uint8 game_type from a char string
uint8 get_game_type(const char *string)
{
 if(string != NULL && strlen(string) >= 2)
   {
    if(strcmp("md",string) == 0 || strcmp("martian",string) == 0)
      return NUVIE_GAME_MD;
    if(strcmp("se",string) == 0 || strcmp("savage",string) == 0)
      return NUVIE_GAME_SE;
    if(strcmp("u6",string) == 0 || strcmp("ultima6",string) == 0)
      return NUVIE_GAME_U6;
   } 

 return NUVIE_GAME_NONE;
}
