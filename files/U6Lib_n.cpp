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

#include "U6Lzw.h"

#include "U6Lib_n.h"

U6Lib_n::U6Lib_n()
{
 num_offsets = 0;
 items = NULL;
}


U6Lib_n::~U6Lib_n(void)
{
 close();
}


/* (Re)calculate item offsets from item sizes.
 */
#if 0
void U6Lib_n::calc_item_offsets()
{
    if(num_offsets == 0)
        return;
    if(sizes[0])
        offsets[0] = (num_offsets * lib_size); // skip library index
//    printf("calc_item_offsets: sizes[0] == %d\n", sizes[0]);
//    printf("calc_item_offsets: offsets[0] == %d\n", offsets[0]);
    for(uint32 i = 1; i < num_offsets; i++)
    {
        if(sizes[i])
        {
            offsets[i] = (offsets[i - 1] + sizes[i - 1]);
            if(offsets[i] == 0)
                offsets[i] = (num_offsets * lib_size);
        }
//        printf("calc_item_offsets: sizes[%d] == %d\n", i, sizes[i]);
//        printf("calc_item_offsets: offsets[%d] == %d\n", i, offsets[i]);
    }
}


/* Add a new item to the library after the last item. The library index must
 * already be large enough for the new item and its size to be added.
 */
void U6Lib_n::add_item(unsigned char *src, uint32 src_len)
{
    unsigned char *dcopy = 0;
    if(data.size() >= num_offsets)
        return;
    sizes[data.size()] = src_len;
    if(src_len)
    {
        dcopy = new unsigned char [src_len];
        memcpy(dcopy, src, src_len);
        data.push_back(dcopy);
    }
    else
        data.push_back(0);
}


/* Write the data to the library file, at the offset indicated, for the
 * requested item number. Do not write the data if the indicated offset
 * or size is 0.
 */
void U6Lib_n::write_item(uint32 item_number)
{
    if(item_number >= num_offsets
       || offsets[item_number] == 0 || sizes[item_number] == 0)
        return;
    file.seek(offsets[item_number]);
    file.writeBuf(data[item_number], sizes[item_number]);
}


/* Write the item index to the start of the library file. This is a list of
 * 2 or 4 byte offsets from the start of the file to the start of the data
 * segment for each item.
 */
void U6Lib_n::write_index()
{
    file.seekStart();
    for(uint32 o = 0; o < num_offsets; o++)
    {
        if(lib_size == 2)
            file.write2(offsets[o]);
        else if(lib_size == 4)
            file.write4(offsets[o]);
    }
}


/* Get item offsets from an index file `index_f'. They are in hexadecimal
 * notation, 1 item per line, in the format "offset filename". Filenames are
 * loaded into `names'.
 */
void U6Lib_n::load_index(FILE *index_f)
{
    char input[256] = "", // input line
         offset_str[9] = "", // listed offset
         name[256] = ""; // source file name
    int in_len = 0, oc = 0; // length of input line, character in copy string
    int c = 0, add_count = 0; // character in input line, number of entries
    uint32 offset32;

    if(!index_f)
        return;
    while(fgets(input, 256, index_f))
    {
        in_len = strlen(input);
        // skip spaces, read offset, break on #
        for(c = 0; c < in_len && isspace(input[c]) && input[c] != '#'; c++);
        for(oc = 0; c < in_len && !isspace(input[c]) && input[c] != '#'; c++)
            offset_str[oc++] = input[c];
        offset_str[oc] = '\0';
        // skip spaces, read name, break on #, \n
        for(;c < in_len && isspace(input[c]) && input[c] != '#'; c++);
        for(oc = 0; c < in_len && input[c] != '\n' && input[c] != '#'; c++)
            name[oc++] = input[c];
        name[oc] = '\0';
        if(strlen(offset_str))
        {
            offset32 = strtol(offset_str, NULL, 16);
            add_item_offset(offset32, name);
            ++add_count;
        }
        offset_str[0] = '\0'; oc = 0;
    }
    data.reserve(add_count);
}


/* Append an offset and a name to the library index. The entry for the item's
 * size is added and set to 0. It will be set again when the data is added.
 */
void U6Lib_n::add_item_offset(uint32 offset32, const char *name)
{
    if(!num_offsets)
    {
        offsets = (uint32 *)malloc(sizeof(uint32));
        sizes = (uint32 *)malloc(sizeof(uint32));
    }
    else
    {
        offsets = (uint32 *)realloc(offsets,
                                    sizeof(uint32) * (num_offsets + 1));
        sizes = (uint32 *)realloc(sizes,
                                  sizeof(uint32) * (num_offsets + 1));
    }
    offsets[num_offsets] = offset32;
    names.push_back(name);
    sizes[num_offsets] = 0;
    ++num_offsets;
}


/* Open a new file for writing, with lib_size set to `size'.
 */
bool U6Lib_n::create(std::string &filename, uint8 size)
{
    if(!file.open(filename, "wb"))
    {
        printf("U6Lib: Error opening %s\n", filename.c_str());
        return(false);
    }
    lib_size = size;
    return(true);
}
#endif

bool U6Lib_n::open(std::string &filename, uint8 size, uint8 type)
{
 game_type = type;
 
 if(file.open(filename,"rb") == false)
   {
    printf("Error: Opening %s\n",filename.c_str());
    return false;
   }
 
 lib_size = size;
 this->parse_lib();

 return true;
}

void U6Lib_n::close()
{
 if(items)
  free(items);
 items = NULL;
 
 file.close();
 
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

 file.seek(item->offset);
 
 if(is_compressed(item_number))
  {
   U6Lzw lzw;
   lzw_buf = (unsigned char *)malloc(item->size);
   file.readToBuf(lzw_buf,item->size);
   lzw.decompress_buffer(lzw_buf, item->size, buf, item->uncomp_size);
  }
 else
   file.readToBuf(buf,item->size);
 
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
 
 file.seekStart();
 
 if(game_type != NUVIE_GAME_U6) //U6 doesn't have a 4 byte filesize header.
    {
     skip4 = true;
     filesize = file.read4();
    }
 else
    filesize = file.filesize();
    
 num_offsets = calculate_num_offsets(skip4);
 
 items = (U6LibItem *)malloc(sizeof(U6LibItem) * (num_offsets + 1));
 
 file.seekStart();
 if(skip4)
    file.seek(0x4);
 for(i = 0; i < num_offsets && !file.eof(); i++)
   {
    if(lib_size == 2)
       items[i].offset = file.read2();
    else
      {
       items[i].offset = file.read4();
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
               file.seek(item->offset);
               uncomp_size = file.read4();
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
   file.seek(0x4);
   
 //find first non-zero offset and calculate num_offsets from that.  
 for(i=0;!file.eof();i++)
   {
    if(lib_size == 2)
      offset = file.read2();
    else
      {
       offset = file.read4();
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
