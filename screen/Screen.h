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
#include <string>

#include <SDL.h>

#include "U6def.h"
#include "Configuration.h"
#include "Surface.h"
#include "Scale.h"

class Screen
{
 Configuration *config;
 SDL_Surface *sdl_surface;
 RenderSurface *surface;
 
 ScalerRegistry		scaler_reg;		// Scaler Registry
 const ScalerStruct	*scaler;		// Scaler
 int scaler_index;	// Index of Current Scaler
 int scale_factor;	// Scale factor

 bool fullscreen;
 bool doubleBuffer;
 
 uint8 palette[768];
 uint16 width;
 uint16 height;
 
 public:
   Screen(Configuration *cfg);
   ~Screen();

   bool init(uint16 width, uint16 height);
   
   bool set_palette(uint8 *palette);
   bool rotate_palette(uint8 pos, uint8 length);
   bool clear(uint16 x, uint16 y, sint16 w, sint16 h,SDL_Rect *clip_rect=NULL);
   void *get_pixels();
   uint16 get_pitch();
   uint16 get_bpp();

   bool fill(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);
      
   bool blit(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans=false, SDL_Rect *clip_rect=NULL);     
   void update();
   void update(uint16 x, uint16 y, uint16 w, uint16 h);
   
   void lock();
   void unlock();
   
   bool initScaler();

protected:

   bool fill16(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);
   
   bool fill32(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);

inline bool Screen::blit16(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans);

inline bool Screen::blit32(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans);

void set_screen_mode();
bool try_scaler(int w, int h, uint32 flags, int hwdepth);
};


#endif /* __Screen_h__ */

