/*
 *  Screen.cpp
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

#include "Screen.h"

Screen::Screen(uint16 new_width, uint16 new_height)
{
 width = new_width;
 height = new_height;
 
  	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
   return;
	}

 surface = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE | SDL_HWPALETTE);
 
 printf("surface pitch = %d\n",surface->pitch);
 
}
 
Screen::~Screen()
{
 SDL_Quit();
}
  
BOOL Screen::set_palette(SDL_Color *palette)
{

 SDL_SetColors(surface,palette,0,256);
  
 return TRUE;
}

BOOL Screen::clear(uint8 color)
{
 SDL_FillRect(surface, NULL, (uint32)color);

 return TRUE; 
}

void *Screen::get_pixels()
{
 if(surface == NULL)
    return NULL; 
 
 return surface->pixels;
}

uint16 Screen::get_pitch()
{
 return (uint16)surface->pitch;
}

uint16 Screen::get_bpp()
{
 return (uint16)surface->format->BitsPerPixel;
}

bool Screen::blit(unsigned char *buf, uint16 bpp, uint16 x, uint16 y, uint16 width, uint16 height, bool trans)
{
 unsigned char *pixels;
 uint16 pitch;
 uint16 i,j;
 
 pixels = (unsigned char *)surface->pixels;
 pitch = surface->pitch;
 
 pixels += y * pitch + x;

 if(trans)
  {
   for(i=0;i<height;i++)
     {
      for(j=0;j<width;j++)
        {
         if(buf[j] != 0xff)
           pixels[j] = buf[j];
        }
      buf += width;
      pixels += pitch;
     }
  }
 else
  {
   for(i=0;i<height;i++)
     {
      memcpy(pixels,&buf[i*width],width);
      pixels += pitch;
     }
  }

 return true;
}

void Screen::update()
{
 if(surface)
   SDL_UpdateRect(surface,0,0,0,0);

 return;
}

void Screen::lock()
{
 SDL_LockSurface(surface);
 
 return;
}

void Screen::unlock()
{
 SDL_UnlockSurface(surface);
 
 return;
}
