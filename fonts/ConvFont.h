#ifndef __ConvFont_h__
#define __ConvFont_h__

/*
 *  ConvFont.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 08 2012.
 *  Copyright (c) 2012. All rights reserved.
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

class ConvFont : public Font
{
   uint16 data_offset;
 public:

   ConvFont();
   ~ConvFont();

   bool init(unsigned char *data, uint16 num_chars, uint16 char_offset);

   uint16 getCharWidth(uint8 c);
   uint16 drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y,
                 uint8 color);
  protected:

};

#endif /* __ConvFont_h__ */
