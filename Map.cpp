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
 win_width = 10;
 win_height = 10;
 
 surface = NULL;
 dungeons = NULL;
 
 cur_x = 0;
 cur_y = 0;
 
 cur_level = 0;
}
 
Map::~Map()
{
 if(surface != NULL)
   free(surface);
   
 // free dungeon levels too.
}
 
bool Map::loadMap(Screen *s, TileManager *tm, ObjManager *om)
{
 std::string filename;
 U6File map_file;
 U6File chunks_file;
 unsigned char *map_data;
 unsigned char *map_ptr;
 unsigned char *chunk_data;

 uint8 i;
 
 screen = s;
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

void Map::set_windowSize(uint16 width, uint16 height)
{
 win_width = width;
 win_height = height;
}
 
void Map::moveWindowLevel(uint8 new_level)
{
 cur_level = new_level;
}

void Map::moveWindow(uint16 new_x, uint16 new_y)
{
 cur_x = new_x;
 cur_y = new_y;
}

void Map::moveWindowRelative(sint16 rel_x, sint16 rel_y)
{
 if(cur_x + rel_x >= 0 && cur_x + rel_x <= 1024 - win_width)
   cur_x += rel_x;
   
 if(cur_y + rel_y >= 0 && cur_y + rel_y <= 1024 - win_height)
   cur_y += rel_y;
}

void Map::get_windowLevel(uint8 *level)
{
 *level = cur_level;
}

void Map::get_windowPos(uint16 *x, uint16 *y)
{
 *x = cur_x;
 *y = cur_y;
}
 
void Map::drawMap()
{
 uint16 i,j;
 unsigned char *map_ptr;
 Tile *tile;
 
 map_ptr = &surface[cur_y * 1024 + cur_x];
 
  for(i=0;i<win_height;i++)
  {
   for(j=0;j<win_width;j++)
     {
      tile = tile_manager->get_tile(map_ptr[j]); 
      screen->blit((unsigned char *)tile->data,8,(j*16),(i*16),16,16);
      //text->drawChar(screen,i*16+j,8+(j*8),8+(i*8));
      //map_ptr++;
     }
   map_ptr += 1024;
  }

 drawObjs();
}

void Map::drawObjs()
{
 uint16 sc_x, sc_y;
 uint16 sc_w, sc_h;
 uint16 x,y;
 
 if(cur_level == 0)
   {
    sc_y = cur_y / 128;
    sc_x = cur_x / 128;
    
    if(cur_x % 128 + win_width > 128)
       sc_w = 2;
    else
       sc_w = 1;

    if(cur_y % 128 + win_height > 128)
       sc_h = 2;
    else
       sc_h = 1;
          
    for(y=sc_y; y < sc_y + sc_h; y++)
      {
       for(x=sc_x; x < sc_x + sc_w; x++)
         drawObjSuperBlock(obj_manager->get_obj_superchunk(x,y,0));
      }      
   }
   
 return;
}

void Map::drawObjSuperBlock(U6LList *superblock)
{
 U6Link *link;
 Obj *obj;
 
 for(link=superblock->start();link != NULL;)
  {
   obj = (Obj *)link->data;
//   if(obj->y > cur_y + win_height)
//      break;

   if(obj->y >= cur_y && obj->y < cur_y + win_height)
     {
      if(obj->x >= cur_x && obj->x < cur_x + win_width)
        {
         //draw here.
         drawObj(obj);
        }
     }
     
   link = superblock->next();
  }
 
}

inline void Map::drawObj(Obj *obj)
{
 //Tile *tile;
 
  //tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n); 
  //screen->blit((char *)tile->data,8,((obj->x - cur_x)*16),((obj->y - cur_y)*16),16,16,tile->transparent);
  
  drawTile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n,obj->x - cur_x, obj->y - cur_y);
  
}

inline void Map::drawTile(uint16 tile_num, uint16 x, uint16 y)
{
 Tile *tile;
 bool dbl_width, dbl_height;
 
 tile = tile_manager->get_tile(tile_num);

 dbl_width = tile->dbl_width;
 dbl_height = tile->dbl_height;
 
 screen->blit(tile->data,8,x*16,y*16,16,16,tile->transparent);
       
 if(dbl_width)
   {
    tile_num--;
    if(x > 0)
      {
       tile = tile_manager->get_tile(tile_num);
       screen->blit(tile->data,8,(x-1)*16,y*16,16,16,tile->transparent);
      }
   }
   
 if(dbl_height)
   {
    tile_num--;
    if(y > 0)
      {
       tile = tile_manager->get_tile(tile_num);
       screen->blit(tile->data,8,x*16,(y-1)*16,16,16,tile->transparent);
      }
   }
   
 if(x > 0 && dbl_width && y > 0 && dbl_height)
   {
    tile_num--;
    tile = tile_manager->get_tile(tile_num);
    screen->blit(tile->data,8,(x-1)*16,(y-1)*16,16,16,tile->transparent);
   }
 
}

//void Map::get_schunk_coords(&uint16 schunk_num, &uint16 schunk_x, &uint16 schunk_y)
//{
//}

