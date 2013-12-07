#ifndef __FontManager_h__
#define __FontManager_h__

/*
 *  FontManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Dec 07 2003.
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
#include <list>

class Configuration;
class Font;

#define NUVIE_FONT_NORMAL 0
#define NUVIE_FONT_GARG   1

class FontManager
{
 Configuration *config;

 std::vector<Font *> fonts;
 uint16 num_fonts;
 Font *conv_font;
 Font *conv_garg_font;
 unsigned char *conv_font_data;
 uint8 *conv_font_widths;
 public:

 FontManager(Configuration *cfg);
 ~FontManager();

 bool init(nuvie_game_t game_type);


 Font *get_font(uint16 font_number);
 Font *get_conv_font() { return conv_font; }
 Font *get_conv_garg_font() { return conv_garg_font; }

 protected:

 bool initU6();
 bool initWOU(std::string filename);
 bool initMDSystemFont();
 bool initConvFonts(nuvie_game_t game_type);
};

#endif /* __FontManager_h__ */
