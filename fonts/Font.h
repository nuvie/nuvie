#ifndef __Font_h__
#define __Font_h__

/*
 *  Font.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Jan 28 2003.
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

#define FONT_COLOR_U6_NORMAL    0x48
#define FONT_COLOR_U6_HIGHLIGHT 0x0c
#define FONT_COLOR_WOU_NORMAL    0
#define FONT_COLOR_WOU_HIGHLIGHT 4

class Configuration;
class Screen;
class U6Shape;

class Font
{
protected:
 unsigned char *font_data;
 uint16 num_chars;
 uint16 offset;

private:
 uint16 height;
 uint8 pixel_char;

 public:

   Font();
   ~Font();

   virtual bool init(unsigned char *data, uint16 num_chars, uint16 char_offset) {return false;}
   bool init(const char *filename);
   bool initWithBuffer(unsigned char *buffer, uint32 buffer_len);

//   bool drawString(Screen *screen, std::string str, uint16 x, uint16 y);
   uint16 drawString(Screen *screen, const char *str, uint16 x, uint16 y, uint8 color=FONT_COLOR_U6_NORMAL, uint8 highlight_color=FONT_COLOR_U6_HIGHLIGHT);
   uint16 drawString(Screen *screen, const char *str, uint16 string_len, uint16 x, uint16 y, uint8 color, uint8 highlight_color=FONT_COLOR_U6_HIGHLIGHT);

   virtual uint16 drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y,
                 uint8 color = FONT_COLOR_U6_NORMAL);

   uint16 drawStringToShape(U6Shape *shp, const char *str, uint16 x, uint16 y, uint8 color);
   uint8 drawCharToShape(U6Shape *shp, uint8 char_num, uint16 x, uint16 y, uint8 color);

   virtual uint16 getCharWidth(uint8 c);
   uint16 getStringWidth(const char *str);
  protected:

   uint8 get_char_num(uint8 c);

};

#endif /* __Font_h__ */
