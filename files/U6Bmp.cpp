/*
 *  U6Bmp.cpp
 *  Nuive
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

U6Bmp::U6Bmp()
{
 width = 0;
 height = 0;
 data = NULL;
 data_size = 0;
}

U6Bmp::~U6Bmp()
{
 if(data != NULL)
   free(data);
}

uint16 U6Bmp::get_width()
{
 return width;
}

uint16 U6Bmp::get_height()
{
 return height;
}

unsigned char *U6Bmp::get_data()
{
 if(data_size < 5)
   return NULL;
   
 return &data[4]; //skip width and height bytes.
}

bool U6Bmp::open(std::string filename)
{
 U6Lzw *lzw;

 if(filename.length() == 0)
   return false;
   
 lzw = new U6Lzw;
 
 data = lzw->decompress_file(filename,data_size);

 delete lzw;

 if(data == NULL)
   return false;
   
 width = (data[0] + (data[1]<<8));
 height = (data[2] + (data[3]<<8));
  
 return true;
}
