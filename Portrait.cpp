/*
 *  Portrait.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 20 2003.
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
#include "NuvieIOFile.h"
#include "Portrait.h"
#include "U6Lzw.h"

Portrait::Portrait(Configuration *cfg)
{
 config = cfg;
}

bool Portrait::init()
{
 std::string filename;
 NuvieIOFileRead objlist;
 
 config->pathFromValue("config/ultima6/gamedir","savegame/objlist",filename);
 if(objlist.open(filename) == false)
   throw "Opening savegame/objlist";

 objlist.seek(0x1c72);
 
 avatar_portrait_num = objlist.read1(); //read in the avatar portrait number from objlist.
 
 
 config->pathFromValue("config/ultima6/gamedir","portrait.a",filename);
 if(portrait_a.open(filename,4) == false)
    throw "Opening portrait.a";
 
 config->pathFromValue("config/ultima6/gamedir","portrait.b",filename);
 if(portrait_b.open(filename,4) == false)
    throw "Opening portrait.b";
 
 config->pathFromValue("config/ultima6/gamedir","portrait.z",filename);
 if(portrait_z.open(filename,4) == false)
    throw "Opening portrait.z";
 
 return true;
}

unsigned char *Portrait::get_portrait_data(uint16 num)
{
 U6Lzw lzw;
 U6Lib_n *portrait;
 unsigned char *lzw_data;
 uint32 new_length;
 unsigned char *new_portrait;
 
 if(num <= 1) //avatar portrait
    {
     portrait = &portrait_z;
     num = avatar_portrait_num;
    }
 else
   {
    num -= 1;
 
    if(num > 194) //there are 194 npc portraits
      return NULL;

    if(num < 98)
      portrait = &portrait_a;
    else
     {
      num -= 98;
      portrait = &portrait_b;
     }
   }

 lzw_data = portrait->get_item(num);
 new_portrait = lzw.decompress_buffer(lzw_data, portrait->get_item_size(num), new_length);
 
 return new_portrait;
}
