/*
 *  GamePalette.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "U6def.h"
#include "U6File.h"
#include "Configuration.h"
#include "Screen.h"
#include "GamePalette.h"

GamePalette::GamePalette(Screen *s, Configuration *cfg)
{
 screen = s;
 config = cfg;
 
 palette = (uint8 *)malloc(768);
 memset(palette, 0, 768);
 
 this->loadPalette();
 
 screen->set_palette(palette);
 
 counter = 0;
}

GamePalette::~GamePalette()
{
 free(palette);
}


bool GamePalette::loadPalette()
{
 uint16 i,j;
 std::string filename;
 U6File file;
 unsigned char *buf;
 uint8 *pal_ptr;
 std::string key, game_name, game_id, pal_name;
 
 config->value("config/GameName",game_name);
 config->value("config/GameID",game_id);

 pal_name.assign(game_id);
 pal_name.append("pal");
 
 key.assign("config/");
 key.append(game_name);
 key.append("/gamedir");
    
 config->pathFromValue(key,pal_name,filename);
 
 if(file.open(filename,"rb") == false)
  {
   printf("Error: loading palette.\n");
   return false;
  }
 
 buf = file.readFile();
 
 pal_ptr = palette;
 
 for(i=0,j=0;i<256;i++,j+=3)
  {
   pal_ptr[0] = buf[j]<<2;
   pal_ptr[1] = buf[j+1]<<2;
   pal_ptr[2] = buf[j+2]<<2;
   pal_ptr += 3;
  }

 free(buf);
 
 return true;
}

void GamePalette::rotatePalette()
{
 screen->rotate_palette(0xe0,8); // Fires, braziers, candles
 screen->rotate_palette(0xe8,8); // BluGlo[tm] magical items

 if(counter == 0)
  {
   screen->rotate_palette(0xf0,4); // 
   screen->rotate_palette(0xf4,4); // Kitchen Cauldrons
   screen->rotate_palette(0xfb,4); // 
   counter = 1;
  }
 else
  counter = 0;

}
