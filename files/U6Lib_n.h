#ifndef __U6Lib_n_h__
#define __U6Lib_n_h__

/*
 *  U6Lib_n.h
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
#include "U6def.h"
#include "U6File.h"
#include <vector>

using std::string;
using std::vector;


class U6Lib_n
{
 uint8 lib_size; // measured in bytes either 2 or 4
 uint32 num_offsets; // number of items, size of lists
 uint32 num_zero_offsets;
 uint32 *offsets, *sizes;
 U6File file;
 vector<string> names; // names from index file, same number as offsets
 vector<unsigned char *> data; // data-buffers for each item

public:
   U6Lib_n();
   ~U6Lib_n();
 
   bool open(std::string &filename, uint8 size);
   bool create(std::string &filename, uint8 size);
   uint32 get_num_items();
   uint32 get_item_size(uint32 item_number);
   uint32 get_item_offset(uint32 item_number);
   void add_item_offset(uint32 offset32, const char *name);
   unsigned char *get_item(uint32 item_number);
   void load_index(FILE *index_f);
   void write_index();
   void write_item(uint32 item_number);
   /* Returns the filename associated with `item_number'.
    */
   const char *item_name(uint32 item_number)
   {
    if(item_number >= num_offsets)
        return(NULL);
    return(names[item_number].c_str());
   }
   void add_item(unsigned char *src, uint32 src_len);
   unsigned char *item(uint32 item_number) { return(data[item_number]); }
   /* Returns the number of bytes to `item_number'. It does not calculate the
    * size as `get_item_size' does - it gets it from the sizes list.
    */
   uint32 item_size(uint32 item_number)
   {
    if(item_number >= num_offsets)
        return(0);
    return(sizes[item_number]);
   }
   /* Write all items to the library file at their respective offsets from the
    * start of the file.
    */   
   void write_items()
   {
       for(int i = 0; i < num_offsets; i++)
        write_item(i);
   }
   void calc_item_offsets();
protected:
   void parse_lib(); 
   uint32 get_first_offset();
};

#if 0
class U6ConverseLib: U6Lib_n
{
private:
    uint32 zero_offset_count;
    string *conversefile;
};
#endif

#endif /* __U6Lib_n_h__ */
