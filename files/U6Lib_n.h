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

class U6Lib_n
{
 uint8 lib_size; // measured in bytes either 2 or 4
 uint32 num_offsets;
 uint32 num_zero_offsets;
 uint32 *offsets;
 U6File file;
 
 public:
   
   U6Lib_n(std::string &filename, uint8 size = 2);
   ~U6Lib_n();
 
   uint32 get_num_items();
   uint32 get_item_size(uint32 item_number);
   unsigned char *get_item(uint32 item_number);

 protected:
 
   void parse_lib(); 
   uint32 get_first_offset();
};

#endif /* __U6Lib_n_h__ */
