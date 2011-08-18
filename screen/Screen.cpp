/*
 *  Screen.cpp
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
#include <iostream>
#include <cmath>
#include "SDL.h"

#include "nuvieDefs.h"
#include "Configuration.h"

#include "Surface.h"
#include "Scale.h"
#include "Screen.h"
#include "MapWindow.h"

#define sqr(a) ((a)*(a))

//Ultima 6 light globe sizes.

static const uint8 globeradius[]   = { 64, 96, 160, 224, 240, 255 };
static const uint8 globeradius_2[] = { 32, 48,  80, 112, 120, 127 };

Screen::Screen(Configuration *cfg)
{
 std::string str_lighting_style;
 config = cfg;

 update_rects = NULL;
 shading_data = NULL;
 updatingalphamap = true;
 std::string x;
 config->value( "config/general/lighting", str_lighting_style );

 if( str_lighting_style == "none" )
	 lighting_style = 0;
 else if( str_lighting_style == "smooth" )
	 lighting_style = 1;
 else
	 lighting_style = 2;

 max_update_rects = 10;
 num_update_rects = 0;
 memset( shading_globe, 0, sizeof(uint8*) * 3 );
}

Screen::~Screen()
{
 delete surface;
 if (update_rects) free(update_rects);
 if (shading_data) free(shading_data);

 for( int i = 0; i < 6; i++ )
   {
    if(shading_globe[i])
       free(shading_globe[i]);
   }

 SDL_Quit();
}

bool Screen::init(uint16 new_width, uint16 new_height)
{
 std::string str;

 width = new_width;
 height = new_height;

  	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		DEBUG(0,LEVEL_EMERGENCY, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
   return false;
	}

 update_rects = (SDL_Rect *)malloc(sizeof(SDL_Rect) * max_update_rects);
 if(update_rects == NULL)
   return false;

 config->value("config/video/scale_method", str, "---");
 scaler_index = scaler_reg.GetIndexForName(str);
 if(scaler_index == -1)
   {
		//config.set("config/video/scale_method","SuperEagle",true);
		scaler_index = scaler_reg.GetIndexForName("SuperEagle");
	 }

config->value("config/video/scale_factor", scale_factor, 1);

fullscreen = false;

 //scaled_surface = SDL_SetVideoMode(width, height, 8, SDL_SWSURFACE | SDL_HWPALETTE);
 //pitch = scaled_surface->pitch;
 //bpp = scaled_surface->format->BitsPerPixel;

 //DEBUG(0,LEVEL_DEBUGGING,"surface pitch = %d\n",pitch);

// memcpy(palette,0,768);

 set_screen_mode();

 return true;
}


bool Screen::set_palette(uint8 *p)
{
 if(surface == NULL || p == NULL)
   return false;

 //SDL_SetColors(scaled_surface,palette,0,256);
 for (int i = 0; i < 256; ++i)
   {
		uint32  r = p[i*3];
		uint32  g = p[i*3+1];
		uint32  b = p[i*3+2];

		uint32	c= ((r>>RenderSurface::Rloss)<<RenderSurface::Rshift) | ((g>>RenderSurface::Gloss)<<RenderSurface::Gshift) | ((b>>RenderSurface::Bloss)<<RenderSurface::Bshift);

		surface->colour32[i] = c;
	 }

 return true;
}

bool Screen::rotate_palette(uint8 pos, uint8 length)
{
 uint32 tmp_colour;
 uint8 i;

 tmp_colour = surface->colour32[pos+length-1];

 for(i= length-1;i > 0; i-- )
    surface->colour32[pos + i] = surface->colour32[pos + i - 1];

 surface->colour32[pos] = tmp_colour;

 return true;
}

bool Screen::clear(uint16 x, uint16 y, sint16 w, sint16 h,SDL_Rect *clip_rect)
{
 uint8 *pixels;
 uint16 i;
 uint16 x1,y1;

 pixels = (uint8 *)surface->pixels;

 if(clip_rect)
   {
    x1 = x;
    y1 = y;
    if(x < clip_rect->x)
      x = clip_rect->x;

    if(y < clip_rect->y)
      y = clip_rect->y;

    if(x1 + w > clip_rect->x + clip_rect->w)
      {
       w -= (x1 + w) - (clip_rect->x + clip_rect->w);
       if(w <= 0)
         return false;
      }

    if(y1 + h > clip_rect->y + clip_rect->h)
      {
       h -= (y1 + h) - (clip_rect->y + clip_rect->h);
       if(h <= 0)
         return false;
      }
   }

 pixels += y * surface->pitch + (x * surface->bytes_per_pixel);

 for(i=0;i<h;i++)
    {
     memset(pixels,0,w*surface->bytes_per_pixel);
     pixels += surface->pitch;
    }

 return true;
}

bool Screen::fill(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h)
{
 if(surface->bits_per_pixel == 16)
    return fill16(colour_num, x, y, w, h);

 return fill32(colour_num, x, y, w, h);
}

bool Screen::fill16(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h)
{
 uint16 *pixels;
 uint16 i,j;

 pixels = (uint16 *)surface->pixels;

 pixels += y * surface->w + x;

 for(i=0;i<h;i++)
    {
     for(j=0;j<w;j++)
        pixels[j] = (uint16)surface->colour32[colour_num];

     pixels += surface->w;
    }

 return true;
}

bool Screen::fill32(uint8 colour_num, uint16 x, uint16 y, sint16 w, sint16 h)
{
 uint32 *pixels;
 uint16 i,j;


 pixels = (uint32 *)surface->pixels;

 pixels += y * surface->w + x;

 for(i=0;i<h;i++)
    {
     for(j=0;j<w;j++)
        pixels[j] = surface->colour32[colour_num];

     pixels += surface->w;
    }

 return true;
}

void Screen::put_pixel(uint8 colour_num, uint16 x, uint16 y)
{
	if(surface->bits_per_pixel == 16)
	{
		uint16 *pixel = (uint16 *)surface->pixels + y * surface->w + x;
		*pixel = (uint16)surface->colour32[colour_num];
	}
	else
	{
		uint32 *pixel = (uint32 *)surface->pixels + y * surface->w + x;
		*pixel = (uint32)surface->colour32[colour_num];
	}
}

void *Screen::get_pixels()
{
 //if(scaled_surface == NULL)
 //   return NULL;

 //return scaled_surface->pixels;
 return NULL;
}

SDL_Surface *Screen::get_sdl_surface()
{
 if(surface)
   return surface->get_sdl_surface();

 return NULL;
}

bool Screen::blit(sint32 dest_x, sint32 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans, SDL_Rect *clip_rect)
{
 uint16 src_x = 0;
 uint16 src_y = 0;

 // clip to screen.

 if(dest_x >= width || dest_y >= height)
   return false;

 if(dest_x < 0)
   {
    if(dest_x + src_w <= 0)
      return false;
    else
      src_w += dest_x;
   }

 if(dest_y < 0)
   {
    if(dest_y + src_h <= 0)
      return false;
    else
      src_h += dest_y;
   }

 if(dest_x + src_w >= width)
   src_w = width - dest_x;

 if(dest_y + src_h >= height)
   src_h = height - dest_y;

 //clip to rect if required.

 if(clip_rect)
  {
   if(clip_rect->x > dest_x)
      {
       src_x = clip_rect->x - dest_x;
       if(src_x > dest_x + src_w)
         return false;
       src_w -= clip_rect->x - dest_x;
       dest_x = clip_rect->x;
      }

   if(clip_rect->y > dest_y)
     {
      src_y = clip_rect->y - dest_y;
      if(src_y > dest_y + src_h)
         return false;
      src_h -= clip_rect->y - dest_y;
      dest_y = clip_rect->y;
     }

   if(dest_x + src_w > clip_rect->x + clip_rect->w)
     {
      if(clip_rect->x + clip_rect->w - dest_x <= 0)
        return false;

      src_w = clip_rect->x + clip_rect->w - dest_x;
     }

   if(dest_y + src_h > clip_rect->y + clip_rect->h)
     {
      if(clip_rect->y + clip_rect->h - dest_y <= 0)
        return false;

      src_h = clip_rect->y + clip_rect->h - dest_y;
     }

   src_buf += src_y * src_pitch + src_x;
  }

 if(surface->bits_per_pixel == 16)
   return blit16(dest_x, dest_y, src_buf, src_bpp, src_w, src_h, src_pitch, trans);

 return blit32(dest_x, dest_y, src_buf, src_bpp, src_w, src_h, src_pitch, trans);
}

inline bool Screen::blit16(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans)
{
 uint16 *pixels;
 uint16 i,j;

 pixels = (uint16 *)surface->pixels;

 pixels += dest_y * surface->w + dest_x;

 if(trans)
  {
   for(i=0;i<src_h;i++)
     {
      for(j=0;j<src_w;j++)
        {
         if(src_buf[j] != 0xff)
           pixels[j] = (uint16)surface->colour32[src_buf[j]];
        }
      src_buf += src_pitch;
      pixels += surface->w; //pitch;
     }
  }
 else
  {
   for(i=0;i<src_h;i++)
     {
      for(j=0;j<src_w;j++)
        {
         pixels[j] = (uint16)surface->colour32[src_buf[j]];
        }
      src_buf += src_pitch;
      pixels += surface->w; //surface->pitch;
     }
  }

 return true;
}

bool Screen::blit32(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch, bool trans)
{
 uint32 *pixels;
 uint16 i,j;

 pixels = (uint32 *)surface->pixels;

 pixels += dest_y * surface->w + dest_x;


 if(trans)
  {
   for(i=0;i<src_h;i++)
     {
      for(j=0;j<src_w;j++)
        {
         if(src_buf[j] != 0xff)
           pixels[j] = surface->colour32[src_buf[j]];
        }
      src_buf += src_pitch;
      pixels += surface->w; //pitch;
     }
  }
 else
  {
   for(i=0;i<src_h;i++)
     {
      for(j=0;j<src_w;j++)
        {
         pixels[j] = surface->colour32[src_buf[j]];
        }
      src_buf += src_pitch;
      pixels += surface->w; //surface->pitch;
     }
  }

 return true;
}

void Screen::blitbitmap(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color)
{
 if(surface->bits_per_pixel == 16)
   blitbitmap16(dest_x, dest_y, src_buf, src_w, src_h, fg_color, bg_color);
 else
   blitbitmap32(dest_x, dest_y, src_buf, src_w, src_h, fg_color, bg_color);

 return;
}

void Screen::blitbitmap16(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color)
{
 uint16 *pixels;
 uint16 i,j;

 pixels = (uint16 *)surface->pixels;

 pixels += dest_y * surface->w + dest_x;

 for(i=0;i<src_h;i++)
  {
   for(j=0;j<src_w;j++)
    {
     if(src_buf[j])
       pixels[j] = (uint16)surface->colour32[fg_color];
     else
       pixels[j] = (uint16)surface->colour32[bg_color];
    }
   src_buf += src_w;
   pixels += surface->w; //surface->pitch;
  }

 return;
}

void Screen::blitbitmap32(uint16 dest_x, uint16 dest_y, unsigned char *src_buf, uint16 src_w, uint16 src_h, uint8 fg_color, uint8 bg_color)
{
 uint32 *pixels;
 uint16 i,j;

 pixels = (uint32 *)surface->pixels;

 pixels += dest_y * surface->w + dest_x;

 for(i=0;i<src_h;i++)
  {
   for(j=0;j<src_w;j++)
    {
     if(src_buf[j])
       pixels[j] = surface->colour32[fg_color];
     else
       pixels[j] = surface->colour32[bg_color];
    }
   src_buf += src_w;
   pixels += surface->w; //surface->pitch;
  }

 return;
}

//4 is pure-light
//0 is pitch-black
//Globe of r 1 is just a single tile of 2

static const char TileGlobe[][11*11] =
{
{
    1,1,1,
    1,2,1,
    1,1,1
},
{
    0,0,0,0,0,
    0,1,2,1,0,
    0,2,3,2,0,
    0,1,2,1,0,
    0,0,0,0,0
},
{
    0,0,1,1,1,0,0,
    0,1,2,2,2,1,0,
    1,2,3,3,3,2,1,
    1,2,3,4,3,2,1,
    1,2,3,3,3,2,1,
    0,1,2,2,2,1,0,
    0,0,1,1,1,0,0
},
{
    0,0,0,1,1,1,0,0,0,
    0,0,1,2,2,2,1,0,0,
    0,1,2,3,3,3,2,1,0,
    1,2,3,3,4,3,3,2,1,
    1,2,3,4,4,4,3,2,1,
    1,2,3,3,4,3,3,2,1,
    0,1,2,3,3,3,2,1,0,
    0,0,1,2,2,2,1,0,0,
    0,0,0,1,1,1,0,0,0
},
{
    0,0,1,1,2,2,2,1,1,0,0,
    0,1,1,2,2,2,2,2,1,1,0,
    1,1,2,2,3,3,3,2,2,1,1,
    1,2,2,3,4,4,4,3,2,2,1,
    2,2,3,4,4,4,4,4,3,2,2,
    2,2,3,4,4,4,4,4,3,2,2,
    2,2,3,4,4,4,4,4,3,2,2,
    1,2,2,3,4,4,4,3,2,2,1,
    1,1,2,2,3,3,3,2,2,1,1,
    0,1,1,2,2,2,2,2,1,1,0,
    0,0,1,1,2,2,2,1,1,0,0,
},
{
    1,1,2,2,2,2,2,2,2,1,1,
    1,2,2,3,3,3,3,3,2,2,1,
    2,2,3,3,4,4,4,3,3,2,2,
    2,3,3,4,4,4,4,4,3,3,2,
    2,3,4,4,4,4,4,4,4,3,2,
    2,3,4,4,4,4,4,4,4,3,2,
    2,3,4,4,4,4,4,4,4,3,2,
    2,3,3,4,4,4,4,4,3,3,2,
    2,2,3,3,4,4,4,3,3,2,2,
    1,2,2,3,3,3,3,3,2,2,1,
    1,1,2,2,2,2,2,2,2,1,1
}
};

void Screen::clearalphamap8( uint16 x, uint16 y, uint16 w, uint16 h, uint8 opacity )
{
	switch( lighting_style )
	{
	case 0:
		return;
	default:
	case 1:
		shading_ambient = opacity;
		break;
	case 2:
		if( opacity < 0xFF )
            shading_ambient = 0;
        else
            shading_ambient = 0xFF;
		break;
	}

    if( shading_data == NULL )
    {
        shading_rect.x = x;
        shading_rect.y = y;
        shading_rect.w = w;
        shading_rect.h = h;
        shading_data = (unsigned char*)malloc(sizeof(char)*shading_rect.w*shading_rect.h);
        if( shading_data == NULL )
        {
            /* We couldn't allocate memory for the opacity map, so just disable lighting */
            shading_ambient = 0xFF;
            return;
        }
        buildalphamap8();
    }
    if( shading_ambient == 0xFF )
    {
    }
    else
    {
        memset( shading_data, shading_ambient, sizeof(char)*shading_rect.w*shading_rect.h );
    }
    updatingalphamap = true;

    //Light globe around the avatar
    if( lighting_style == 2 )
        drawalphamap8globe( 5, 5, opacity/64+3 ); //range (0..3)+3 or (3..6)
    else if( lighting_style != 0 )
        drawalphamap8globe( 5, 5, 3 );
}

void Screen::buildalphamap8()
{
    //Build three globes for 6 intensities
    for( int i = 0; i < 6; i++ )
    {
        shading_globe[i] = (uint8*)malloc(sqr(globeradius[i]));
        for( int y = 0; y < globeradius[i]; y++ )
        {
            for( int x = 0; x < globeradius[i]; x++ )
            {
                float r;
                //Distance from center
                r  = sqrtf( sqr((y-globeradius_2[i]))+sqr((x-globeradius_2[i])) );
                //Unitize
                r /= sqrtf( sqr(globeradius_2[i])+sqr(globeradius_2[i]) );
                //Calculate brightness
                r  = (float)exp(-(10*r*r));
                //Fit into a byte
                r *= 255;
                //Place it
                shading_globe[i][y*globeradius[i]+x] = (uint8)r;
            }
        }
    }

    //Get the three shading tiles (for original-style dithered lighting)
    Game *game = Game::get_game();
    int game_type;
    config->value("config/GameType",game_type);

    if(game_type == NUVIE_GAME_U6 || game_type == NUVIE_GAME_SE)
    {
        shading_tile[0] = game->get_map_window()->get_tile_manager()->get_tile(444)->data;
        shading_tile[1] = game->get_map_window()->get_tile_manager()->get_tile(445)->data;
        shading_tile[2] = game->get_map_window()->get_tile_manager()->get_tile(446)->data;
        shading_tile[3] = game->get_map_window()->get_tile_manager()->get_tile(447)->data;
    }
    else //NUVIE_GAME_MD
    {
        shading_tile[0] = game->get_map_window()->get_tile_manager()->get_tile(268)->data;
        shading_tile[1] = game->get_map_window()->get_tile_manager()->get_tile(269)->data;
        shading_tile[2] = game->get_map_window()->get_tile_manager()->get_tile(270)->data;
        shading_tile[3] = game->get_map_window()->get_tile_manager()->get_tile(271)->data;
    }
}


void Screen::drawalphamap8globe( sint16 x, sint16 y, uint16 r )
{
    sint16 i,j;
    //Clamp lighting globe size to 0-5 (6 levels)
    if( r > 5 )
        r = 5;
    if( r < 1 )
        return;
    if( shading_ambient == 0xFF )
        return;
	if( lighting_style == 0 )
		return;
    if( lighting_style == 2 )
    {
        //Draw using "original" lighting
		for( j = 0; j <= r*2; j++ )
			for( i = 0; i <= r*2; i++ )
			{
				if( x + i - r < 0 || x + i - r >= 11 )
					continue;
				if( y + j - r < 0 || y + j - r >= 11  )
					continue;
				shading_data[(y+j-r)*11+(x+i-r)] = MIN( shading_data[(y+j-r)*11+(x+i-r)] + TileGlobe[r-1][j*(r*2+1)+i], 4 );
			}
        return;
    }
    x = x*16+8;
    y = y*16+8;
    //Draw using "smooth" lighting
    //The x and y are relative to (0,0) of the screen itself, and are absolute coordinates, so are i and j
    r--;
    for(i=-globeradius_2[r];i<globeradius_2[r];i++)
        for(j=-globeradius_2[r];j<globeradius_2[r];j++)
        {
            if( (y-shading_rect.y+i)-1 < 0 ||
                (x-shading_rect.x+j)-1 < 0 ||
                (y-shading_rect.y+i)+1 > shading_rect.h ||
                (x-shading_rect.x+j)+1 > shading_rect.w )
                continue;
            shading_data[(y-shading_rect.y+i)*shading_rect.w+(x-shading_rect.x+j)] = MIN( shading_data[(y-shading_rect.y+i)*shading_rect.w+(x-shading_rect.x+j)] + shading_globe[r][(i+globeradius_2[r])*globeradius[r]+(j+globeradius_2[r])], 255 );
        }
}


void Screen::blitalphamap8()
{
    //pixel = (dst*(1-alpha))+(src*alpha)   for an interpolation
    //pixel = pixel * alpha                 for a reduction
    //We use a reduction here

    if( shading_ambient == 0xFF )
        return;
	if( lighting_style == 0 )
		return;

    uint16 i,j;
    Game *game = Game::get_game();
    updatingalphamap = false;

    if( lighting_style == 2 )
    {
        for( j = 0; j < 11; j++ )
        {
            for( i = 0; i < 11; i++ )
            {
                if( shading_data[j*11+i] < 4 )
                    blit(i*16,j*16,shading_tile[shading_data[j*11+i]],8,16,16,16,true,game->get_map_window()->get_clip_rect());
            }
        }
        return;
    }

    switch( surface->bits_per_pixel )
    {
    case 16:
        uint16 *pixels16;
        pixels16 = (uint16 *)surface->pixels;

        pixels16 += shading_rect.y*surface->w;

        for(i=shading_rect.y;i<shading_rect.h+shading_rect.y;i++)
        {
            for(j=shading_rect.x;j<shading_rect.w+shading_rect.x;j++)
            {
                pixels16[j] = ( ( (unsigned char)(( (float)(( pixels16[j] & surface->Rmask ) >> surface->Rshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Rshift ) | //R
                              ( ( (unsigned char)(( (float)(( pixels16[j] & surface->Gmask ) >> surface->Gshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Gshift ) | //G
                              ( ( (unsigned char)(( (float)(( pixels16[j] & surface->Gmask ) >> surface->Bshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Bshift );  //B

                //Red = 0xF800 = 1111 1000 0000 0000
                //Grn = 0x07E0 = 0000 0111 1110 0000
                //Blu = 0x001F = 0000 0000 0001 1111
            }
            pixels16 += surface->w;
        }
        return;
        break;
    case 24:
    case 32:
        uint32 *pixels;
        pixels = (uint32 *)surface->pixels;

        pixels += shading_rect.y*surface->w;

        for(i=shading_rect.y;i<shading_rect.h+shading_rect.y;i++)
        {
            for(j=shading_rect.x;j<shading_rect.w+shading_rect.x;j++)
            {
            	pixels[j] = ( ( (unsigned char)(( (float)(( pixels[j] & surface->Rmask ) >> surface->Rshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Rshift ) | //R
                            ( ( (unsigned char)(( (float)(( pixels[j] & surface->Gmask ) >> surface->Gshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Gshift ) | //G
                            ( ( (unsigned char)(( (float)(( pixels[j] & surface->Bmask ) >> surface->Bshift)) * (float)(shading_data[(j-shading_rect.x)+(i-shading_rect.y)*shading_rect.w])/255.0f) ) << surface->Bshift );  //B
            }
            pixels += surface->w;
        }
        return;
        break;
    default:
        DEBUG(0,LEVEL_ERROR,"Screen::blitalphamap8() cannot handle your screen surface depth of %d\n",surface->bits_per_pixel);
        break;
        return;
    }

}


/* Return an 8bit surface. Source format is assumed to be identical to screen. */
SDL_Surface *Screen::create_sdl_surface_8(unsigned char *src_buf, uint16 src_w, uint16 src_h)
{
    SDL_Surface *new_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, src_w, src_h, 8, 0, 0, 0, 0);
    unsigned char *pixels = (unsigned char *)new_surface->pixels;

    if(surface->bits_per_pixel == 16)
    {
        uint16 *src = (uint16 *)src_buf;
        for(int p = 0; p < (src_w * src_h); p++)
            for(int i = 0; i < 256; i++) // convert to 8bpp
                if(src[p] == (uint16)surface->colour32[i])
                {
                    pixels[p] = i;
                    break;
                }
    }
    else
    {
        uint32 *src = (uint32 *)src_buf;
        for(int p = 0; p < (src_w * src_h); p++)
            for(int i = 0; i < 256; i++)
                if(src[p] == surface->colour32[i])
                {
                    pixels[p] = i;
                    break;
                }
    }
    return(new_surface);
}


SDL_Surface *Screen::create_sdl_surface_from(unsigned char *src_buf, uint16 src_bpp, uint16 src_w, uint16 src_h, uint16 src_pitch)
{
 SDL_Surface *new_surface;
 uint16 i,j;

 new_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, src_w, src_h, surface->bits_per_pixel,
                                    surface->Rmask, surface->Gmask, surface->Bmask, 0);

 if(surface->bits_per_pixel == 16)
   {
    uint16 *pixels = (uint16 *)new_surface->pixels;

    for(i=0;i<src_h;i++)
      {
       for(j=0;j<src_w;j++)
         {
          pixels[j] = (uint16)surface->colour32[src_buf[j]];
         }
       src_buf += src_pitch;
       pixels += src_pitch; //surface->pitch;
      }
   }
 else
   {
    uint32 *pixels = (uint32 *)new_surface->pixels;

    for(i=0;i<src_h;i++)
      {
       for(j=0;j<src_w;j++)
         {
          pixels[j] = surface->colour32[src_buf[j]];
         }
       src_buf += src_pitch;
       pixels += src_pitch; //surface->pitch;
      }
   }

 return new_surface;

}

uint16 Screen::get_pitch()
{
 return (uint16)surface->pitch;
}

uint16 Screen::get_bpp()
{
 return surface->bits_per_pixel;
}

void Screen::update()
{
 //if(scaled_surface)
 if(scaler)
  {
   scaler->Scale(surface->format_type, surface->pixels,		// type, source
                 0, 0, surface->w, surface->h,							// x, y, w, h
				         surface->pitch/surface->bytes_per_pixel, surface->h,	// pixels/line, pixels/col
				         sdl_surface->pixels,									// dest
				         sdl_surface->pitch/sdl_surface->format->BytesPerPixel,	// destpixels/line
                 scale_factor);
  }
 else
  {
/*  
   uint8 *src = (uint8 *)surface->pixels;
   uint8 *dest = (uint8 *)sdl_surface->pixels;

   memcpy(dest,src,surface->w*surface->bytes_per_pixel);
   dest += sdl_surface->pitch;
   src += surface->pitch; */
  }

SDL_UpdateRect(sdl_surface,0,0,0,0);

 return;
}


void Screen::update(sint32 x, sint32 y, uint16 w, uint16 h)
{
 //if(scaled_surface)
/*
 if(x >= 2)
   x -= 2;
 if(y >= 2)
   y -= 2;
 if(w <= surface->w-2)
   w += 2;
 if(h <= surface->h-2)
   h += 2;
*/

 if(x < 0) x = 0;
 if(y < 0) y = 0;
 if(x > width)
   return;
 if(y > height)
   return;
 if((x + w) > width) w = width - x;
 if((y + h) > height) h = height - y;

 if(scaler)
  {
   scaler->Scale(surface->format_type, surface->pixels,		// type, source
                 x, y, w, h,							// x, y, w, h
                 surface->pitch/surface->bytes_per_pixel, surface->h,	// pixels/line, pixels/col
                 sdl_surface->pixels,									// dest
                 sdl_surface->pitch/sdl_surface->format->BytesPerPixel,	// destpixels/line
                 scale_factor);
  }

 if(num_update_rects == max_update_rects)
   {
    update_rects = (SDL_Rect *)realloc(update_rects,sizeof(SDL_Rect) * (max_update_rects + 10));
    max_update_rects += 10;
   }

 update_rects[num_update_rects].x = x*scale_factor;
 update_rects[num_update_rects].y = y*scale_factor;
 update_rects[num_update_rects].w = w*scale_factor;
 update_rects[num_update_rects].h = h*scale_factor;

 num_update_rects++;

 //SDL_UpdateRect(sdl_surface,x*scale_factor,y*scale_factor,w*scale_factor,h*scale_factor);

 //DEBUG(0,LEVEL_DEBUGGING,"update rect(%d,%d::%d,%d)\n", update_rects[num_update_rects].x, update_rects[num_update_rects].y, update_rects[num_update_rects].w, update_rects[num_update_rects].h);

 return;
}

void Screen::preformUpdate()
{
/*
 uint16 i;
 //DEBUG(0,LEVEL_DEBUGGING,"Screen update %d.\n",num_update_rects);
 if(!scaler)
  {
   uint8 *src = (uint8 *)surface->pixels;
   uint8 *dest = (uint8 *)sdl_surface->pixels;

   for(i=0;i < num_update_rects;i++)
    {
     memcpy(dest + update_rects[i].y * sdl_surface->pitch + sdl_surface->format->BytesPerPixel * update_rects[i].x,
            src + update_rects[i].y * surface->pitch + surface->bytes_per_pixel * update_rects[i].x,
            update_rects[i].w * surface->bytes_per_pixel);
    }
  }
*/
  
 SDL_UpdateRects(sdl_surface,num_update_rects,update_rects);
 num_update_rects = 0;
}

void Screen::lock()
{
// SDL_LockSurface(scaled_surface);

 return;
}

void Screen::unlock()
{
 //SDL_UnlockSurface(scaled_surface);

 return;
}

bool Screen::initScaler()
{
 std::string scaler_name;

 return true;
}

void Screen::set_screen_mode()
{
	// Get info. about video.
	const SDL_VideoInfo *vinfo = SDL_GetVideoInfo();
	uint32 flags = 0;

	if (scale_factor == 0) scale_factor = 1;
	scaler = 0;

	// Get BPP
	int bpp = vinfo->vfmt->BitsPerPixel;

	// If we can't use the format, force 16 bit
	if (bpp != 16 && bpp != 32)
		bpp = 16;

#ifdef MACOSX 
    if (scale_factor == 1) //FIXME There appears to be an update issue with 32bpp x1 scale on OS X
        bpp = 16;          // I'll need to look into this further. For now we can just use 16bpp at x1 scale.
#endif

	DEBUG(0,LEVEL_DEBUGGING,"Attempting to set vid mode: %dx%dx%dx%d",width,height,bpp,scale_factor);

	// Is Fullscreen?
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
		DEBUG(1,LEVEL_DEBUGGING," Fullscreen");
	}
//	else
//		flags |= SDL_RESIZABLE;

	// Opengl Stuff
#ifdef WANT_OPENGL
	if (useOpengl) {
		DEBUG(0,LEVEL_DEBUGGING," OpenGL\n");

		// Want double-buffering.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );

		//SDL_GL_LoadLibrary("opengl32.dll");

		// Allocate surface: 640x480x32bpp.
		sdl_surf = SDL_SetVideoMode(width*scale_factor, height*scale_factor, bpp, flags|SDL_OPENGL|SDL_GL_DOUBLEBUFFER|SDL_HWSURFACE);
		if (sdl_surf) {
			opengl = new OpenGL(width, height, palette);
			surface = CreateRenderSurface(opengl);
			surface->w = width;
			surface->h = height;

			mouse->init_opengl(screen);
			ShapeManager::get().init_opengl(screen);

			DEBUG(0,LEVEL_DEBUGGING,"%d bytes of textures loaded so far\n",opengl->get_texture_mem_used());

			// Hide system mouse cursor
			SDL_ShowCursor(0);

			return;
		}

		DEBUG(0,LEVEL_DEBUGGING,"Setting up OpenGL Failed. Trying\n");
	}
#endif //WANT_OPENGL

	if (vinfo->hw_available && doubleBuffer && fullscreen) {
		flags |= SDL_HWSURFACE|SDL_DOUBLEBUF;
		DEBUG(1,LEVEL_DEBUGGING," Hardware Double Buffered\n");
	}
	else {
		flags |= SDL_SWSURFACE;
		DEBUG(1,LEVEL_DEBUGGING," Software Surface\n");
	}

	// Old Software rendering. Try a scaler_index first,
	if (!try_scaler(width, height, flags, bpp)) {

		scale_factor = 1;
		scaler = 0;
		sdl_surface = SDL_SetVideoMode(width, height, bpp, flags);

		// Couldn't create it, so disable double buffering
		if (!sdl_surface) {
			flags &= ~(SDL_HWSURFACE|SDL_DOUBLEBUF);
			flags |= SDL_SWSURFACE;
			scale_factor = 1;
			scaler = 0;
			sdl_surface = SDL_SetVideoMode(width, height, bpp, flags);
		}

		if (!sdl_surface) {
			DEBUG(0,LEVEL_NOTIFICATION,"Unable to create surface. Attempting 320x200x1 Software Surface\n");
			width = 320;
			height = 200;
			sdl_surface = SDL_SetVideoMode(width, height, bpp, flags);
			if (!sdl_surface) {
				DEBUG(0,LEVEL_EMERGENCY,"Unable to create surface. Exiting\n");
				exit (-1);
			}
		}

		if (sdl_surface->flags & SDL_HWSURFACE) {
			DEBUG(0,LEVEL_DEBUGGING,"Created Double Buffered Surface\n");
			surface = CreateRenderSurface (width, height, bpp);
		}
		else {
			surface = CreateRenderSurface(sdl_surface);
		}
	}
#ifdef UNDER_CE
	SHFullScreen(GetActiveWindow(), SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON);
#endif
	// Hide system mouse cursor
#ifndef UNDER_CE
//	SDL_ShowCursor(0);
#endif

	surface->set_format(sdl_surface->format);


//	if (zbuffer) screen->create_zbuffer();
}


bool Screen::try_scaler(int w, int h, uint32 flags, int hwdepth)
{
	// Try the universal scalers
	if (scale_factor > 1 && scaler_index >= 0) {

		scaler = scaler_reg.GetScaler(scaler_index);

		// If the scaler wasn't found, use the Point scaler
		if (!scaler) {
			DEBUG(0,LEVEL_NOTIFICATION,"Couldn't find scaler for scaler_index %d\n",scaler_index);
			scaler = scaler_reg.GetPointScaler();
		}
		// If the scaler selected is 2x only, and we are in a > than 2x mode, use Point
		else if (scale_factor > 2 && scaler->flags & SCALER_FLAG_2X_ONLY)
		{
			DEBUG(0,LEVEL_NOTIFICATION,"Scaler %s only supports 2x. %dx requested\n", scaler->name, scale_factor);
			scaler = scaler_reg.GetPointScaler();
		}
		// If it requires 16 bit, force that. However, if it fails use point
		else if (scaler->flags & SCALER_FLAG_16BIT_ONLY)
		{
			if ( !SDL_VideoModeOK(w, h, 16, flags)) {
				DEBUG(0,LEVEL_NOTIFICATION,"%s requires 16 bit colour. Couldn't set mode.\n",scaler->name );
				scaler = scaler_reg.GetPointScaler();
			}
			else if (hwdepth != 16) {
				DEBUG(0,LEVEL_NOTIFICATION,"%s requires 16 bit colour. Forcing.\n",scaler->name);
				hwdepth = 16;
			}
		}
		// If it requires 32 bit, force that. However, if it fails use point
		else if (scaler->flags & SCALER_FLAG_32BIT_ONLY)
		{
			if ( !SDL_VideoModeOK(w, h, 32, flags)) {
				DEBUG(0,LEVEL_NOTIFICATION,"%s requires 32 bit colour. Couldn't set mode.\n",scaler->name );
				scaler = scaler_reg.GetPointScaler();
			}
			else if (hwdepth != 32) {
				DEBUG(0,LEVEL_NOTIFICATION,"%s requires 32 bit colour. Forcing.\n",scaler->name);
				hwdepth = 32;
			}
		}

		DEBUG(0,LEVEL_NOTIFICATION,"Using scaler: %s\n",scaler->name);

		// Attempt to set Video mode
		if ( !SDL_VideoModeOK(w, h, hwdepth, flags))
		{
			hwdepth = 0;

			// Try 32 bit (but only if allowed)
			if (SDL_VideoModeOK(w, h, 32, flags) && !(scaler->flags & SCALER_FLAG_16BIT_ONLY))
				hwdepth = 32;
			// Try 16 bit (but only if allowed)
			else if (SDL_VideoModeOK(w, h, 16, flags) && !(scaler->flags & SCALER_FLAG_32BIT_ONLY))
				hwdepth = 16;
		}

		// Oh no, it didn't work
		if (hwdepth != 16 && hwdepth != 32)
		{
			DEBUG(0,LEVEL_NOTIFICATION,"%s requires 16/32 bit colour. Couldn't set mode.\n",scaler->name);
		}
		else if ((sdl_surface = SDL_SetVideoMode(w*scale_factor, h*scale_factor, hwdepth, flags)))
		{
			/* Create render surface */
			surface = CreateRenderSurface (w, h, hwdepth);
			return true;
		}

		// Output that scaled surface creation failed
		DEBUG(0,LEVEL_ERROR,"Couldn't create %s scaled surface\n",scaler->name);
		delete surface;
		scaler = 0;
		surface = 0;
		sdl_surface = 0;
		return false;
	}

	return false;
}

//Note! assumes area divides evenly by down_scale factor
unsigned char *Screen::copy_area(SDL_Rect *area, uint16 down_scale)
{
 if(surface->bits_per_pixel == 16)
   return(copy_area16(area, down_scale));

 return(copy_area32(area, down_scale));
}

unsigned char *Screen::copy_area16(SDL_Rect *area, uint16 down_scale)
{
 SDL_PixelFormat *fmt;
 SDL_Surface *main_surface = get_sdl_surface();
 unsigned char *dst_pixels = NULL;
 unsigned char *ptr;
 uint16 *src_pixels;
 uint32 r,g,b;
 uint32 ra, ga, ba;
 uint16 x, y;
 uint8 x1, y1;

 dst_pixels = new unsigned char[((area->w / down_scale) * (area->h / down_scale)) * 3];
 ptr = dst_pixels;

 fmt = main_surface->format;

 for(y = 0; y < area->h; y += down_scale)
  {
   for(x = 0; x < area->w; x += down_scale)
    {
     r = 0;
     g = 0;
     b = 0;

     src_pixels = (uint16 *)main_surface->pixels;
     src_pixels += ((area->y + y) * surface->w + (area->x + x));

     for(y1 = 0; y1 < down_scale; y1++)
      {
       for(x1 = 0; x1 < down_scale; x1++)
        {
         ra = *src_pixels & fmt->Rmask;
         ra >>= fmt->Rshift;
         ra <<= fmt->Rloss;

         ga = *src_pixels & fmt->Gmask;
         ga >>= fmt->Gshift;
         ga <<= fmt->Gloss;

         ba = *src_pixels & fmt->Bmask;
         ba >>= fmt->Bshift;
         ba <<= fmt->Bloss;

         r += ra;
         g += ga;
         b += ba;

         src_pixels++;
        }
       src_pixels += surface->w;
      }

     ptr[0] = (uint8)(r/(down_scale*down_scale));
     ptr[1] = (uint8)(g/(down_scale*down_scale));
     ptr[2] = (uint8)(b/(down_scale*down_scale));
     ptr += 3;
    }
  }

 return dst_pixels;
}

unsigned char *Screen::copy_area32(SDL_Rect *area, uint16 down_scale)
{
 SDL_PixelFormat *fmt;
 SDL_Surface *main_surface = get_sdl_surface();
 unsigned char *dst_pixels = NULL;
 unsigned char *ptr;
 uint32 *src_pixels;
 uint32 r,g,b;
 uint32 ra, ga, ba;
 uint16 x, y;
 uint8 x1, y1;

 dst_pixels = new unsigned char[((area->w / down_scale) * (area->h / down_scale)) * 3];
 ptr = dst_pixels;

 fmt = main_surface->format;

 for(y = 0; y < area->h; y += down_scale)
  {
   for(x = 0; x < area->w; x += down_scale)
    {
     r = 0;
     g = 0;
     b = 0;

     src_pixels = (uint32 *)main_surface->pixels;
     src_pixels += ((area->y + y) * surface->w + (area->x + x));

     for(y1 = 0; y1 < down_scale; y1++)
      {
       for(x1 = 0; x1 < down_scale; x1++)
        {
         ra = *src_pixels & fmt->Rmask;
         ra >>= fmt->Rshift;
         ra <<= fmt->Rloss;

         ga = *src_pixels & fmt->Gmask;
         ga >>= fmt->Gshift;
         ga <<= fmt->Gloss;

         ba = *src_pixels & fmt->Bmask;
         ba >>= fmt->Bshift;
         ba <<= fmt->Bloss;

         r += ra;
         g += ga;
         b += ba;

         src_pixels++;
        }
       src_pixels += surface->w;
      }

     ptr[0] = (uint8)(r/(down_scale*down_scale));
     ptr[1] = (uint8)(g/(down_scale*down_scale));
     ptr[2] = (uint8)(b/(down_scale*down_scale));
     ptr += 3;
    }
  }

 return dst_pixels;
}

// surface -> unsigned char *
// (NULL area = entire screen)
unsigned char *Screen::copy_area(SDL_Rect *area)
{
    SDL_Rect screen_area = { 0, 0, surface->w, surface->h };
    if(!area)
        area = &screen_area;

    if(surface->bits_per_pixel == 16)
        return(copy_area16(area));
    return(copy_area32(area));
}


// unsigned char * -> surface
// unsigned char * -> target (src area still means location on screen, not relative to target)
// (NULL area = entire screen)
void Screen::restore_area(unsigned char *pixels, SDL_Rect *area,
                          unsigned char *target, SDL_Rect *target_area)
{
    SDL_Rect screen_area = { 0, 0, surface->w, surface->h };
    if(!area)
        area = &screen_area;

    if(surface->bits_per_pixel == 16)
        restore_area16(pixels, area, target, target_area);
    else
        restore_area32(pixels, area, target, target_area);
}


unsigned char *Screen::copy_area32(SDL_Rect *area)
{
    uint32 *copied = (uint32 *)malloc(area->w * area->h * 4);
    uint32 *dest = copied;
    uint32 *src = (uint32 *)surface->pixels;
            src += area->y * surface->w + area->x;

    for(uint32 i = 0; i < area->h; i++)
    {
        for(uint32 j = 0; j < area->w; j++)
            dest[j] = src[j];
        dest += area->w;
        src += surface->w;
    }
    return((unsigned char *)copied);
}


void Screen::restore_area32(unsigned char *pixels, SDL_Rect *area,
                            unsigned char *target, SDL_Rect *target_area)
{
    uint32 *src = (uint32 *)pixels;
    uint32 *dest = (uint32 *)surface->pixels;
            dest += area->y * surface->w + area->x;
    if(target) // restore to target instead of screen
    {
        dest = (uint32 *)target;
        dest += (area->y-target_area->y) * target_area->w + (area->x-target_area->x);
    }
    for(uint32 i = 0; i < area->h; i++)
    {
        for(uint32 j = 0; j < area->w; j++)
            dest[j] = src[j];
        src += area->w;
        dest += target ? target_area->w : surface->w;
    }
    free(pixels);
}


unsigned char *Screen::copy_area16(SDL_Rect *area)
{
    uint16 *copied = (uint16 *)malloc(area->w * area->h * 2);
    uint16 *dest = copied;
    uint16 *src = (uint16 *)surface->pixels;
            src += area->y * surface->w + area->x;

    for(uint32 i = 0; i < area->h; i++)
    {
        for(uint32 j = 0; j < area->w; j++)
            dest[j] = src[j];
        dest += area->w;
        src += surface->w;
    }
    return((unsigned char *)copied);
}


void Screen::restore_area16(unsigned char *pixels, SDL_Rect *area,
                            unsigned char *target, SDL_Rect *target_area)
{
    uint16 *src = (uint16 *)pixels;
    uint16 *dest = (uint16 *)surface->pixels;
            dest += area->y * surface->w + area->x;
    if(target) // restore to target instead of screen
    {
        dest = (uint16 *)target;
        dest += (area->y-target_area->y) * target_area->w + (area->x-target_area->x);
    }

    for(uint32 i = 0; i < area->h; i++)
    {
        for(uint32 j = 0; j < area->w; j++)
            dest[j] = src[j];
        src += area->w;
        dest += target ? target_area->w : surface->w;
    }
    free(pixels);
}
