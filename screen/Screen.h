#ifndef __Screen_h__
#define __Screen_h__

/*
 *  Screen.h
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

#include <SDL.h>

#include "U6def.h"
#include "Configuration.h"
#include "Surface.h"

class Screen: Surface
{
 Configuration *config;
 SDL_Surface *scaled_surface;
 
 public:
   Screen(Configuration *cfg);
   ~Screen();

   bool init(uint16 width = 320, uint16 height = 200);
   
   virtual bool set_palette(SDL_Color *palette);
   virtual bool clear(uint8 color = 0);
   virtual void *get_pixels();
   virtual uint16 get_pitch();
   virtual uint16 get_bpp();
      
   virtual bool blit(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans=false);     
   void update();
   
   void lock();
   void unlock();
   
   bool initScaler();

};


#endif /* __Screen_h__ */

