/*
 *  TileManager.cpp
 *  Nuvie
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
#include <cmath>
#include <cstdlib>
#include "nuvieDefs.h"

#include "Configuration.h"

#include "Console.h"
#include "NuvieIOFile.h"
#include "U6Lib_n.h"
#include "U6Lzw.h"
#include "Game.h"
#include "Dither.h"
#include "U6misc.h"
#include "Look.h"
#include "GameClock.h"
#include "TileManager.h"
#include "GUI.h"
static char article_tbl[][5] = {"", "a ", "an ", "the "};

static const uint16 U6_ANIM_SRC_TILE[32] = {0x16,0x16,0x1a,0x1a,0x1e,0x1e,0x12,0x12,
                                            0x1a,0x1e,0x16,0x12,0x16,0x1a,0x1e,0x12,
                                            0x1a,0x1e,0x1e,0x12,0x12,0x16,0x16,0x1a,
                                            0x12,0x16,0x1e,0x1a,0x1a,0x1e,0x12,0x16};

//static const uint16 U6_WALL_TYPES[1][2] = {{156,176}};

TileManager::TileManager(Configuration *cfg)
:desc_buf(NULL)
{
 config = cfg;
 look = NULL;
 dither = NULL;
 game_counter = rgame_counter = 0;
 memset(tileindex,0,sizeof(tileindex));
 memset(tile,0,sizeof(tile));
 memset(&animdata,0,sizeof animdata);
}

TileManager::~TileManager()
{
 // remove tiles
 free(desc_buf);
 delete look;
}

bool TileManager::loadTiles()
{
 std::string maptiles_path, masktype_path, path;
 NuvieIOFileRead objtiles_vga;
 NuvieIOFileRead tileindx_vga;
 NuvieIOFileRead file;
 U6Lib_n lib_file;
 U6Lzw *lzw;
 uint32 tile_offset;

 unsigned char *tile_data = NULL;
 uint32 maptiles_size = 0;
 uint32 objtiles_size;

 unsigned char *masktype = NULL;
 uint32 masktype_size;
 uint16 i;

 Dither *dither;
 
 dither = Game::get_game()->get_dither();
 
 config->value("config/GameType",game_type);
 config_get_path(config,"maptiles.vga",maptiles_path);
 config_get_path(config,"masktype.vga",masktype_path);

 lzw = new U6Lzw();

 switch(game_type)
   {
    case NUVIE_GAME_U6 :
                         tile_data = lzw->decompress_file(maptiles_path,maptiles_size);
                         if(tile_data == NULL)
                         {
                            ConsoleAddError("Decompressing " + maptiles_path);
                            return false;
                         }

                         masktype = lzw->decompress_file(masktype_path,masktype_size);
                         if(masktype == NULL)
                         {
                        	 ConsoleAddError("Decompressing " + masktype_path);
                        	 return false;
                         }
                         break;
    case NUVIE_GAME_MD :
    case NUVIE_GAME_SE : if(lib_file.open(maptiles_path,4,game_type) == false)
                         {
                             ConsoleAddError("Opening " + maptiles_path);
                             return false;
                         }
                         maptiles_size = lib_file.get_item_size(0);

                         tile_data = lib_file.get_item(0);
                         lib_file.close();

                         if(lib_file.open(masktype_path,4,game_type) == false)
                         {
                        	 ConsoleAddError("Opening " + masktype_path);
                        	 return false;
                         }
                         //masktype_size = lib_file.get_item_size(0);

                         masktype = lib_file.get_item(0);
                         lib_file.close();
                         break;
   }

 config_get_path(config,"objtiles.vga",path);
 if(objtiles_vga.open(path) == false)
 {
	 ConsoleAddError("Opening " + path);
	 return false;
 }

 objtiles_size = objtiles_vga.get_size();

 tile_data = (unsigned char *)nuvie_realloc(tile_data,maptiles_size + objtiles_size);

 objtiles_vga.readToBuf(&tile_data[maptiles_size], objtiles_size);

 config_get_path(config,"tileindx.vga",path);

 if(tileindx_vga.open(path) == false)
 {
	 ConsoleAddError("Opening " + path);
	 return false;
 }

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

   dither->dither_bitmap(tile[i].data,16,16,tile[i].transparent);
   
   tileindex[i] = i; //set all tile indexs to default value. this is changed in update() for animated tiles
  }

 loadAnimData();
 loadTileFlag();

 free(masktype);
 free(tile_data);

 look = new Look(config);
 if(look->init() == false)
 {
   ConsoleAddError("Initialising Look Class");
   return false;
 }

 desc_buf = (char *)malloc(look->get_max_len() + 6); // add space for "%03d \n\0" or "the \n\0"
 if(desc_buf == NULL)
 {
   ConsoleAddError("Allocating desc_buf");
   return false;
 }

 loadAnimMask();

#ifdef TILEMANAGER_DEBUG

 look->print();

 DEBUG(0,LEVEL_DEBUGGING,"Dumping tile flags:");
 for(i=0;i<2048;i++)
  {
   bool plural;
   DEBUG(1,LEVEL_DEBUGGING,"%04d : ",i);
   print_b(tile[i].flags1);
   DEBUG(1,LEVEL_DEBUGGING," ");
   print_b(tile[i].flags2);
   DEBUG(1,LEVEL_DEBUGGING," ");
   print_b(tile[i].flags3);
   DEBUG(1,LEVEL_DEBUGGING," %s\n",look->get_description(i,&plural));
  }
#endif

 delete lzw;
/*
 std::string datadir = GUI::get_gui()->get_data_dir();
 std::string imagefile;

 build_path(datadir, "images", path);
 datadir = path;
 build_path(datadir, "new_avatar.raw", imagefile);

 file.open(imagefile.c_str());
 for(i=0;i<16;i++)
 {
	 file.readToBuf(tile[i+1776].data, 256);
	 dither->dither_bitmap(tile[i+1776].data,16,16,tile[i+1776].transparent);
 }
 file.readToBuf(tile[1269].data, 256);
 dither->dither_bitmap(tile[1269].data,16,16,tile[1269].transparent);
 file.close();
*/
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


// set entry in tileindex[] to tile num
void TileManager::set_tile_index(uint16 tile_index, uint16 tile_num)
{
    tileindex[tile_index] = tile_num;
}


void TileManager::set_anim_loop(uint16 tile_num, sint8 loopc, uint8 loop)
{
    for(uint32 i = 0; i < 32; i++)
        if(animdata.tile_to_animate[i] == tile_num)
        {
            animdata.loop_count[i] = loopc;
            animdata.loop[i] = loop;
        }
}


const char *TileManager::lookAtTile(uint16 tile_num, uint16 qty, bool show_prefix)
{
 const char *desc;
 bool plural;
 Tile *tile;

 tile = get_original_tile(tile_num);

 if(qty > 1)
   plural = true;
 else
  plural = false;

 desc = look->get_description(tile->tile_num,&plural);
 if(show_prefix == false)
   return desc;

 if(qty > 0 && plural)
   sprintf(desc_buf,"%u %s",qty, desc);
 else
   sprintf(desc_buf,"%s%s",article_tbl[tile->article_n], desc);

 DEBUG(0,LEVEL_DEBUGGING,"%s (%d): flags1:",desc_buf,tile_num);
 print_b(LEVEL_INFORMATIONAL,tile->flags1);
 DEBUG(1,LEVEL_DEBUGGING," f2:");
 print_b(LEVEL_INFORMATIONAL,tile->flags2);
 DEBUG(1,LEVEL_DEBUGGING," f3:");
 print_b(LEVEL_INFORMATIONAL,tile->flags3);
 DEBUG(1,LEVEL_DEBUGGING,"\n");
 
 return desc_buf;
}

bool TileManager::tile_is_stackable(uint16 tile_num)
{
 return look->has_plural(tile_num); // luteijn: FIXME, doesn't take into account Zu Ylem, Silver Snake Venom, and possibly other stackables that don't have a plural defined.
}

void TileManager::update()
{
 uint16 i;
 uint16 current_anim_frame = 0;
 uint16 prev_tileindex;
 uint8 current_hour = Game::get_game()->get_clock()->get_hour();
 static sint8 last_hour = -1;

 // cycle animated tiles

 for(i = 0; i < animdata.number_of_tiles_to_animate; i++)
    {
     if(animdata.loop_count[i] != 0)
       {
        if(animdata.loop[i] == 0) // get next frame
          current_anim_frame = (game_counter & animdata.and_masks[i]) >> animdata.shift_values[i];
        else if(animdata.loop[i] == 1) // get previous frame
          current_anim_frame = (rgame_counter & animdata.and_masks[i]) >> animdata.shift_values[i];
        prev_tileindex = tileindex[animdata.tile_to_animate[i]];
        tileindex[animdata.tile_to_animate[i]] = tileindex[animdata.first_anim_frame[i] + current_anim_frame];
        // loop complete if back to first frame (and not infinite loop)
        if(animdata.loop_count[i] > 0
           && tileindex[animdata.tile_to_animate[i]] != prev_tileindex
           && tileindex[animdata.tile_to_animate[i]] == tileindex[animdata.first_anim_frame[i]])
          --animdata.loop_count[i];
       }
     else // not animating
        tileindex[animdata.tile_to_animate[i]] = tileindex[animdata.first_anim_frame[i]];
    }

 if(Game::get_game()->anims_paused() == false) // update counter
 {
    if(game_counter == 65535)
      game_counter = 0;
    else
      game_counter++;
    if(rgame_counter == 0)
      rgame_counter = 65535;
    else
      rgame_counter--;
 }
 // cycle time-based animations
 if(current_hour != last_hour)
   update_timed_tiles(current_hour);
 last_hour = current_hour;
}


bool TileManager::loadTileFlag()
{
 std::string filename;
 NuvieIOFileRead file;
 uint16 i;

 config_get_path(config,"tileflag",filename);

 if(file.open(filename) == false)
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

   if(tile[i].flags1 & 0x8)
     tile[i].damages = true;
   else
     tile[i].damages = false;
  }

 for(i=0;i < 2048; i++)
  {
   tile[i].flags2 = file.read1();
   if(tile[i].flags2 & 0x10)
     tile[i].toptile = true;
   else
     tile[i].toptile = false;

   if(tile[i].flags2 & 0x4 || tile[i].flags2 & 0x8)
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
 NuvieIOFileRead file;
 uint8 i;
 int game_type;
 config->value("config/GameType",game_type);
 config_get_path(config,"animdata",filename);

 if(file.open(filename) == false)
   return false;

 if(file.get_size() != 194)
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

  for(i=0;i<32;i++) // FIXME: any data on which tiles don't start as animated?
  {
   animdata.loop[i] = 0; // loop forwards
   if((game_type == NUVIE_GAME_U6 &&
      (animdata.tile_to_animate[i] == 862 // Crank
      || animdata.tile_to_animate[i] == 1009 // Crank
      || animdata.tile_to_animate[i] == 1020)) // Chain
      || (game_type == NUVIE_GAME_MD
      && animdata.tile_to_animate[i] >= 16 && animdata.tile_to_animate[i] <= 23)) // canal
    animdata.loop_count[i] = 0; // don't start animated
   else
    animdata.loop_count[i] = -1; // infinite animation
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
 int game_type;

 config->value("config/GameType",game_type);
 if(game_type != NUVIE_GAME_U6)                //only U6 has animmask.vga
   return true;

 config_get_path(config,"animmask.vga",filename);

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


/* Update tiles for timed-based animations.
 */
void TileManager::update_timed_tiles(uint8 hour)
{
    uint16 new_tile;
//    switch(Game::get_game()->get_game_type())
//    {
//        case NUVIE_GAME_U6:
            // sundials
            if(hour >= 5 && hour <= 6)
                new_tile = 328;
            else if(hour >= 7 && hour <= 8)
                new_tile = 329;
            else if(hour >= 9 && hour <= 10)
                new_tile = 330;
            else if(hour >= 11 && hour <= 12)
                new_tile = 331;
            else if(hour >= 13 && hour <= 14)
                new_tile = 332;
            else if(hour >= 15 && hour <= 16)
                new_tile = 333;
            else if(hour >= 17 && hour <= 18)
                new_tile = 334;
            else if(hour >= 19 && hour <= 20)
                new_tile = 335;
            else // 9pm to 5am
                new_tile = 861;
            set_tile_index(861, new_tile);
//            break;
//    }
}


/* Returns tile rotated about the center by `rotate' degrees. (8-bit; clipped to
 * standard 16x16 size) It must be deleted after use.
 * **Fixed-point rotate function taken from the SDL Graphics Extension library
 * (SGE) (c)1999-2003 Anders Lindström, licensed under LGPL v2+.**
 */
Tile *TileManager::get_rotated_tile(Tile *tile, float rotate, uint8 src_y_offset)
{
    Tile *new_tile = new Tile(*tile); // retain properties of original tile
    get_rotated_tile(tile, new_tile, rotate, src_y_offset);

    return new_tile;
}

void TileManager::get_rotated_tile(Tile *tile, Tile *dest_tile, float rotate, uint8 src_y_offset)
{
    unsigned char tile_data[256];

    memset(&dest_tile->data, 255, 256); // fill output with transparent color

    Sint32 dy, sx, sy;
    Sint16 rx, ry;
    Uint16 px = 8, py = 8; // rotate around these coordinates
    Uint16 xmin = 0, xmax = 15, ymin = 0, ymax = 15; // size
    Uint16 sxmin = xmin, sxmax = xmax, symin = ymin, symax = ymax;
    Uint16 qx = 8, qy = 8; // ?? don't remember

    float theta = float(rotate*M_PI/180.0);  /* Convert to radians.  */

    Sint32 const stx = Sint32((sin(theta)) * 8192.0);
    Sint32 const ctx = Sint32((cos(theta)) * 8192.0);
    Sint32 const sty = Sint32((sin(theta)) * 8192.0);
    Sint32 const cty = Sint32((cos(theta)) * 8192.0);
    Sint32 const mx = Sint32(px*8192.0);
    Sint32 const my = Sint32(py*8192.0);

    Sint32 const dx = xmin - qx;
    Sint32 const ctdx = ctx*dx;
    Sint32 const stdx = sty*dx;

    Sint32 const src_pitch=16;
    Sint32 const dst_pitch=16;
    Uint8 const *src_row = (Uint8 *)&tile->data;
    Uint8 const *dst_pixels = (Uint8 *)&dest_tile->data;
    Uint8 *dst_row;

    if(src_y_offset > 0 && src_y_offset < 16) //shift source down before rotating. This is used by bolt and arrow tiles.
    {
        memset(&tile_data, 255, 256);
        memcpy(&tile_data[src_y_offset*16], &tile->data, 256-(src_y_offset*16));
    	src_row = (Uint8 *)&tile_data;
    }

    for(uint32 y=ymin; y<ymax; y++)
    {
        dy = y - qy;

        sx = Sint32(ctdx  + stx*dy + mx);  /* Compute source anchor points */
        sy = Sint32(cty*dy - stdx  + my);

        /* Calculate pointer to dst surface */
        dst_row = (Uint8 *)dst_pixels + y*dst_pitch;

        for(uint32 x=xmin; x<xmax; x++)
        {
            rx=Sint16(sx >> 13);  /* Convert from fixed-point */
            ry=Sint16(sy >> 13);

            /* Make sure the source pixel is actually in the source image. */
            if( (rx>=sxmin) && (rx<=sxmax) && (ry>=symin) && (ry<=symax) )
                *(dst_row + x) = *(src_row + ry*src_pitch + rx);

            sx += ctx;  /* Incremental transformations */
            sy -= sty;
        }
    }


    //memcpy(&dest_tile->data, &tile_data, 256); // replace data

    return;
}


#if 0 /* old */
Tile *TileManager::get_rotated_tile(Tile *tile, float rotate)
{
    float angle = (rotate != 0) ? (rotate * M_PI) / 180.0 : 0; // radians
    const float mul_x1 = cos(angle);  // | x1  y1 |
    const float mul_y1 = sin(angle);  // | x2  y2 |
    const float mul_x2 = -mul_y1;
    const float mul_y2 = mul_x1;
    unsigned char tile_data[256];
    unsigned char *input = (unsigned char *)&tile->data, *output;

    memset(&tile_data, 255, 256); // fill output with transparent color

    for(sint8 y = -8; y < 8; y++) // scan input pixels
    {
        for(sint8 x = -8; x < 8; x++)
        {
            sint8 rx = (sint8)rint((x * mul_x1) + (y * mul_x2)); // calculate target pixel
            sint8 ry = (sint8)rint((x * mul_y1) + (y * mul_y2));
            if(rx >= -8 && rx <= 7 && ry >= -8 && ry <= 7)
            {
                output = (unsigned char *)&tile_data;
                output += (ry + 8) * 16;
                output[rx + 8] = input[x + 8]; // copy
                if(rx <= 6) output[rx + 8 + 1] = input[x + 8]; // copy again to adjacent pixel
            }
        }
        input += 16; // next line
    }

    Tile *new_tile = new Tile(*tile); // retain properties of original tile
    memcpy(&new_tile->data, &tile_data, 256); // replace data
    return(new_tile);
}
#endif

Tile *TileManager::get_cursor_tile()
{
	Tile *cursor_tile = NULL;
	switch(game_type)
	{
	case NUVIE_GAME_U6 : cursor_tile = get_tile(365);
	break;

	case NUVIE_GAME_MD : cursor_tile = get_tile(265);
	break;

	case NUVIE_GAME_SE : cursor_tile = get_tile(381);
	break;
	}

	return cursor_tile;
}

Tile *TileManager::get_use_tile()
{
	Tile *use_tile = NULL;
	switch(game_type)
	{
	case NUVIE_GAME_U6 : use_tile = get_tile(364);
	break;

	case NUVIE_GAME_MD : use_tile = get_tile(264);
	break;

	case NUVIE_GAME_SE : use_tile = get_tile(380);
	break;
	}

	return use_tile;
}
