/*
 *  U6File.cpp
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

#include<stdio.h>
#include<stdlib.h>

#include "U6def.h"
#include "U6File.h"

U6File::U6File()
{
 f = NULL;
}


U6File::~U6File()
{
 if(f != NULL)
  fclose(f);
}


bool U6File::open(std::string &directory, std::string &file, char *mode)
{
 std::string filename;
 
 filename = directory;
 
 if(filename[filename.length()] != U6PATH_DELIMITER)
   filename += U6PATH_DELIMITER + file;
 else
   filename += file;

 return open(filename,mode);
}

bool U6File::open(std::string filename, char *mode)
{
 f = fopen(filename.c_str(),mode);

 if(f == NULL)
   return false;

 return true;
}

bool U6File::open(char *filename, char *mode)
{
 f = fopen(filename,mode);
 
 if(f == NULL)
  return false;
  
 return true;
}


uint8 U6File::read1()
{
 return(fgetc(f));
}

uint16 U6File::read2()
{
 unsigned char b0, b1;

 b0 = fgetc(f);
 b1 = fgetc(f);

 return (b0 + (b1<<8));
}

uint32 U6File::read4()
{
   unsigned char b0, b1, b2, b3;
   b0 = fgetc(f);
   b1 = fgetc(f);
   b2 = fgetc(f);
   b3 = fgetc(f);

   return (b0 + (b1<<8) + (b2<<16) + (b3<<24));
}

bool U6File::readToBuf(unsigned char *buf, uint32 buf_size)
{
 fread(buf,1,buf_size,f);

 return true;
}

unsigned char *U6File::readBuf(uint32 size, uint32 &bytes_read)
{
 unsigned char *buf;
 
 buf = (unsigned char *)malloc(size);
 if(buf == NULL)
   {
    bytes_read = 0;
    return NULL;
   }
   
 bytes_read = fread(buf,1,size,f);

 return buf;
}

unsigned char *U6File::readFile()
{
 unsigned char *buf;
 uint32 size;

 size = this->filesize();
 
 this->seekStart();

 buf = (unsigned char *)malloc(size);
 if(buf == NULL)
   return NULL;
    
 fread(buf, 1, size, f);
  
 return buf;
}

uint32 U6File::filesize()
{
 long file_length;
 long cur_pos = ftell(f);

 fseek(f, 0, SEEK_END);
 file_length = ftell(f);
 fseek(f, cur_pos, SEEK_SET);

 return(file_length);
}

bool U6File::seek(uint32 pos)
{
 if(fseek(f,pos,SEEK_SET))
   return true;

 return false;
}

bool U6File::seekStart()
{
 if(fseek(f, 0, SEEK_SET))
   return true;

 return false;
}

bool U6File::seekEnd()
{
 if(fseek(f,0,SEEK_END))
   return true;

 return false;
}

bool U6File::eof()
{
 return (bool)feof(f);
}
