#ifndef __U6LList_h__
#define __U6LList_h__

/*
 *  U6LList.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Mar 15 2003.
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

#define U6LLIST_FREE_DATA true

struct U6Link
{
 U6Link *next;
 U6Link *prev;
 void *data;
 U6Link() {next = NULL; prev = NULL; data = NULL;}
};

class U6LList
{
 U6Link *head;
 U6Link *tail;
 U6Link *cur;

 public:

 U6LList();
 ~U6LList();

 bool add(void *data);
 bool addAtPos(uint32 pos, void *data);

 uint32 findPos(void *data);
 
 bool remove(void *data);
 bool removeAll();

 uint32 count();

 U6Link *start();
 U6Link *end();

 U6Link *next();
 U6Link *prev();
 U6Link *gotoPos(uint32 pos);
};

#endif /* __U6LList_h__ */

