/*
 *  U6Lib_n.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Sat Mar 08 2003.
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

#include <stdlib.h>

#include "U6Lib_n.h"

U6Lib_n::U6Lib_n(std::string &filename, uint8 size)
{
 file.open(filename,"r");
 
 lib_size = size;
 num_offsets = 0;
 num_zero_offsets = 0;
 offsets = NULL;
 
 this->parse_lib();
}

U6Lib_n::~U6Lib_n(void)
{

 free(offsets);
}
 
uint32 U6Lib_n::get_num_items(void)
{
 return num_offsets;
}

uint32 U6Lib_n::get_item_size(uint32 item_number)
{
 uint32 size;
 
 if(item_number >= num_offsets)
   return 0;
   
 size = offsets[item_number+1] - offsets[item_number];

 return size;
}

unsigned char *U6Lib_n::get_item(uint32 item_number)
{
 uint32 item_size;
 unsigned char *buf;
 
 if(item_number >= num_offsets)
   return NULL;
 
 item_size = this->get_item_size(item_number);
 
 buf = (unsigned char *)malloc(item_size);
 
 file.readToBuf(buf,item_size);
 
 return buf;
}

void U6Lib_n::parse_lib()
{
 uint32 first_offset;
 uint32 i;
 
 if(lib_size != 2 && lib_size != 4)
   return;
 
 file.seekStart();
 
 first_offset = this->get_first_offset();

 num_offsets = (first_offset - (num_zero_offsets * lib_size)) / lib_size;

 offsets = (uint32 *)malloc(sizeof(uint32) * (num_offsets + 1));
 
 file.seek((num_zero_offsets + 1) * lib_size);

 offsets[0] = first_offset;
  
 for(i=1;i<num_offsets && !file.eof();i++)
   {
    if(lib_size == 2)
       offsets[i] = file.read2();
    else
       offsets[i] = file.read4();
   }

 offsets[num_offsets] = file.filesize(); //this is used to calculate the size of the last item in the lib.
 
 return; 
}

// we need to handle NULL offsets at the start of the offset table in the converse.a file
uint32 U6Lib_n::get_first_offset()
{
 uint32 i;
 uint32 offset = 0;
 
 for(i=0;!file.eof();i++)
   {
    if(lib_size == 2)
      offset = file.read2();
    else
      offset = file.read4();

    if(offset != 0)
      break;
   }
   
 if(offset != 0)
   {
    num_zero_offsets = i;
    return offset;
   }
   
 return 0;
}
