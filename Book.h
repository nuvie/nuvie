#ifndef __Book_h__
#define __Book_h__

/*
 *  Book.h
 *  Nuive
 *
 *  Created by Eric Fry on Wed Apr 09 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "U6def.h"

#include "Configuration.h"
#include "U6Lib_n.h"

class Book
{
 Configuration *config;
 
 U6Lib_n books;
 
 public:
 
 Book(Configuration *cfg);
 
 bool init();
 char *get_book_data(uint16 num);
};

#endif /* __Book_h__ */

