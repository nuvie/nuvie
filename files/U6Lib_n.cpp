/*
 *  U6Lib_n.cpp
 *  Nuvie
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

#include <cstdlib>
#include <cstdio>
#include <cctype>

#include "NuvieIOFile.h"
#include "U6Lzw.h"

#include "U6Lib_n.h"

U6Lib_n::U6Lib_n()
{
 num_offsets = 0;
 items = NULL;
 data = NULL;
}


U6Lib_n::~U6Lib_n(void)
{
 close();
}

bool U6Lib_n::open(std::string &filename, uint8 size, uint8 type)
{
 NuvieIOFileRead *file;
 
 file = new NuvieIOFileRead();
 
 if(file->open(filename) == false)
   {
    delete file;
    return false;
   }

 return open((NuvieIO *)file, size, type);
}

bool U6Lib_n::open(NuvieIO *new_data, uint8 size, uint8 type)
{
 game_type = type;
 data = new_data;
 
 lib_size = size;
 this->parse_lib();

 return true;
}

void U6Lib_n::close()
{
 if(items)
  free(items);
 items = NULL;
 
 if(data != NULL)
   data->close();
 
 num_offsets = 0;
 
 return;
}

uint32 U6Lib_n::get_num_items(void)
{
 return num_offsets;
}


/* Returns the location of `item_number' in the library file.
 */
uint32 U6Lib_n::get_item_offset(uint32 item_number)
{
    if(item_number >= num_offsets)
        return(0);
    return(items[item_number].offset);
}

uint32 U6Lib_n::get_item_size(uint32 item_number)
{
 if(item_number >= num_offsets)
   return(0);

 return(items[item_number].uncomp_size);
}

unsigned char *U6Lib_n::get_item(uint32 item_number, unsigned char *ret_buf)
{
 U6LibItem *item;
 unsigned char *buf, *lzw_buf;

 if(item_number >= num_offsets)
   return NULL;
   
 item = &items[item_number];
 
 if(item->size == 0)
   return NULL;

 if(ret_buf == NULL)
   buf = (unsigned char *)malloc(item->uncomp_size);
 else
   buf = ret_buf;

 data->seek(item->offset);
 
 if(is_compressed(item_number))
  {
   U6Lzw lzw;
   lzw_buf = (unsigned char *)malloc(item->size);
   data->readToBuf(lzw_buf,item->size);
   lzw.decompress_buffer(lzw_buf, item->size, buf, item->uncomp_size);
  }
 else
   data->readToBuf(buf,item->size);
 
 return buf;
}

bool U6Lib_n::is_compressed(uint32 item_number)
{
 uint32 i;
 
 switch(items[item_number].flag)
  {
   case 0x1 :
   case 0x20 : return true;
   case 0xff :
               for(i=item_number;i<num_offsets;i++)
                 {
                  if(items[i].flag != 0xff)
                    break;
                 }
               if(i < num_offsets)
                 return is_compressed(i);
               break;
  }

 return false;
}

void U6Lib_n::parse_lib()
{
 uint32 i;
 bool skip4 = false;
 
 if(lib_size != 2 && lib_size != 4)
   return;
 
 data->seekStart();
 
 if(game_type != NUVIE_GAME_U6) //U6 doesn't have a 4 byte filesize header.
    {
     skip4 = true;
     filesize = data->read4();
    }
 else
    filesize = data->get_size();
    
 num_offsets = calculate_num_offsets(skip4);
 
 items = (U6LibItem *)malloc(sizeof(U6LibItem) * (num_offsets + 1));
 memset(items, 0, sizeof(U6LibItem) * (num_offsets + 1));

 data->seekStart();
 if(skip4)
    data->seek(0x4);
 for(i = 0; i < num_offsets && !data->is_end(); i++)
   {
    if(lib_size == 2)
       items[i].offset = data->read2();
    else
      {
       items[i].offset = data->read4();
       items[i].flag = (items[i].offset & 0xff000000) >> 24; //extract flag byte
       items[i].offset &= 0xffffff;
      }
   }

 items[num_offsets].offset = filesize; //this is used to calculate the size of the last item in the lib.

 calculate_item_sizes();
 
 return; 
}

void U6Lib_n::calculate_item_sizes()
{
 uint32 i, next_offset = 0;

 for(i=0;i < num_offsets; i++)
   {
    items[i].size = 0;
    // get next non-zero offset, including the filesize at items[num_offsets]
    for(uint32 o = (i + 1); o <= num_offsets; o++)
        if(items[o].offset)
        {
            next_offset = items[o].offset;
            break;
        }

    if(items[i].offset && (next_offset > items[i].offset))
        items[i].size = next_offset - items[i].offset;
    
    items[i].uncomp_size = calculate_item_uncomp_size(&items[i]);
   }

 return;
}

uint32 U6Lib_n::calculate_item_uncomp_size(U6LibItem *item)
{
 uint32 uncomp_size = 0;
 
 switch(item->flag)
  {
   case 0x01 : //compressed
               data->seek(item->offset);
               uncomp_size = data->read4();
               break;

               //FIX check this. uncompressed 4 byte item size header
   case 0xc1 : uncomp_size = item->size - 4;
                break;

              // uncompressed
   case 0x0  :
   case 0x2  :
   case 0xe0 :
   default   : uncomp_size = item->size;
               break;
  }

 return uncomp_size;
}

// we need to handle NULL offsets at the start of the offset table in the converse.a file
uint32 U6Lib_n::calculate_num_offsets(bool skip4) //skip4 bytes of header.
{
 uint32 i;
 uint32 offset = 0;

 if(skip4)
   data->seek(0x4);
   
 //find first non-zero offset and calculate num_offsets from that.  
 for(i=0;!data->is_end();i++)
   {
    if(lib_size == 2)
      offset = data->read2();
    else
      {
       offset = data->read4();
       offset &= 0xffffff; // clear flag byte.
      }
    if(offset != 0)
      {
       if(skip4)
         offset -= 4;
         
       return offset / lib_size;
      } 
   }
      
 return 0;
}
