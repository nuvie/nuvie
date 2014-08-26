/*
 *  NuvieBmpFile.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Nov 10 2013.
 *  Copyright (c) 2013. All rights reserved.
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
#include <stdlib.h>

#include <string>

#include "nuvieDefs.h"
#include "Screen.h"
#include "NuvieBmpFile.h"

#define NUVIEBMPFILE_MAGIC 0x4d42 // 'BM'

NuvieBmpFile::NuvieBmpFile()
{
 data = NULL;
 prev_width = 0;
 prev_height = 0;
 prev_bits = 0;
 memset(&header, 0, sizeof(header));
 memset(&infoHeader, 0, sizeof(infoHeader));
}

NuvieBmpFile::~NuvieBmpFile()
{
 if(data != NULL)
   free(data);
}


bool NuvieBmpFile::load(std::string filename)
{
 NuvieIOFileRead file;

 if(filename.length() == 0)
   return handleError("zero byte file");

 if(!file.open(filename))
 {
   return handleError("opening file");
 }

 if(file.get_size() < 0x36) //invalid header.
 {
   return handleError("filesize < 0x36");
 }

 header.type = file.read2();
 header.size = file.read4();
 header.reserved1 = file.read2();
 header.reserved2 = file.read2();
 header.offset = file.read4();

 infoHeader.size = file.read4();
 infoHeader.width = file.read4();
 infoHeader.height = file.read4();
 infoHeader.planes = file.read2();
 infoHeader.bits = file.read2();
 infoHeader.compression = file.read4();
 infoHeader.imagesize = file.read4();
 infoHeader.xresolution = file.read4();
 infoHeader.yresolution = file.read4();
 infoHeader.ncolours = file.read4();
 infoHeader.importantcolours = file.read4();

 if(header.type != NUVIEBMPFILE_MAGIC) //invalid magic.
 {
   return handleError("invalid BMP magic.");
 }

 if(infoHeader.bits != 8 && infoHeader.bits != 24)
 {
   return handleError("only 256 colour bitmaps supported.");
 }

 if(infoHeader.compression != 0)// && infoHeader.compression != 2)
 {
   return handleError("only uncompressed BMP files are supported");
   //return handleError("only raw and bi_rle8 compression formats are supported.");

   //FIXME need to handle rle compression.
 }

 if(infoHeader.bits == 8)
 {
   for(uint32 i=0;i<infoHeader.ncolours;i++)
   {
     uint8 b = file.read1();
     uint8 g = file.read1();
     uint8 r = file.read1();
     file.read1(); // 0
     palette[i] = (uint32)r|(uint32)(g<<8)|(uint32)(b<<16);
   }
 }

 file.seekStart();
 file.seek(header.offset);

 uint16 bytes_per_pixel = infoHeader.bits / 8;
 uint32 bmp_line_width = infoHeader.width * bytes_per_pixel;
 if(bmp_line_width % 4 != 0)
 {
   bmp_line_width += (4-(bmp_line_width % 4));
 }

 if(data == NULL || infoHeader.width != prev_width || infoHeader.height != prev_height || prev_bits != infoHeader.bits)
 {
   if(data)
   {
     free(data);
   }
   data = (unsigned char *)malloc(infoHeader.width * infoHeader.height * bytes_per_pixel);
   prev_width = infoHeader.width;
   prev_height = infoHeader.height;
   prev_bits = infoHeader.bits;
   if(data == NULL)
   {
     return handleError("allocating memory for image");
   }
 }

 uint32 end = header.offset + bmp_line_width * infoHeader.height;

 uint32 data_width = infoHeader.width * bytes_per_pixel;

 for(sint32 i=0;i<infoHeader.height;i++)
 {
   file.seek(end-bmp_line_width-(bmp_line_width*i));
   file.readToBuf(&data[i*data_width], data_width);
 }

 return true;
}

bool NuvieBmpFile::handleError(std::string error)
{
  if(data)
  {
    free(data);
    data = NULL;
  }

  DEBUG(0,LEVEL_ERROR, error.c_str());

  return false;
}

Tile *NuvieBmpFile::getTile()
{
  if(infoHeader.width != 16 || infoHeader.height != 16 || infoHeader.bits != 8)
  {
    return NULL;
  }

  Tile *t = (Tile *)malloc(sizeof(Tile));
  if(t==NULL)
  {
    return NULL;
  }
  memset(t, 0, sizeof(Tile));
  memcpy(t->data, data, 256);

  return t;
}

unsigned char *NuvieBmpFile::getRawIndexedData()
{
  if(infoHeader.bits != 8)
  {
    return NULL;
  }

  return data;
}

unsigned char *NuvieBmpFile::getRawIndexedDataCopy()
{
  if(data == NULL || infoHeader.bits != 8)
  {
    return NULL;
  }

  unsigned char *copy = (unsigned char *)malloc(infoHeader.width*infoHeader.height);
  if(copy == NULL)
  {
    return NULL;
  }
  memcpy(copy, data, infoHeader.width*infoHeader.height);
  return copy;
}

SDL_Surface *NuvieBmpFile::getSdlSurface8(Screen *screen)
{
  if(data == NULL || infoHeader.bits != 8)
  {
    return NULL;
  }

  return screen->create_sdl_surface_from(data, 8, infoHeader.width, infoHeader.height, infoHeader.width);
}

SDL_Surface *NuvieBmpFile::getSdlSurface32(std::string filename)
{
  load(filename);
  return getSdlSurface32();
}

SDL_Surface *NuvieBmpFile::getSdlSurface32()
{
  uint32 rmask = 0x000000ff;
  uint32 gmask = 0x0000ff00;
  uint32 bmask = 0x00ff0000;
//uint32 amask = 0xff000000;

  if(data == NULL)
  {
    return NULL;
  }

  SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, infoHeader.width, infoHeader.height, 32,
      rmask, gmask, bmask, 0);

  unsigned char *src_buf = data;
  uint32 *pixels = (uint32 *)surface->pixels;

  if(infoHeader.bits == 8)
  {
  for(sint32 i=0;i<infoHeader.height;i++)
    {
     for(sint32 j=0;j<infoHeader.width;j++)
       {
        pixels[j] = palette[src_buf[j]];
       }
     src_buf += infoHeader.width;
     pixels += infoHeader.width;
    }
  }
  else //bits == 24
  {
    for(sint32 i=0;i<infoHeader.height;i++)
      {
       for(sint32 j=0;j<infoHeader.width;j++)
         {
          pixels[j] = (uint32)src_buf[j*3+2]|(uint32)(src_buf[j*3+1]<<8)|(uint32)(src_buf[j*3+0]<<16);
         }
       src_buf += infoHeader.width*3;
       pixels += infoHeader.width;
      }
  }
  return surface;
}
