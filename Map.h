#ifndef __Map_h__
#define __Map_h__

/*
 *  Map.h
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
#include <cstdlib>
#include "U6def.h"
#include "U6LList.h"

#include "Configuration.h"
#include "Screen.h"
#include "ObjManager.h"

class ActorManager;
class Actor;
class TileManager;

#define MAP_ORIGINAL_TILE true

/* Map Location with 2D X,Y coordinates and plane (map number)
 */
class MapCoord
{
public:
    union { uint16 x; sint16 sx; };
    union { uint16 y; sint16 sy; };
    uint8 z; // plane

    MapCoord(uint16 nx, uint16 ny, uint16 nz) { x = nx; y = ny; z = nz; }

    uint32 xdistance(MapCoord &c2) { return(abs(c2.x - x)); }
    uint32 ydistance(MapCoord &c2) { return(abs(c2.y - y)); }
    // greatest 2D distance X or Y (estimate of shortest)
    uint32 distance(MapCoord &c2)
    {
        uint16 dx = xdistance(c2), dy = ydistance(c2);
        return(dx >= dy ? dx : dy);
    }
    // get absolute coordinates for relative destination (dx,dy)
    MapCoord abs_coords(sint16 dx, sint16 dy);
    // location is on screen?
//    bool is_visible() { return(Game::get_map_window()->in_window(x, y, z)); }
    bool is_visible();

    bool operator==(MapCoord &c2) {return(x == c2.x && y == c2.y && z == c2.z);}
    bool operator!=(MapCoord &c2) { return(!(*this == c2)); }
};


class Map
{
 Configuration *config;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 
 uint8 *surface;
 uint8 *dungeons[5];
  
 public:

 Map(Configuration *cfg);
 ~Map();

 void set_actor_manager(ActorManager *am) { actor_manager = am; }
 Actor *get_actor(uint16 x, uint16 y, uint8 z);

 bool loadMap(TileManager *tm, ObjManager *om);
 unsigned char *get_map_data(uint8 level);
 Tile *get_tile(uint16 x, uint16 y, uint8 level, bool original_tile=false);
 uint16 get_width(uint8 level);
 bool is_passable(uint16 x, uint16 y, uint8 level);
 bool is_water(uint16 x, uint16 y, uint16 level, bool ignore_objects=false);
 bool is_boundary(uint16 x, uint16 y, uint8 level);
 bool actor_at_location(uint16 x, uint16 y, uint8 level);

 const char *look(uint16 x, uint16 y, uint8 level);
 
 protected:
  
 void insertSurfaceSuperChunk(unsigned char *schunk_ptr, unsigned char *chunk_data, uint8 schunk_num);
 void insertSurfaceChunk(unsigned char *chunk, uint16 x, uint16 y);
 
 void insertDungeonSuperChunk(unsigned char *schunk_ptr, unsigned char *chunk_data, uint8 level);
 void insertDungeonChunk(unsigned char *chunk, uint16 x, uint16 y, uint8 level); 
};

#endif /* __Map_h__ */
