#ifndef __MapWindow_h__
#define __MapWindow_h__

/*
 *  MapWindow.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Mar 21 2003.
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

#include <SDL.h>

#include "ObjManager.h"
#include "GUI_widget.h"

class Configuration;
class TileManager;
class ActorManager;
class Actor;
class Map;
class MapCoord;
class Screen;

class MapWindow: public GUI_Widget
{
 Configuration *config;
 int game_type;
 
 Map *map;
 
 uint16 *tmp_buf; // tempory buffer for flood fill, hide rooms. 
 
 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 
 sint16 cur_x, cur_y;
 uint16 cursor_x, cursor_y;
 Tile *cursor_tile;
 Tile *use_tile;
 
 bool show_cursor;
 bool show_use_cursor;
 
 uint16 win_width, win_height;
 uint8 cur_level;

 SDL_Rect clip_rect;

 Obj* selected_obj;
 Actor *selected_actor;
 bool hackmove;
 
 public:
 
 MapWindow(Configuration *cfg);
 ~MapWindow();
 
 bool init(Map *m, TileManager *tm, ObjManager *om, ActorManager *am);
 
 bool set_windowSize(uint16 width, uint16 height);
 void set_show_cursor(bool state);
 void set_show_use_cursor(bool state);
 
 void moveLevel(uint8 new_level);
 void moveMap(sint16 new_x, sint16 new_y, sint8 new_level);
 void moveMapRelative(sint16 rel_x, sint16 rel_y);
 
 void centerMapOnActor(Actor *actor);
 void centerCursor();
 
 void moveCursor(sint16 new_x, sint16 new_y);
 void moveCursorRelative(sint16 rel_x, sint16 rel_y);
 
 const char *lookAtCursor(bool show_prefix = true);
 Obj *get_objAtCursor();
 Actor *get_actorAtCursor();
 MapCoord get_cursorCoord();
 Obj *get_objAtMousePos(int x, int y);
 void mouseToWorldCoords (int mx, int my, int &wx, int &wy);

 void get_level(uint8 *level);
 void get_pos(uint16 *x, uint16 *y);

 bool in_window(uint16 x, uint16 y, uint8 z);
 bool in_dungeon_level() { return(cur_level != 0 && cur_level != 5); }

 void updateBlacking();
 
 void Display(bool full_redraw);
 
 virtual GUI_status	MouseDown (int x, int y, int button);
 virtual GUI_status	MouseUp (int x, int y, int button);
 virtual GUI_status	MouseMotion (int x, int y, Uint8 state);

 void drag_drop_success (int x, int y, int message, void *data);
 void drag_drop_failed (int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);

 void drag_draw(int x, int y, int message, void* data);

protected:

 void drawActors();
 void drawObjs();
 void drawObjSuperBlock(bool draw_lowertiles, bool toptile);
 inline void drawObj(Obj *obj, bool draw_lowertiles, bool toptile);
 inline void drawTile(Tile *tile, uint16 x, uint16 y, bool toptile);
 void drawBorder();
 inline void drawTopTile(Tile *tile, uint16 x, uint16 y, bool toptile);

 void generateTmpMap();
 void boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y);
 bool boundaryLookThroughWindow(uint16 tile_num, uint16 x, uint16 y);
 
 void reshapeBoundary();
 inline bool tmpBufTileIsBlack(uint16 x, uint16 y);
 bool tmpBufTileIsBoundary(uint16 x, uint16 y);
 bool tmpBufTileIsWall(uint16 x, uint16 y);
};

#endif /* __MapWindow_h__ */
