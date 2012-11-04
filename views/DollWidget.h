#ifndef __DollWidget_h__
#define __DollWidget_h__

/*
 *  DollWidget.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Sep 01 2003.
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

#include "GUI_widget.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "InventoryMessage.h"

class Configuration;
class Actor;

class DollWidget : public GUI_Widget {

 Configuration *config;

 bool enable_doubleclick;

 TileManager *tile_manager;
 ObjManager *obj_manager;

 Actor *actor;

 Obj *selected_obj, *unready_obj;

 uint8 bg_color;
 
 bool need_to_free_tiles;
 Tile *empty_tile, *blocked_tile;

 public:
 DollWidget(Configuration *cfg, GUI_CallBack *callback = NULL);
 ~DollWidget();

 bool init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, bool override_style = false);
 void set_actor(Actor *a);
 void Display(bool full_redraw);

 virtual GUI_status MouseDown(int x, int y, int button);
 virtual GUI_status MouseUp(int x,int y,int button);
 virtual GUI_status MouseMotion(int x,int y,Uint8 state);
 GUI_status MouseDouble(int x, int y, int button);
 GUI_status MouseClick(int x, int y, int button);
 GUI_status MouseDelayed(int x, int y, int button);

 void drag_drop_success(int x, int y, int message, void *data);
 void drag_drop_failed(int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);

 void drag_draw(int x, int y, int message, void* data);

 SDL_Rect *get_item_hit_rect(uint8 location);

 protected:

 GUI_CallBack *callback_object; // object-selected callback

 void display_doll();
 void display_readied_object(uint8 location, uint16 x, uint16 y, Actor *actor, Tile *empty_tile);
};

#endif /* __DollWidget_h__ */

