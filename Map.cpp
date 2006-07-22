/*
 *  Map.cpp
 *  Nuvie
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

#include "nuvieDefs.h"
#include "NuvieIOFile.h"

#include "Configuration.h"
#include "Game.h"
#include "TileManager.h"
#include "ActorManager.h"
#include "Map.h"
#include "MapWindow.h"

#include "U6misc.h"


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

Tile *Map::get_tile(uint16 x, uint16 y, uint8 level, bool original_tile)
{
 Tile *map_tile;
 uint8 *ptr;

 if(level > 5)
   return NULL;

 ptr = get_map_data(level);
 wrap_coords(x,y,level, x,y);
 if(original_tile)
    map_tile = tile_manager->get_original_tile(ptr[y * get_width(level) + x]);
 else
    map_tile = tile_manager->get_tile(ptr[y * get_width(level) + x]);

 return map_tile;
}

uint16 Map::get_width(uint8 level)
{
 if(level == 0)
   return 1024; // surface

 return 256; // dungeon
}

bool Map::is_passable(uint16 x, uint16 y, uint8 level)
{
 uint8 *ptr;
 Tile *map_tile;

 wrap_coords(x,y,level, x,y);

 uint8 obj_status = obj_manager->is_passable(x, y, level);
 if(obj_status == OBJ_NOT_PASSABLE)
  {
   return false;
  }

//special case for bridges, hacked doors and dungeon enterances etc.
 if(obj_status != OBJ_NO_OBJ && obj_manager->is_forced_passable(x, y, level))
   return true;

 ptr = get_map_data(level);
 map_tile = tile_manager->get_original_tile(ptr[y * get_width(level) + x]);

 return map_tile->passable;
}

bool Map::is_boundary(uint16 x, uint16 y, uint8 level)
{
 uint8 *ptr;
 Tile *map_tile;

 wrap_coords(x,y,level, x,y);

 ptr = get_map_data(level);
 map_tile = tile_manager->get_tile(ptr[y * get_width(level) + x]);

 if(map_tile->boundary && obj_manager->is_forced_passable(x, y, level) == false)
   return true;

 if(obj_manager->is_boundary(x, y, level))
   return true;

 return false;
}

bool Map::is_water(uint16 x, uint16 y, uint16 level, bool ignore_objects)
{
 uint8 *ptr;
 Tile *map_tile;
 Obj *obj;

 wrap_coords(x,y,level, x,y);

 if(!ignore_objects)
   {
    obj = obj_manager->get_obj(x, y, level);
    if(obj != NULL)
      return false;
   }

 ptr = get_map_data(level);
 map_tile = tile_manager->get_original_tile(ptr[y * get_width(level) + x]);

 if(map_tile->water)
   return true;

 return false;
}

bool Map::is_damaging(uint16 x, uint16 y, uint8 level, bool ignore_objects)
{
    uint8 *ptr=get_map_data(level);
    wrap_coords(x,y,level, x,y);
    Tile *map_tile=tile_manager->get_original_tile(ptr[y*get_width(level) + x]);

    if(map_tile->damages)
        return true;

    if(!ignore_objects)
    {
        if(obj_manager->is_damaging(x, y, level))
            return true;
    }
    return false;
}

uint8 Map::get_impedance(uint16 x, uint16 y, uint8 level, bool ignore_objects)
{
    uint8 *ptr=get_map_data(level);
    wrap_coords(x,y,level, x,y);
    Tile *map_tile=tile_manager->get_original_tile(ptr[y*get_width(level) + x]);
    uint8 impedance = 0;

    if(!ignore_objects)
    {
        Obj *obj = obj_manager->get_obj(x, y, level);
        if(obj != 0)
            impedance += (obj_manager->get_obj_tile(obj->obj_n, obj->frame_n)->flags1 & TILEFLAG_IMPEDANCE)>>TILEFLAG_IMPEDANCE_SHIFT;
    }

    impedance += (map_tile->flags1 & TILEFLAG_IMPEDANCE)>>TILEFLAG_IMPEDANCE_SHIFT;
    return impedance;
}

bool Map::actor_at_location(uint16 x, uint16 y, uint8 level)
{
 wrap_coords(x,y,level, x,y);
 //check for blocking Actor at location.
 if(actor_manager->get_actor(x,y,level) != NULL)
   return true;

 return false;
}

/* Return pointer to actor standing at map coordinates.
 */
Actor *Map::get_actor(uint16 x, uint16 y, uint8 z)
{
    wrap_coords(x,y,z, x,y);
    return(actor_manager->get_actor(x,y,z));
}


const char *Map::look(uint16 x, uint16 y, uint8 level)
{
 unsigned char *ptr;
 uint16 tile_num;
 Obj *obj;
 uint16 qty = 0;

 if(level == 0)
  {
   ptr = surface;
  }
 else
   ptr = dungeons[level - 1];

    wrap_coords(x,y,level, x,y);
    obj = obj_manager->get_obj(x, y, level);
    if(obj != NULL && !(obj->status & OBJ_STATUS_INVISIBLE)) //only show visible objects.
     {
//      tile = tile_manager->get_original_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
//      tile_num = tile->tile_num;
//      qty = obj->qty;
      return obj_manager->look_obj(obj);
     }
    else
      tile_num =  ptr[y * get_width(level) + x];
 return tile_manager->lookAtTile(tile_num,qty,true);
}


bool Map::loadMap(TileManager *tm, ObjManager *om)
{
 std::string filename;
 NuvieIOFileRead map_file;
 NuvieIOFileRead chunks_file;
 unsigned char *map_data;
 unsigned char *map_ptr;
 unsigned char *chunk_data;

 uint8 i;

 tile_manager = tm;
 obj_manager = om;

 config_get_path(config,"map",filename);
 if(map_file.open(filename) == false)
   return false;

 config_get_path(config,"chunks",filename);
 if(chunks_file.open(filename) == false)
   return false;

 map_data = map_file.readAll();
 if(map_data == NULL)
   return false;

 chunk_data = chunks_file.readAll();
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

 free(map_data);
 free(chunk_data);

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


/* Get absolute coordinates for relative destination from MapCoord.
 */
MapCoord MapCoord::abs_coords(sint16 dx, sint16 dy)
{
//    uint16 pitch = Map::get_width(z); cannot call function without object
    uint16 pitch = (z == 0) ? 1024 : 256;
    dx += x;
    dy += y;
    // keep in map boundary
    if(dx < 0)
        dx = 0;
    else if(dx >= pitch)
        dx = pitch - 1;
    if(dy < 0)
        dy = 0;
    else if(dy >= pitch)
        dy = pitch - 1;
    return(MapCoord(dx, dy, z));
}


/* Returns true if this map coordinate is visible in the game window.
 */
bool MapCoord::is_visible()
{
    return(Game::get_game()->get_map_window()->in_window(x, y, z));
}


bool Map::testIntersection(int x, int y, uint8 level, uint8 flags, LineTestResult &Result)
{
/* more checks added, may need more testing (SB-X) */
#if 0
	if (flags & LT_HitUnpassable)
	{
		if (!is_passable (x, y, level))
		{
			Result.init (x, y, level, NULL, obj_manager->get_obj (x, y, level, true));
			return	true;
		}
	}

	if (flags & LT_HitForcedPassable)
	{
		if (obj_manager->is_forced_passable (x, y, level))
		{
			Result.init (x, y, level, NULL, obj_manager->get_obj (x, y, level, true));
			return	true;
		}
	}

	if (flags & LT_HitActors)
	{
		// TODO:
	}

	return	false;
#else
	if (flags & LT_HitUnpassable)
	{
		if (!is_passable (x, y, level))
		{
			Result.init (x, y, level, NULL, obj_manager->get_obj (x, y, level, true));
			return	true;
		}
	}

	if (flags & LT_HitForcedPassable)
	{
		if (obj_manager->is_forced_passable (x, y, level))
		{
			Result.init (x, y, level, NULL, obj_manager->get_obj (x, y, level, true));
			return	true;
		}
	}

	if (flags & LT_HitActors)
	{
		if (actor_manager->get_actor (x, y, level))
		{
			Result.init (x, y, level, actor_manager->get_actor (x, y, level), NULL);
			return	true;
		}
	}

	if ( (flags & LT_HitLocation) && Result.loc_to_hit)
	{
		if (x == Result.loc_to_hit->x && y == Result.loc_to_hit->y)
		{
			Result.init (x, y, level, NULL, NULL);
			Result.loc_to_hit->z = level;
                        Result.hitLoc = Result.loc_to_hit;
			return	true;
		}
	}

	if (flags & LT_HitObjects)
	{
		if (obj_manager->get_obj (x, y, level))
		{
			Result.init (x, y, level, NULL, obj_manager->get_obj (x, y, level, true));
			return	true;
		}
	}

	return	false;
#endif
}

//	returns true if a line hits something travelling from (start_x, start_y) to
//	(end_x, end_y).  If a hit occurs Result is filled in with the relevant info.
bool Map::lineTest(int start_x, int start_y, int end_x, int end_y, uint8 level,
				   uint8 flags, LineTestResult &Result, uint32 skip)
{
	//	standard Bresenham's algorithm.
	int	deltax = abs (end_x - start_x);
	int	deltay = abs (end_y - start_y);

	int	x = start_x;
	int	y = start_y;
	int d;
	int xinc1, xinc2;
	int yinc1, yinc2;
	int dinc1, dinc2;
	uint32 count;


	if (deltax >= deltay)
	{
		d = (deltay << 1) - deltax;

		count = deltax + 1;
		dinc1 = deltay << 1;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = 1;
		xinc2 = 1;
		yinc1 = 0;
		yinc2 = 1;
	}
	else
	{
		d = (deltax << 1) - deltay;

		count = deltay + 1;
		dinc1 = deltax << 1;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = 1;
		yinc2 = 1;
	}

	if (start_x > end_x)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (start_y > end_y)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	for (uint32 i = 0; i < count; i++)
	{
		//	test the current location
		if ((i >= skip) && (testIntersection(x, y, level, flags, Result) == true))
			return	true;

		if (d < 0)
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
		else
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
	}

	return	false;
}

/* Wrap coordinates that have rolled over from the left edge of the map to the
   right side of the map. */
inline void Map::wrap_coords(uint16 x, uint16 y, uint8 z, uint16 &wx, uint16 &wy)
{
    const int coord_max = 65535; // hopefully uint16 is really 16 bits
    uint16 map_width = get_width(z);
    if(x >= map_width)
        x = map_width - (coord_max-x);
    if(y >= map_width)
        y = map_width - (coord_max-y);
    wx = x;
    wy = y;
}
