#ifndef __Screen_h__
#define __Screen_h__

/*
 *  Screen.h
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

#include "SDL.h"

#include "Game.h"
#include "Surface.h"
#include "Scale.h"

#define LIGHTING_STYLE_NONE 0
#define LIGHTING_STYLE_SMOOTH 1
#define LIGHTING_STYLE_ORIGINAL 2

class Configuration;

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
 SDL_Rect *update_rects;
 uint16 num_update_rects;
 uint16 max_update_rects;

 SDL_Rect shading_rect;
 uint8 *shading_data;
 uint8 *shading_globe[6];
 uint8 shading_ambient;
 uint8 *shading_tile[4];

 public:
   Screen(Configuration *cfg);
   ~Screen();

   bool init();

   bool set_palette(uint8 *palette);
   bool set_palette_entry(uint8 idx, uint8 r, uint8 g, uint8 b);
   bool rotate_palette(uint8 pos, uint8 length);
   bool clear(sint16 x, sint16 y, sint16 w, sint16 h,SDL_Rect *clip_rect=NULL);
   void *get_pixels();
   const unsigned char *get_surface_pixels() { return(surface->get_pixels()); }
   uint16 get_pitch();
   SDL_Surface *create_sdl_surface_from(unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch);
   SDL_Surface *create_sdl_surface_8(unsigned char *src_buf, uint16 src_w, uint16 src_h);
   uint16 get_bpp();
   int get_scale_factor() { return scale_factor; }
   SDL_Surface *get_sdl_surface();
   uint16 get_width() { return width; }
   uint16 get_height() { return height; }
   uint16 get_translated_x(uint16 x);
   uint16 get_translated_y(uint16 y);

   bool fill(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);
   void fade(uint16 dest_x, uint16 dest_y, uint16 src_w, uint16 src_h, uint8 opacity, uint8 fade_bg_color=0);
   void stipple_8bit(uint8 color_num);
   void stipple_8bit(uint8 color_num, uint16 x, uint16 y, uint16 w, uint16 h);
   void put_pixel(uint8 colour_num, uint16 x, uint16 y);

   bool blit(sint32 dest_x, sint32 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans=false, SDL_Rect *clip_rect=NULL, uint8 opacity=255);
   void blitbitmap(uint16 dest_x, uint16 dest_y, const unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color);

   void buildalphamap8();
   void clearalphamap8( uint16 x, uint16 y, uint16 w, uint16 h, uint8 opacity );
   void drawalphamap8globe( sint16 x, sint16 y, uint16 radius );
   void blitalphamap8(sint16 x, sint16 y, SDL_Rect *clip_rect);
   bool updatingalphamap;
   int get_lighting_style() { return lighting_style; }
   int lighting_style;

   uint8 get_ambient() { return shading_ambient; }
   void set_ambient( uint8 ambient ) { shading_ambient = ambient; }

   void update();
   void update(sint32 x, sint32 y, uint16 w, uint16 h);
   void preformUpdate();
   bool should_update_alphamap() { return updatingalphamap; }
   void lock();
   void unlock();

   bool initScaler();

   unsigned char *copy_area(SDL_Rect *area = NULL, unsigned char *buf = NULL);
   unsigned char *copy_area(SDL_Rect *area, uint16 down_scale);

   void restore_area(unsigned char *pixels, SDL_Rect *area = NULL, unsigned char *target = NULL, SDL_Rect *target_area = NULL, bool free_src = true);

   void draw_line (int sx, int sy, int ex, int ey, uint8 color);

protected:

   bool fill16(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);

   bool fill32(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h);

   void fade16(uint16 dest_x, uint16 dest_y, uint16 src_w, uint16 src_h, uint8 opacity, uint8 fade_bg_color);
   void fade32(uint16 dest_x, uint16 dest_y, uint16 src_w, uint16 src_h, uint8 opacity, uint8 fade_bg_color);

   inline uint16 blendpixel16(uint16 p, uint16 p1, uint8 opacity);
   inline uint32 blendpixel32(uint32 p, uint32 p1, uint8 opacity);

inline bool blit16(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans);
inline bool blit16WithOpacity(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans, uint8 opacity);

inline bool blit32(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans);
inline bool blit32WithOpacity(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans, uint8 opacity);

inline void blitbitmap16(uint16 dest_x, uint16 dest_y, const unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color);

inline void blitbitmap32(uint16 dest_x, uint16 dest_y, const unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color);

   unsigned char *copy_area16(SDL_Rect *area, uint16 down_scale);
   unsigned char *copy_area32(SDL_Rect *area, uint16 down_scale);

   unsigned char *copy_area16(SDL_Rect *area, unsigned char *buf);
   unsigned char *copy_area32(SDL_Rect *area, unsigned char *buf);
   void restore_area16(unsigned char *pixels, SDL_Rect *area, unsigned char *target = NULL, SDL_Rect *target_area = NULL, bool free_src = true);
   void restore_area32(unsigned char *pixels, SDL_Rect *area, unsigned char *target = NULL, SDL_Rect *target_area = NULL, bool free_src = true);

void set_screen_mode();
bool try_scaler(int w, int h, uint32 flags, int hwdepth);
};


#endif /* __Screen_h__ */

