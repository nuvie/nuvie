#ifndef __TileManager_h__
#define __TileManager_h__

/*
 *  TileManager.h
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

#include "nuvieDefs.h"

class Configuration;
class Look;

// tile types stored in masktype.vga

#define U6TILE_PLAIN 0x0
#define U6TILE_TRANS 0x5
#define U6TILE_PBLCK 0xA

#define TILEFLAG_WALL_MASK  0xf0 // 11110000
//flags1
#define TILEFLAG_WALL_NORTH 0x80
#define TILEFLAG_WALL_EAST  0x40
#define TILEFLAG_WALL_SOUTH 0x20
#define TILEFLAG_WALL_WEST  0x10
#define TILEFLAG_DAMAGING 0x8
#define TILEFLAG_WALL     0x4
#define TILEFLAG_BLOCKING 0x2
#define TILEFLAG_WATER    0x1

//flags2
#define TILEFLAG_BOUNDARY         0x4
#define TILEFLAG_WINDOW           0x8
#define TILEFLAG_MISSILE_BOUNDARY  0x20
#define TILEFLAG_DOUBLE_HEIGHT    0x40
#define TILEFLAG_DOUBLE_WIDTH     0x80

//flags3
#define TILEFLAG_CAN_PLACE_ONTOP 0x2
#define TILEFLAG_FORCED_PASSABLE 0x4

// FIXME These should probably go else where.
#define TILE_U6_SLING_STONE 398
#define TILE_U6_ARROW       566
#define TILE_U6_BOLT        567

typedef struct {
uint16 tile_num;
bool passable;
bool water;
bool toptile;
bool dbl_width;
bool dbl_height;
bool transparent;
bool boundary;
bool damages;
uint8 article_n;
//uint8 qty;
//uint8 flags;

uint8 flags1;
uint8 flags2;
uint8 flags3;

unsigned char data[256];
} Tile;


typedef struct {
uint16 number_of_tiles_to_animate;
uint16 tile_to_animate[0x20];
uint16 first_anim_frame[0x20];
uint8 and_masks[0x20];
uint8 shift_values[0x20];
sint8 loop_count[0x20]; // times to animate (-1 = infinite)
uint8 loop[0x20]; // 0 = loop forwards, 1 = backwards
} Animdata;

class TileManager
{
 Tile tile[2048];
 uint16 tileindex[2048]; //used for animated tiles
 uint16 game_counter, rgame_counter;
 Animdata animdata;
 Look *look;

 char *desc_buf; // for look
 Configuration *config;

 public:

   TileManager(Configuration *cfg);
   ~TileManager();

   bool loadTiles();
   Tile *get_tile(uint16 tile_num);
   Tile *get_anim_base_tile(uint16 tile_num);
   Tile *get_original_tile(uint16 tile_num);
   void set_tile_index(uint16 tile_index, uint16 tile_num);
   uint16 get_tile_index(uint16 tile_index) { return(tileindex[tile_index]); }
   void set_anim_loop(uint16 tile_num, sint8 loopc, uint8 loop = 0);

   const char *lookAtTile(uint16 tile_num, uint16 qty, bool show_prefix);
   bool tile_is_stackable(uint16 tile_num);
   void update();
   void update_timed_tiles(uint8 hour);

   Tile *get_rotated_tile(Tile *tile, float rotate);

 protected:

   bool loadAnimData();
   bool loadTileFlag();
   void decodePixelBlockTile(unsigned char *tile_data, uint16 tile_num);

   bool loadAnimMask();
};

#endif /* __TileManager_h__ */

