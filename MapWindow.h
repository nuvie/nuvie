#ifndef __MapWindow_h__
#define __MapWindow_h__

/*
 *  MapWindow.h
 *  Nuive
 *
 *  Created by Eric Fry on Fri Mar 21 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include "U6def.h"
#include "Configuration.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "ActorManager.h"

class MapWindow
{
 Configuration *config;
 Screen *screen;
 
 Map *map;
 
 unsigned char *tmp_buf; // tempory buffer for flood fill, hide rooms. 
 
 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 
 uint16 cur_x, cur_y;
 
 uint16 win_width, win_height;
 uint8 cur_level;

 public:
 
 MapWindow(Configuration *cfg);
 ~MapWindow();
 
 bool init(Screen *s, Map *m, TileManager *tm, ObjManager *om, ActorManager *am);
 
 bool set_windowSize(uint16 width, uint16 height);
 
 void moveLevel(uint8 new_level);
 void move(uint16 new_x, uint16 new_y);
 void moveRelative(sint16 rel_x, sint16 rel_y);
 void centerOnActor(Actor *actor);
 
 void get_level(uint8 *level);
 void get_pos(uint16 *x, uint16 *y);

 void drawMap();
  
protected:

 void drawObjs();
 void drawObjSuperBlock(U6LList *superblock, bool toptile);
 inline void drawObj(Obj *obj, bool toptile);
 inline void drawTile(uint16 tile_num, uint16 x, uint16 y, bool toptile);
 inline void MapWindow::drawTopTile(Tile *tile, uint16 x, uint16 y, bool toptile);

 void generateTmpMap();
 void boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y);
};

#endif /* __MapWindow_h__ */
