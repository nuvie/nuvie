/*
 *  Map.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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
#include <string>

#include "U6def.h"
#include "U6File.h"

#include "Configuration.h"
#include "TileManager.h"

#include "Map.h"

Map::Map(Configuration *cfg)
{
 config = cfg;
 
 surface = NULL;

}
 
Map::~Map()
{
 uint8 i;
 
 if(surface == NULL)
   return;
   
 free(surface);
   
 for(i=0;i<5;i++)
   free(dungeons[i]);
}

unsigned char *Map::get_map_data(uint8 level)
{
 if(level == 0)
   return surface;
 
 if(level > 5)
   return NULL;
 
 return dungeons[level - 1];
}

uint16 Map::get_width(uint8 level)
{
 if(level == 0)
   return 1024; // surface
   
 return 256; // dungeon
}

bool Map::loadMap(TileManager *tm, ObjManager *om)
{
 std::string filename;
 U6File map_file;
 U6File chunks_file;
 unsigned char *map_data;
 unsigned char *map_ptr;
 unsigned char *chunk_data;

 uint8 i;
 
 tile_manager = tm;
 obj_manager = om;
 
 config->pathFromValue("config/ultima6/gamedir","map",filename);
 if(map_file.open(filename,"rb") == false)
   return false;

 config->pathFromValue("config/ultima6/gamedir","chunks",filename);
 if(chunks_file.open(filename,"rb") == false)
   return false;

 map_data = map_file.readFile();
 if(map_data == NULL)
   return false;
   
 chunk_data = chunks_file.readFile();
 if(chunk_data == NULL)
   return false;

 map_ptr = map_data;

surface = (unsigned char *)malloc(1024 * 1024);
if(surface == NULL)
  return false;
   
 for(i=0;i<64;i++)
   {
    insertSurfaceSuperChunk(map_ptr,chunk_data,i);
    map_ptr += 384;
   }

 for(i=0;i < 5; i++)
   {
    dungeons[i] = (unsigned char *)malloc(256 * 256);
    if(dungeons[i] == NULL)
      return false;
      
    insertDungeonSuperChunk(map_ptr,chunk_data,i);
    map_ptr += 1536;
   }

 return true;      
}

void Map::insertSurfaceSuperChunk(unsigned char *schunk, unsigned char *chunk_data, uint8 schunk_num)
{
 uint16 world_x, world_y;
 uint16 c1,c2;
 uint8 i,j;
 
 world_x = schunk_num % 8;
 world_y = (schunk_num - world_x) / 8;
 
 world_x *= 128;
 world_y *= 128;
 
 for(i=0;i<16;i++)
   {
    for(j=0;j<16;j += 2)
      {
       c1 = ((schunk[1] & 0xf) << 8) | schunk[0];
       c2 = (schunk[2] << 4) | (schunk[1] >> 4);
       
       insertSurfaceChunk(&chunk_data[c1*64],world_x + j * 8, world_y + i * 8);
       insertSurfaceChunk(&chunk_data[c2*64],world_x + (j+1) * 8, world_y + i * 8);
       
       schunk += 3;
      }
   }
}

void Map::insertSurfaceChunk(unsigned char *chunk, uint16 x, uint16 y)
{
 unsigned char *map_ptr;
 uint8 i;
 
 map_ptr = &surface[y * 1024 + x];
 
 for(i=0;i<8;i++)
   {
    memcpy(map_ptr,chunk,8);
    map_ptr += 1024;
    chunk += 8;
   }

}

void Map::insertDungeonSuperChunk(unsigned char *schunk, unsigned char *chunk_data, uint8 level)
{
 uint16 c1,c2;
 uint8 i,j;
  
 for(i=0;i<32;i++)
   {
    for(j=0;j<32;j += 2)
      {
       c1 = ((schunk[1] & 0xf) << 8) | schunk[0];
       c2 = (schunk[2] << 4) | (schunk[1] >> 4);
       
       insertDungeonChunk(&chunk_data[c1*64], j * 8, i * 8,level);
       insertDungeonChunk(&chunk_data[c2*64], (j+1) * 8, i * 8,level);
       
       schunk += 3;
      }
   }
}

void Map::insertDungeonChunk(unsigned char *chunk, uint16 x, uint16 y, uint8 level)
{
 unsigned char *map_ptr;
 uint8 i;
 
 map_ptr = &dungeons[level][y * 256 + x];
 
 for(i=0;i<8;i++)
   {
    memcpy(map_ptr,chunk,8);
    map_ptr += 256;
    chunk += 8;
   }

}
