#ifndef __BMPFont_h__
#define __BMPFont_h__

/*
 *  BMPFont.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Feb 09 2013.
 *  Copyright (c) 2013. All rights reserved.
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

#include "Font.h"

class Configuration;
class Screen;

class BMPFont : public Font
{
   SDL_Surface *sdl_font_data;
   uint8 *font_width_data;

   uint16 char_w, char_h;

   bool dual_font_mode;
   bool rune_mode;

 public:

   BMPFont();
   ~BMPFont();

   bool init(std::string bmp_filename, bool dual_fontmap=false);

   uint16 getCharWidth(uint8 c);
   uint16 getCharHeight() { return 16; }
   uint16 drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y,
                 uint8 color);
   uint16 getStringWidth(const char *str, uint16 string_len);
  protected:

};

#endif /* __BMPFont_h__ */
