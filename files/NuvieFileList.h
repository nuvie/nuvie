#ifndef __NuvieFileList_h__
#define __NuvieFileList_h__
/*
 *  NuvieFileList.h
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
#include <sys/types.h>
#include <string>
#include <list>
#include <set>

using std::list;
using std::string;


#define NUVIE_SORT_TIME_DESC 0x1
#define NUVIE_SORT_TIME_ASC  0x2
#define NUVIE_SORT_NAME_DESC 0x3
#define NUVIE_SORT_NAME_ASC  0x5

class Configuration;

class NuvieFileDesc
{
 public:

 std::string filename;
 time_t m_time;

 bool operator<(const NuvieFileDesc &rhs) const { return (rhs.m_time < this->m_time); };
 bool operator()(const NuvieFileDesc &lhs, const NuvieFileDesc &rhs) { return (lhs.m_time > rhs.m_time); };
};

class NuvieFileList
{
 protected:

 std::list<NuvieFileDesc> file_list;
 std::list<NuvieFileDesc>::iterator list_ptr;

 std::string search_prefix;
 uint8 sort_mode;

 public:

   NuvieFileList();
   ~NuvieFileList();

   bool open(const char *directory, const char *restrict, uint8 sort_mode);


   std::string *next();
   std::string *get_latest();
   uint32 get_num_files();

   std::set<std::string> get_filenames();

   void close();

 protected:

 bool add_filename(const char *directory, const char *filename);

};

#endif /* __NuvieFileList_h__ */
