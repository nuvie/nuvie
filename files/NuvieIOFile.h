#ifndef __NuvieIOFile_h__
#define __NuvieIOFile_h__
/*
 *  NuvieIOFile.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jul 05 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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
#include <string>

#include "U6def.h"

#include "NuvieIO.h"


class NuvieIOFile : public NuvieIO
{
 protected:
 FILE *fp;
 
 public:
 
   NuvieIOFile();
   virtual ~NuvieIOFile();
   
   virtual bool open(const char *filename) { return false; };
   bool open(std::string filename) { return open(filename.c_str()); };
   
   void close();
     
   void seek(uint32 new_pos);
   
 protected:
    bool openWithMode(const char *filename, const char *mode);
    uint32 get_filesize();
};

class NuvieIOFileRead : public NuvieIOFile
{
 protected:
 
 public:
 
  // NuvieIOFileRead();
  // virtual ~NuvieIOFileRead();

   bool open(const char *filename);
   bool open(std::string filename) { return open(filename.c_str()); };
        
   uint8 read1();
   uint16 read2();
   uint32 read4();

   bool readToBuf(unsigned char *buf, uint32 buf_size);
};

class NuvieIOFileWrite : public NuvieIOFileRead
{
 
 public:
 
  // NuvieIOFileRead();
  // virtual ~NuvieIOFileRead();

   bool open(const char *filename);
   bool open(std::string filename) { return open(filename.c_str()); };
        
   bool write1(uint8 src);
   bool write2(uint16 src);
   bool write4(uint32 src);
   uint32 writeBuf(unsigned char *src, uint32 src_size);
   uint32 write(NuvieIO *src);

};
#endif /* __NuvieIOFile_h__ */
