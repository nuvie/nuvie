#ifndef __U6File_h__
#define __U6File_h__

/*
 *  U6File.h
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

#include <string>

#include "U6def.h"

class U6File
{
 protected:
   FILE *f;

 public:

   U6File();
   ~U6File();

   bool open(std::string &directory, std::string &filename, char *mode=NULL);
   bool open(std::string filename, char *mode); // default constructor
   bool open(char *filename, char *mode);
   
   uint8 read1();
   uint16 read2();
   uint32 read4();
   bool readToBuf(unsigned char *buf, uint32 buf_size);
   unsigned char *readBuf(uint32 size, uint32 &bytes_read);
   unsigned char *U6File::readFile();

   uint32 filesize();

   bool seek(uint32 pos);
   bool seekStart();
   bool seekEnd();
   bool eof();
};

#endif /* __U6File_h__ */
