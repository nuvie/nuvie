#ifndef __TileManager_h__
#define __TileManager_h__

/*
 *  TileManager.h
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

#include "U6def.h"
#include "Configuration.h"

// tile types stored in masktype.vga

#define U6TILE_PLAIN 0x0
#define U6TILE_TRANS 0x5
#define U6TILE_PBLCK 0xA

typedef struct {
bool passable;
bool toptile;
bool dbl_width;
bool dbl_height;
bool transparent;

//uint8 qty;
//uint8 flags;
unsigned char data[256];
} Tile;


typedef struct {
uint16 number_of_tiles_to_animate;
uint16 tile_to_animate[0x20];
uint16 first_anim_frame[0x20]; 
uint8 and_masks[0x20];
uint8 shift_values[0x20];
} Animdata;

class TileManager
{
 Tile tile[2048];
 uint16 tileindex[2048]; //used for animated tiles
 uint16 game_counter;
 Animdata animdata;
  
 Configuration *config;
 
 public:
 
   TileManager(Configuration *cfg);
   ~TileManager();
    
   bool loadTiles();
   Tile *get_tile(uint16 tile_num);

   void update();
   
 protected:
   
   bool loadAnimData();
   bool loadTileFlag();
   void decodePixelBlockTile(unsigned char *tile_data, uint16 tile_num);
};

#endif /* __TileManager_h__ */

