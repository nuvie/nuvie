/*
 *  GamePalette.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "U6def.h"
#include "U6File.h"
#include "Screen.h"
#include "GamePalette.h"

GamePalette::GamePalette(Screen *s)
{
 screen = s;
 
 palette = (SDL_Color *)malloc(sizeof(SDL_Color) * 256);
 memset(palette, 0, sizeof(SDL_Color) * 256);
 
 this->loadPalette();
 
 screen->set_palette(palette);
 
}

GamePalette::~GamePalette()
{
 free(palette);
}


void GamePalette::loadPalette()
{
 uint16 i,j;
 U6File file;
 unsigned char *buf;
 SDL_Color *pal_ptr;
 
 file.open("/Users/eric/Projects/Ultima6/nuvie/ultima6/u6pal","r");
 
 buf = file.readFile();
 
 pal_ptr = palette;
 
 for(i=0,j=0;i<256;i++,j+=3)
  {
   palette[i].r = buf[j]<<2;
   palette[i].g = buf[j+1]<<2;
   palette[i].b = buf[j+2]<<2;
   pal_ptr += sizeof(SDL_Color);
  }

 free(buf);
 //delete file;
}

void GamePalette::rotatePalette(uint8 pos, uint8 length, uint8 amount)
{
 //SDL_Color color;
 //for
 //color = 
}
