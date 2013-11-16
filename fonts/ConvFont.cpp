/*
 *  ConvFont.cpp
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

#include <stdio.h>

#include <string>
#include <cctype>

#include "nuvieDefs.h"

#include "Screen.h"
#include "ConvFont.h"

ConvFont::ConvFont()
{
 data_offset = 0;
 num_chars = 0;
 offset = 0;
 f_data = NULL;
 f_w_data = NULL;
}

ConvFont::~ConvFont()
{

}

bool ConvFont::init(unsigned char *data, uint8 *width_data, uint16 num_c, uint16 char_offset)
{
 data_offset = char_offset;
 num_chars = num_c;

 f_data = data;
 f_w_data = width_data;


 return true;
}

uint16 ConvFont::getCharWidth(uint8 c)
{
	return f_w_data[c+data_offset];
}

uint16 ConvFont::drawChar(Screen *screen, uint8 char_num, uint16 x, uint16 y,
                    uint8 color)
{

 unsigned char *buf = (unsigned char *)f_data + (data_offset * 64) + (char_num % 16) * 8 + (char_num / 16) * 128 * 8;
 screen->blit(x,y,buf,8,8,8,128,true,NULL);
 return getCharWidth(char_num);
}
