/*
 *  TileManager.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Tue Mar 11 2003.
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

#include "U6File.h"
#include "U6Lzw.h"

#include "TileManager.h"

TileManager::TileManager(Configuration *cfg)
{
 config = cfg;
}

TileManager::~TileManager()
{
 // remove tiles
}

bool TileManager::loadTiles()
{
 std::string path;
 U6File objtiles_vga;
 U6File tileindx_vga;
 U6Lzw *lzw;
 uint32 tile_offset;
 
 unsigned char *tile_data;
 uint32 decomp_size;
 uint32 objtiles_size;
 
 unsigned char *masktype;
 uint32 masktype_size;
 uint16 i;
 
 lzw = new U6Lzw();
 
 config->pathFromValue("config/ultima6/gamedir","maptiles.vga",path);
 tile_data = lzw->decompress_file(path,decomp_size);
 
 config->pathFromValue("config/ultima6/gamedir","objtiles.vga",path);
 objtiles_vga.open(path,"rb");
 
 objtiles_size = objtiles_vga.filesize();
 
 tile_data = (unsigned char *)realloc(tile_data,decomp_size + objtiles_size);
  
 objtiles_vga.readToBuf(&tile_data[decomp_size], objtiles_size);
 
 
 config->pathFromValue("config/ultima6/gamedir","masktype.vga",path);
 masktype = lzw->decompress_file(path,masktype_size);

 config->pathFromValue("config/ultima6/gamedir","tileindx.vga",path);
 tileindx_vga.open(path,"rb");
 
 for(i=0;i<2048;i++)
  {
   tile_offset = tileindx_vga.read2() * 16;
   
   tile[i].transparent = false;
   
   switch(masktype[i])
    {
     case U6TILE_TRANS : tile[i].transparent = true;
     case U6TILE_PLAIN : memcpy(tile[i].data, &tile_data[tile_offset], 256);
                         break;
                         
     case U6TILE_PBLCK : tile[i].transparent = true;
                         decodePixelBlockTile(&tile_data[tile_offset],i);
                         break;
    }
    
   tileindex[i] = i; //set all tile indexs to default value. this is changed in update() for animated tiles
  }

 loadAnimData();
 loadTileFlag();
 
 free(masktype);
 free(tile_data);
 
 //delete objtiles_vga;
 //delete tileindx_vga;
 
 delete lzw;
 
 return true;
}

Tile *TileManager::get_tile(uint16 tile_num)
{
 return &tile[tileindex[tile_num]];
}

void TileManager::update()
{
 uint16 i;
 uint16 current_anim_frame;
 
 // cycle animated tiles

 for (i = 0; i < animdata.number_of_tiles_to_animate; i++)
    {
     current_anim_frame = (game_counter & animdata.and_masks[i]) >> animdata.shift_values[i];

     tileindex[animdata.tile_to_animate[i]] = tileindex[animdata.first_anim_frame[i] + current_anim_frame];
    }

 if(game_counter == 65535)
   game_counter = 0;
 else
   game_counter++;
}

bool TileManager::loadTileFlag()
{
 std::string filename;
 U6File file;
 uint16 i;
 uint8 byte;
 
 config->pathFromValue("config/ultima6/gamedir","tileflag",filename);

 if(file.open(filename,"rb") == false)
   return false;
 
 for(i=0;i < 2048; i++)
  {
   byte = file.read1();
   if(byte & 0x2)
     tile[i].passable = false;
   else
     tile[i].passable = true;
  }

 for(i=0;i < 2048; i++)
  {
   byte = file.read1();
   if(byte & 0x10)
     tile[i].hightile = true;
   else
     tile[i].hightile = false;

   if(byte & 0x40)
     tile[i].dbl_height = true;
   else
     tile[i].dbl_height = false;

   if(byte & 0x80)
     tile[i].dbl_width = true;
   else
     tile[i].dbl_width = false;

    //FIX add article
  }

 return true;
}

bool TileManager::loadAnimData()
{
 std::string filename;
 U6File file;
 uint8 i;
 
 config->pathFromValue("config/ultima6/gamedir","animdata",filename);

 if(file.open(filename,"rb") == false)
   return false;

 if(file.filesize() != 194)
   return false;

 animdata.number_of_tiles_to_animate = file.read2();
 
 for(i=0;i<32;i++)
  {
   animdata.tile_to_animate[i] = file.read2();
  }

 for(i=0;i<32;i++)
  {
   animdata.first_anim_frame[i] = file.read2();
  }
  
  for(i=0;i<32;i++)
  {
   animdata.and_masks[i] = file.read1();
  }
  
  for(i=0;i<32;i++)
  {
   animdata.shift_values[i] = file.read1();
  }
  
 return true;
}

void TileManager::decodePixelBlockTile(unsigned char *tile_data, uint16 tile_num)
{
 uint8 len;
 uint8 i;
 uint16 disp;
 uint8 x;
 unsigned char *ptr;
 unsigned char *data_ptr;
 
// num_blocks = tile_data[0];
 
 ptr = &tile_data[1];
 
 data_ptr = tile[tile_num].data;
   
 memset(data_ptr,0xff,256); //set all pixels to transparent.

 for(i=0; ; i++)
   {
    disp = (ptr[0] + (ptr[1] << 8));
       
    x = disp % 160 + (disp >= 1760 ? 160 : 0);
       
    len = ptr[2];

    if(len == 0)
      break;
    
    data_ptr += x;
    
    memcpy(data_ptr,&ptr[3],len);
    
    data_ptr += len;
    
    ptr += (3+len);
   }
   
 return;
}

