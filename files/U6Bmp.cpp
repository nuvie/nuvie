/*
 *  U6Bmp.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Mar 09 2003.
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
#include <stdlib.h>

#include <string>

#include "U6Lzw.h"
#include "U6Bmp.h"

U6Bmp::U6Bmp(): U6Shape()
{
 data = NULL;
}

U6Bmp::~U6Bmp()
{
 if(data != NULL)
   free(data);

 raw = NULL;
}


bool U6Bmp::load(std::string filename)
{
 U6Lzw lzw;
 uint32 data_size;
 
 if(data != NULL)
   return false;

 if(filename.length() == 0)
   return false;

 data = lzw.decompress_file(filename,data_size);

 if(data == NULL)
   return false;
   
 width = (data[0] + (data[1]<<8));
 height = (data[2] + (data[3]<<8));
 
 raw = data + 0x4;
   
 return true;
}
