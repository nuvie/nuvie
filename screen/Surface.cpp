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

Surface::Surface()
{
 width = 0;
 height = 0;
 
 pitch = 0;
 bpp = 0;
}

Surface::~Surface()
{
 if(data != NULL)
   free(data);

}

bool Surface::init(uint16 new_width, uint16 new_height)
{
 width = new_width;
 height = new_height;
 pitch = width;
 bpp = 8;
 
 data = (unsigned char *)malloc(width * height);
 if(data == NULL)
   {
    printf("Argh! out of memory. Surface::init()\n");
    return false;
   }
   
 return true; 
}


bool Surface::set_palette(SDL_Color *palette)
{

 return true;
}

bool Surface::clear(uint8 color)
{
 memset(data,color, width * height);
 
 return true; 
}

void *Surface::get_pixels()
{
 return data;
}

uint16 Surface::get_pitch()
{
 return (uint16)pitch;
}

uint16 Surface::get_bpp()
{
 return bpp;
}

bool Surface::blit(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans)
{
 unsigned char *pixels;
 uint16 i,j;

 pixels = data;
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

void Surface::update()
{
  return;
}

void Surface::lock()
{
 return;
}

void Surface::unlock()
{
 return;
}
