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

//#include "misc.h"

#include "TileManager.h"

void print_b(uint8 num);

static char article_tbl[][5] = {"", "a ", "an ", "the "};

static const uint16 U6_ANIM_SRC_TILE[32] = {0x16,0x16,0x1a,0x1a,0x1e,0x1e,0x12,0x12,
                                            0x1a,0x1e,0x16,0x12,0x16,0x1a,0x1e,0x12,
                                            0x1a,0x1e,0x1e,0x12,0x12,0x16,0x16,0x1a,
                                            0x12,0x16,0x1e,0x1a,0x1a,0x1e,0x12,0x16};

TileManager::TileManager(Configuration *cfg)
{
 config = cfg;
}

TileManager::~TileManager()
{
 // remove tiles
 free(desc_buf);
 delete look;
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
   tile[i].tile_num = i;
   
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
 
 look = new Look(config);
 if(look->init() == false)
   return false;

 desc_buf = (char *)malloc(look->get_max_len() + 6); // add space for "%03d \n\0" or "the \n\0"
 if(desc_buf == NULL)
   return false;
   
 loadAnimMask();
 
#ifdef DEBUG

 look->print();

 for(i=0;i<2048;i++)
  {
   printf("%04d : ",i);
   print_b(tile[i].flags1);
   printf(" ");
   print_b(tile[i].flags2);
   printf(" ");
   print_b(tile[i].flags3);
   printf(" %s\n",look->get_description(i));
  }
#endif
 
 delete lzw;
 
 return true;
}

Tile *TileManager::get_tile(uint16 tile_num)
{
 return &tile[tileindex[tile_num]];
}

Tile *TileManager::get_anim_base_tile(uint16 tile_num)
{
 return &tile[tileindex[U6_ANIM_SRC_TILE[tile_num-16]/2]];
}

Tile *TileManager::get_original_tile(uint16 tile_num)
{
 return &tile[tile_num];
}

char *TileManager::lookAtTile(uint16 tile_num, uint16 qty, bool show_prefix)
{
 char *desc;
 bool plural;
 Tile *tile;
 
 tile = get_tile(tile_num);
 
 if(qty > 1)
   plural = true;
 else
  plural = false;
  
 desc = look->get_description(tile->tile_num,plural);
 if(show_prefix == false)
   return desc;

 if(qty > 0)
   sprintf(desc_buf,"%d %s",qty, desc);
 else
   sprintf(desc_buf,"%s%s",article_tbl[tile->article_n], desc);
   
 return desc_buf;
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
 
 config->pathFromValue("config/ultima6/gamedir","tileflag",filename);

 if(file.open(filename,"rb") == false)
   return false;
 
 for(i=0;i < 2048; i++)
  {
   tile[i].flags1 = file.read1();
   if(tile[i].flags1 & 0x2)
     tile[i].passable = false;
   else
     tile[i].passable = true;
     
   if(tile[i].flags1 & 0x1)
     tile[i].water = true;
   else
     tile[i].water = false;

  }

 for(i=0;i < 2048; i++)
  {
   tile[i].flags2 = file.read1();
   if(tile[i].flags2 & 0x10)
     tile[i].toptile = true;
   else
     tile[i].toptile = false;

   if(tile[i].flags2 & 0x4 || tile[i].flags2 & 0x8) // 0x4 normal boundary, 0x8 window FIX to handle these properly.
     tile[i].boundary = true;
   else
     tile[i].boundary = false;


   if(tile[i].flags2 & 0x40)
     tile[i].dbl_height = true;
   else
     tile[i].dbl_height = false;

   if(tile[i].flags2 & 0x80)
     tile[i].dbl_width = true;
   else
     tile[i].dbl_width = false;
  }

 file.seek(0x1400);

 for(i=0;i < 2048; i++) // '', 'a', 'an', 'the'
  {
   tile[i].flags3 = file.read1();
   tile[i].article_n = (tile[i].flags3 & 0xC0) >> 6;
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


//FIX this is U6 specific.

bool TileManager::loadAnimMask()
{
 std::string filename;
 U6Lzw lzw;
 uint16 i;
 unsigned char *animmask;
 unsigned char *mask_ptr;
 uint32 animmask_size;
 
 unsigned char *tile_data;
 uint16 bytes2clear;
 uint16 displacement;
 
 config->pathFromValue("config/ultima6/gamedir","animmask.vga",filename);
 animmask = lzw.decompress_file(filename,animmask_size);
 
 if(animmask == NULL)
   return false;

 for(i=0;i<32;i++) // Make the 32 tiles from index 16 onwards transparent with data from animmask.vga
  {
   tile_data = tile[16+i].data;
   tile[16+i].transparent = true;
   
   mask_ptr = animmask + i * 64;
   bytes2clear = mask_ptr[0];
      
   if(bytes2clear != 0)
     memset(tile_data,0xff,bytes2clear);

   tile_data += bytes2clear;
   mask_ptr++;
   
   displacement = mask_ptr[0];
   bytes2clear = mask_ptr[1];
   
   mask_ptr += 2;
   
   for(;displacement != 0 && bytes2clear != 0;mask_ptr += 2)
     {
      tile_data += displacement;
      
      memset(tile_data,0xff,bytes2clear);
      tile_data += bytes2clear;
      
      displacement = mask_ptr[0];
      bytes2clear = mask_ptr[1];
     }
  }  
 
 free(animmask);
 
 return true;
}
   
void print_b(uint8 num)
{
 sint8 i;
 
 for(i=7;i>=0;i--)
 {
  if(num & (1<<i))
    printf("1");
  else
    printf("o");
 }
 
 return;
}
