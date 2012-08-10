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
#include "SDL.h"

#include "ObjManager.h"
#include "GUI_widget.h"
#include "Map.h"

class Configuration;
class TileManager;
class ActorManager;
class Actor;
class AnimManager;
class Map;
class MapCoord;
class Screen;
class CallBack;
class Game;

#define MAPWINDOW_THUMBNAIL_SIZE 52
#define MAPWINDOW_THUMBNAIL_SCALE 3

#define MAP_OVERLAY_DEFAULT 0 /* just below border */
#define MAP_OVERLAY_ONTOP   1 /* cover border */

#define MAPWINDOW_ROOFTILES_IMG_W 5
#define MAPWINDOW_ROOFTILES_IMG_H 204

typedef struct {
	Tile *t;
	uint16 x,y;
} TileInfo;

typedef struct {
	Tile *eye_tile;
	uint16 prev_x, prev_y;
	uint16 moves_left;
	CallBack *caller;
} WizardEye;

enum RoofDisplayType {ROOF_DISPLAY_OFF, ROOF_DISPLAY_NORMAL, ROOF_DISPLAY_FORCE_ON };
enum InterfaceType { INTERFACE_NORMAL, INTERFACE_FULLSCREEN, INTERFACE_IGNORE_BLOCK };

class MapWindow: public GUI_Widget
{
 friend class AnimManager;
 Game *game;
 Configuration *config;
 int game_type;
 bool enable_doubleclick;
 bool walk_with_left_button;
 bool x_ray_view;
 InterfaceType interface;

 Map *map;

 uint16 *tmp_map_buf; // tempory buffer for flood fill, hide rooms.
 uint16 tmp_map_width, tmp_map_height;
 SDL_Surface *overlay; // used for visual effects
 uint8 overlay_level; // where the overlay surface is placed

 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 AnimManager *anim_manager;

 sint16 cur_x, cur_y;
 uint16 cursor_x, cursor_y;
 sint16 mousecenter_x, mousecenter_y; // location mousecursor rotates around, relative to cur_x&cur_y
 uint16 last_boundary_fill_x, last_boundary_fill_y; // start of boundary-fill in previous blacking update
 Tile *cursor_tile;
 Tile *use_tile;

 bool show_cursor;
 bool show_use_cursor;
 bool show_grid;

 unsigned char *thumbnail;
 bool new_thumbnail;

 uint16 win_width, win_height;
 uint8 cur_level;

 uint8 cur_x_add, cur_y_add; // pixel offset from cur_x,cur_y (set by shiftMapRelative)
 sint32 vel_x, vel_y; // velocity of automatic map movement (pixels per second)

 SDL_Rect clip_rect;

 Obj* selected_obj;
 Actor *look_actor;
 Obj* look_obj;
 bool hackmove;
 bool walking;
 bool look_on_left_click;
 bool looking; // used to stop look_on_left_click from triggering during mouseup from left button walking, failed drag, or input mode

 bool window_updated;
 bool freeze_blacking_location;
 bool enable_blacking;

 bool roof_mode;
 RoofDisplayType roof_display;

 SDL_Surface *roof_tiles;

 WizardEye wizard_eye_info;

 bool draw_brit_lens_anim;
 bool draw_garg_lens_anim;
 bool drop_with_move;

 public:

 MapWindow(Configuration *cfg);
 ~MapWindow();

 bool init(Map *m, TileManager *tm, ObjManager *om, ActorManager *am);

 sint16 get_cur_x() { return cur_x; }
 sint16 get_cur_y() { return cur_y; }
 bool set_windowSize(uint16 width, uint16 height);
 void set_show_cursor(bool state);
 void set_show_use_cursor(bool state);
 void set_show_grid(bool state);
 bool is_grid_showing() { return show_grid; }
 void set_velocity(sint16 vx, sint16 vy) { vel_x = vx; vel_y = vy; }
 void set_overlay(SDL_Surface *surfpt);
 void set_overlay_level(int level = MAP_OVERLAY_DEFAULT) { overlay_level = level; }
 void set_x_ray_view(bool state);
 void set_freeze_blacking_location(bool state);
 void set_enable_blacking(bool state);
 void set_roof_display_mode(enum RoofDisplayType mode) { roof_display = mode; }
 void set_walking(bool state) { walking = state; }

 void moveLevel(uint8 new_level);
 void moveMap(sint16 new_x, sint16 new_y, sint8 new_level, uint8 new_x_add = 0, uint8 new_y_add = 0);
 void moveMapRelative(sint16 rel_x, sint16 rel_y);
 void shiftMapRelative(sint16 rel_x, sint16 rel_y);
 void set_mousecenter(sint16 new_x, sint16 new_y) { mousecenter_x = new_x; mousecenter_y = new_y; }
 void reset_mousecenter() { mousecenter_x = win_width / 2; mousecenter_y = win_height / 2; }

 void centerMapOnActor(Actor *actor);
 void centerMap(uint16 x, uint16 y, uint8 z);
 void centerCursor();

 void moveCursor(sint16 new_x, sint16 new_y);
 void moveCursorRelative(sint16 rel_x, sint16 rel_y);

 bool tile_is_black(uint16 x, uint16 y, Obj *obj = NULL); // subtracts cur_x and cur_y
 const char *look(uint16 x, uint16 y, bool show_prefix = true);
 const char *lookAtCursor(bool show_prefix = true) { return(look(cursor_x, cursor_y, show_prefix)); }
 Obj *get_objAtCursor();
 Actor *get_actorAtCursor();
 MapCoord get_cursorCoord();
 Obj *get_objAtMousePos(int x, int y);
 Actor *get_actorAtMousePos(int x, int y);
 void mouseToWorldCoords (int mx, int my, int &wx, int &wy);
 void get_movement_direction(uint16 wx, uint16 wy, sint16 &rel_x, sint16 &rel_y, uint8 *mptr = NULL);

 TileManager *get_tile_manager() { return tile_manager; }
 AnimManager *get_anim_manager() { return anim_manager; }
 SDL_Rect *get_clip_rect()       { return &clip_rect; }
 SDL_Surface *get_overlay();

 void get_level(uint8 *level);
 void get_pos(uint16 *x, uint16 *y, uint8 *px = NULL, uint8 *py = NULL);
 void get_velocity(sint16 *vx, sint16 *vy) { *vx = vel_x; *vy = vel_y; }
 void get_windowSize(uint16 *width, uint16 *height);

 bool in_window(uint16 x, uint16 y, uint8 z);
 bool in_dungeon_level() { return(cur_level != 0 && cur_level != 5); }
 bool in_town();
 // can put object at world location x,y?
 bool can_drop_obj(uint16 x, uint16 y, Actor *actor, bool in_inventory, Obj *obj = NULL, bool accepting_drop = false);
 bool can_get_obj(Actor *actor, Obj *obj);
 void display_move_text(Actor *target_actor, Obj *obj);
 MapCoord original_obj_loc;

 void updateBlacking();
 void updateAmbience();
 void update();
 void Display(bool full_redraw);

 virtual GUI_status	MouseDown (int x, int y, int button);
 virtual GUI_status	MouseUp (int x, int y, int button);
 virtual GUI_status	MouseMotion (int x, int y, Uint8 state);
 GUI_status MouseDouble(int x, int y, int button);
 GUI_status MouseClick(int x, int y, int button);
 GUI_status Idle(void);
 GUI_status MouseLeave(Uint8 state);
 GUI_status MouseDelayed(int x, int y, int button);
 GUI_status MouseHeld(int x, int y, int button);
 GUI_status KeyDown(SDL_keysym key);

 void drag_drop_success (int x, int y, int message, void *data);
 void drag_drop_failed (int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);
 bool move_on_drop(Obj *obj);
 void set_interface();
 InterfaceType get_interface() { return interface; }

 void drag_draw(int x, int y, int message, void* data);

 void update_mouse_cursor(uint32 mx, uint32 my);

 unsigned char *make_thumbnail();
 void free_thumbnail();
 SDL_Surface *get_sdl_surface();
 SDL_Surface *get_sdl_surface(uint16 x, uint16 y, uint16 w, uint16 h);
 SDL_Surface *get_roof_tiles() { return roof_tiles; }

 std::vector<Obj *> m_ViewableObjects; //^^ dodgy public buffer
 std::vector<TileInfo> m_ViewableTiles; //^^ dodgy public buffer

 void wizard_eye_start(MapCoord location, uint16 duration, CallBack *caller);

protected:
 void create_thumbnail();

 void drawActors();
 void drawAnims();
 void drawObjs();
 void drawObjSuperBlock(bool draw_lowertiles, bool toptile);
 inline void drawObj(Obj *obj, bool draw_lowertiles, bool toptile);
 inline void drawTile(Tile *tile, uint16 x, uint16 y, bool toptile, bool use_tile_data=false);
 inline void drawNewTile(Tile *tile, uint16 x, uint16 y, bool toptile);
 void drawBorder();
 inline void drawTopTile(Tile *tile, uint16 x, uint16 y, bool toptile);
 inline void drawActor(Actor *actor);
 void drawRoofs();
 void drawGrid();
 void drawRain();
 inline void drawLensAnim();

 void generateTmpMap();
 void boundaryFill(unsigned char *map_ptr, uint16 pitch, uint16 x, uint16 y);
 bool floorTilesVisible();
 bool boundaryLookThroughWindow(uint16 tile_num, uint16 x, uint16 y);

 void reshapeBoundary();
 inline bool tmpBufTileIsBlack(uint16 x, uint16 y);
 bool tmpBufTileIsBoundary(uint16 x, uint16 y);
 bool tmpBufTileIsWall(uint16 x, uint16 y, uint8 direction);

 void wizard_eye_stop();
 void wizard_eye_update();
 bool is_wizard_eye_mode() { if(wizard_eye_info.moves_left != 0) return true; else return false; }

 void loadRoofTiles();
};

#endif /* __MapWindow_h__ */
