/*
 *  NuvieFileList.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun May 30 2004.
 *  Copyright (c) 2004 The Nuvie Team. All rights reserved.
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>

#include "nuvieDefs.h"

#include "NuvieFileList.h"

NuvieFileList::NuvieFileList()
{
}

NuvieFileList::~NuvieFileList()
{
}

bool NuvieFileList::open(const char *directory, const char *search, uint8 s_mode)
{
 DIR *dir;
 struct dirent *entry;
 
 search_prefix.assign(search);
 sort_mode = s_mode;

 dir = opendir(directory);
 if(dir == NULL)
   {
    printf("Error: Failed to open %s\n", directory);
    return false;
   }

 for(;(entry = readdir(dir));)
  {
   add_filename(entry->d_name);
  }

 closedir(dir);
 
 list_ptr = file_list.begin();
 
 return true;
}

bool NuvieFileList::add_filename(const char *filename)
{
 const char *sp =  search_prefix.c_str();
 std::string s;
  
 if(filename == NULL || strlen(filename) == 0)
   return false;

 if(!strcmp(".", filename) || !strcmp("..", filename))
   return false;

 if(strlen(sp))
   {
    if(strncmp(sp, filename, strlen(sp)) != 0)
      return false;
   }
 
 s.assign(filename);
 
 file_list.push_front(s);
 
 return true;
}

std::string *NuvieFileList::next()
{
 if(list_ptr != file_list.end())
  {
   std::string *filename = &(*list_ptr);
   list_ptr++;
   
   return filename;
  }

 return NULL;
}

uint32 NuvieFileList::get_num_files()
{
 return file_list.size();
}

void NuvieFileList::close()
{
 return;
}   