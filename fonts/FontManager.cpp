/*
 *  FontManager.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Jan 28 2004.
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

#include "nuvieDefs.h"
#include "Configuration.h"

#include "NuvieIOFile.h"
#include "FontManager.h"
#include "Font.h"

FontManager::FontManager(Configuration *cfg)
{
 config = cfg;
 num_fonts = 0;
}

FontManager::~FontManager()
{
 std::vector<Font *>::iterator font;
 
 for(font = fonts.begin(); font != fonts.end();)
   {
    delete *font;
    font++;
   }

}
 
bool FontManager::init()
{
 Font *font;
 unsigned char *font_data;
 std::string filename;
 NuvieIOFileRead u6_ch;

 config->pathFromValue("config/ultima6/gamedir","u6.ch",filename);
 
 if(u6_ch.open(filename) == false)
   return false;
 
 font_data = u6_ch.readAll();
 if(font_data == NULL)
   return false;

 // english font
 font = new Font();
 font->init(font_data,128,0);
 fonts.push_back(font);
 num_fonts++;
 
 // runic & gargoyle font
 font = new Font();
 font->init(&font_data[128 * 8], 128, 0);
 fonts.push_back(font);
 num_fonts++;

 free(font_data);
 return true;
}

Font *FontManager::get_font(uint16 font_number)
{
 if(num_fonts > 0 && font_number < num_fonts)
   return fonts[font_number]; //fonts.at(font_number);

 return NULL;
}
