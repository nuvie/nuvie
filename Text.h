#ifndef __Text_h__
#define __Text_h__

/*
 *  Text.h
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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

#include "Configuration.h"
#include "Screen.h"

class Text
{
 Configuration *config;
 unsigned char *font_data;
 
 public:
 
   Text(Configuration *cfg);
   ~Text();
   
   bool loadFont();
   
   bool drawString(Screen *screen, std::string str, uint16 x, uint16 y, uint8 lang_num=0);
   bool drawString(Screen *screen, char *str, uint16 x, uint16 y, uint8 lang_num=0);
   uint8 Text::get_char_num(uint8 c, uint8 lang_num);
   void Text::drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y); 
};

#endif /* __Text_h__ */