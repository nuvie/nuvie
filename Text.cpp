/*
 *  Text.cpp
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

#include <stdio.h>

#include <string>

#include "Configuration.h"
#include "U6File.h"

#include "Screen.h"

#include "Text.h"

Text::Text(Configuration *cfg)
{
 config = cfg;
 
 font_data = NULL;
}

Text::~Text()
{
 if(font_data != NULL)
   free(font_data);
}
   
bool Text::loadFont()
{
 std::string filename;
 U6File u6_ch;

 config->pathFromValue("config/ultima6/gamedir","u6.ch",filename);
 
 if(u6_ch.open(filename,"rb") == false)
   return false;
 
 font_data = u6_ch.readFile();
 if(font_data == NULL)
   return false;
   
 return true;
}

bool Text::drawString(Screen *screen, std::string str, uint16 x, uint16 y, uint8 lang_num)
{
 return drawString(screen, str.c_str(), x, y, lang_num);
}

bool Text::drawString(Screen *screen, const char *str, uint16 x, uint16 y, uint8 lang_num)
{
 uint16 i, len, l; // l is drawn-index of character, to determine x
 bool highlight = false;
 
 if(font_data == NULL)
   return false;
 
 len = strlen(str);
 
 for(i=0, l=0;i<len;i++)
   {
    if(str[i] == '@')
       highlight = true;
    else
      {
       if(!isalpha(str[i]))
          highlight = false;
       drawChar(screen, get_char_num(str[i],lang_num), x + (l++) * 8, y,
                highlight ? 0x0c : 0x48);
      }
   }
 highlight = false;
 return true;
}

uint8 Text::get_char_num(uint8 c, uint8 lang_num)
{
 if(c < 32 || c > 126) // lock char into ascii chars supported by U6 font.
   c = 32;
 if(lang_num > 0)
   {
    if(c >= 97 && c < 123) // toupper alpha characters
      c -= 32;

    if(lang_num == 1) // Britannian
      c += 128;

    if(lang_num == 2) // Gargoylian 
      {
       if(c < 64) // note uses non alpha chars from britannian. :)
          c += 128;
       else
          c += 160;
      }
   }

 return c;
}

void Text::drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y,
                    uint8 color)
{
 unsigned char buf[64];
 unsigned char *pixels;
 uint16 i,j;
 unsigned char *font;
 uint16 pitch;

 memset(buf,0xff,64);
 
 //pixels = (unsigned char *)screen->get_pixels();
 pixels = buf;
 pitch = 8;//screen->get_pitch();
 
 font = &font_data[char_num * 8];
 
 //pixels += y * pitch + x;
 
 for(i=0;i<8;i++)
   {
    for(j=8;j>0;j--)
      {
       if(font[i] & (1<<(j-1)))
         pixels[8-j] = color; // 0th palette entry should be black
   //    else
     //    pixels[7-j] = 0x48;
      }
      
    pixels += pitch;
   }

 screen->blit(x,y,buf,8,8,8,8,true,NULL);
 return;
}
