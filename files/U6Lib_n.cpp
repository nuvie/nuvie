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
#include "U6Lib_n.h"

U6Lib_n::U6Lib_n()
{
 num_offsets = 0;
 num_zero_offsets = 0;
 offsets = sizes = NULL;
}


U6Lib_n::~U6Lib_n(void)
{
 free(offsets);
 free(sizes);
}


/* (Re)calculate item offsets from item sizes.
 */
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


bool U6Lib_n::open(std::string &filename, uint8 size)
{
 if(file.open(filename,"r") == false)
   {
    printf("Error: Opening %s\n",filename.c_str());
    return false;
   }
 
 lib_size = size;
 
 this->parse_lib();
 
 return true;
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
    return(offsets[item_number]);
}


/* Returns data size of `item_number' calculated from offsets.
 */
uint32 U6Lib_n::get_item_size(uint32 item_number)
{
    uint32 next_offset = 0;

    if(item_number > num_offsets)
        return(0);
    // get next non-zero offset, including the filesaze at offsets[num_offsets]
    for(uint32 o = (item_number + 1); o <= num_offsets; o++)
        if(offsets[o])
        {
            next_offset = offsets[o];
            break;
        }
    if(offsets[item_number] && (next_offset > offsets[item_number]))
        return(next_offset - offsets[item_number]);
    return(0);
}


unsigned char *U6Lib_n::get_item(uint32 item_number)
{
 uint32 item_size;
 unsigned char *buf;
 
 if(item_number >= num_offsets)
   return NULL;
 
 item_size = this->get_item_size(item_number);
 if(item_size == 0)
   return NULL;

 buf = (unsigned char *)malloc(item_size);

 file.seek(get_item_offset(item_number));
 
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

// num_offsets = (first_offset - (num_zero_offsets * lib_size)) / lib_size;
 num_offsets = (first_offset / lib_size);
 offsets = (uint32 *)malloc(sizeof(uint32) * (num_offsets + 1));

// file.seek((num_zero_offsets + 1) * lib_size);
 file.seekStart();
 for(i = 0; i < num_offsets && !file.eof(); i++)
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
