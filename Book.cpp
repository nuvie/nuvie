/*
 *  Book.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Wed Apr 09 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

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
