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

#include <vector>
#include <SDL.h>

#include "ObjManager.h"
#include "GUI_widget.h"

class Configuration;
class TileManager;
class ActorManager;
class Actor;
class AnimManager;
class Map;
class MapCoord;
class Screen;

typedef struct {
	Tile *t;
	uint16 x,y;
} TileInfo;

class MapWindow: public GUI_Widget
{
 friend class AnimManager;
 Configuration *config;
 int game_type;
 
 Map *map;
 
 uint16 *tmp_buf; // tempory buffer for flood fill, hide rooms. 
 
 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 AnimManager *anim_manager;
 
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
 bool walking;
 uint32 walk_start_delay;

 bool window_updated;
 
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
 
 const char *look(uint16 x, uint16 y, bool show_prefix = true);
 const char *lookAtCursor(bool show_prefix = true) { return(look(cur_x + cursor_x, cur_y + cursor_y, show_prefix)); }
 Obj *get_objAtCursor();
 Actor *get_actorAtCursor();
 MapCoord get_cursorCoord();
 Obj *get_objAtMousePos(int x, int y);
 Actor *get_actorAtMousePos(int x, int y);
 void mouseToWorldCoords (int mx, int my, int &wx, int &wy);
 void get_movement_direction(uint16 wx, uint16 wx, sint16 &rel_x, sint16 &rel_y);

 TileManager *get_tile_manager() { return tile_manager; }
 SDL_Rect *get_clip_rect() { return &clip_rect; }

 void get_level(uint8 *level);
 void get_pos(uint16 *x, uint16 *y);

 bool in_window(uint16 x, uint16 y, uint8 z);
 bool in_dungeon_level() { return(cur_level != 0 && cur_level != 5); }

 void updateBlacking();
 
 void Display(bool full_redraw);
 
 virtual GUI_status	MouseDown (int x, int y, int button);
 virtual GUI_status	MouseUp (int x, int y, int button);
 virtual GUI_status	MouseMotion (int x, int y, Uint8 state);
 GUI_status MouseDouble(int x, int y, int button);
 GUI_status MouseClick(int x, int y, int button);
 GUI_status Idle(void);
 GUI_status MouseLeave(Uint8 state);

 void drag_drop_success (int x, int y, int message, void *data);
 void drag_drop_failed (int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);

 void drag_draw(int x, int y, int message, void* data);

 void player_walk_to_mouse_cursor(uint32 mx, uint32 my);
 void player_multiuse(uint16 wx, uint16 wy);

 void update_mouse_cursor(uint32 mx, uint32 my);

 std::vector<Obj *> m_ViewableObjects; //^^ dodgy public buffer
 std::vector<TileInfo> m_ViewableTiles; //^^ dodgy public buffer

protected:

 void drawActors();
 void drawAnims();
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
