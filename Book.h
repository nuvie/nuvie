#ifndef __Book_h__
#define __Book_h__

/*
 *  Book.h
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

class Configuration;
class U6Lib_n;

class Book
{
 Configuration *config;

 U6Lib_n *books;

 public:

 Book(Configuration *cfg);
 ~Book();

 bool init();
 char *get_book_data(uint16 num);
};

#endif /* __Book_h__ */

