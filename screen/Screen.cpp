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

#include "Surface.h"

#include "Screen.h"

Screen::Screen()
{
}

Screen::~Screen()
{
 SDL_Quit();
}

bool Screen::init(uint16 new_width, uint16 new_height)
{
 width = new_width;
 height = new_height;
 
  	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
   return false;
	}

 surface = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE | SDL_HWPALETTE);
 pitch = surface->pitch;
 bpp = surface->format->BitsPerPixel;
 
 printf("surface pitch = %d\n",pitch);
 
 return true;
}

bool Screen::set_palette(SDL_Color *palette)
{

 SDL_SetColors(surface,palette,0,256);
  
 return true;
}

bool Screen::clear(uint8 color)
{
 SDL_FillRect(surface, NULL, (uint32)color);

 return true; 
}

void *Screen::get_pixels()
{
 if(surface == NULL)
    return NULL; 
 
 return surface->pixels;
}

bool Screen::blit(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans)
{
 unsigned char *pixels;
 uint16 i,j;
 
 pixels = (unsigned char *)surface->pixels;
 
 pixels += dest_y * pitch + dest_x;

 if(trans)
  {
   for(i=0;i<src_h;i++)
     {
      for(j=0;j<src_w;j++)
        {
         if(src_buf[j] != 0xff)
           pixels[j] = src_buf[j];
        }
      src_buf += src_pitch;
      pixels += pitch;
     }
  }
 else
  {
   for(i=0;i<src_h;i++)
     {
      memcpy(pixels,src_buf,src_w);
      src_buf += src_pitch;
      pixels += pitch;
     }
  }

 return true;
}

uint16 Screen::get_pitch()
{
 return (uint16)pitch;
}

uint16 Screen::get_bpp()
{
 return bpp;
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
