/*
 *  Book.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Apr 09 2003.
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

#include <string>

#include "U6def.h"
#include "Configuration.h"

#include "Book.h"

Book::Book(Configuration *cfg)
{
 config = cfg;
}

bool Book::init()
{
 std::string filename;
 
 config->pathFromValue("config/ultima6/gamedir","book.dat",filename);
 
 if(books.open(filename,2) == false)
    return false;
 
 return true;
}

char *Book::get_book_data(uint16 num)
{
 if(num >= books.get_num_items())
   return NULL;

 return reinterpret_cast<char*>(books.get_item(num));
}
