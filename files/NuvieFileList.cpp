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

#include "nuvieDefs.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>
#include <dirent.h>

#include "U6misc.h"

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
   add_filename(directory, entry->d_name);
  }

 closedir(dir);

 file_list.sort(NuvieFileDesc()); //sort list by time last modified in decending order.

 list_ptr = file_list.begin();

 return true;
}

bool NuvieFileList::add_filename(const char *directory, const char *filename)
{
 struct stat sb;
 const char *sp =  search_prefix.c_str();
 NuvieFileDesc filedesc;
 std::string fullpath;

 if(filename == NULL || strlen(filename) == 0)
   return false;

 if(!strcmp(".", filename) || !strcmp("..", filename))
   return false;

 if(strlen(sp))
   {
    if(strncmp(sp, filename, strlen(sp)) != 0)
      return false;
   }

 build_path(directory, filename, fullpath);

 if(stat(fullpath.c_str(), &sb) != 0)
   {
    printf("Error: Couldn't stat() file %s\n", fullpath.c_str());
    return false;
   }

 filedesc.m_time = sb.st_mtime;
 filedesc.filename.assign(filename);

 file_list.push_front(filedesc);

 return true;
}

std::string *NuvieFileList::next()
{
 if(list_ptr != file_list.end())
  {

   std::string *filename = &((*list_ptr).filename);
   list_ptr++;

   return filename;
  }

 return NULL;
}

std::string *NuvieFileList::get_latest()
{
 std::list<NuvieFileDesc>::iterator iter;

 iter = file_list.begin();

 if(iter != file_list.end())
  {
   std::string *filename = &((*iter).filename);

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
