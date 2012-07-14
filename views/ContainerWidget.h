#ifndef __ContainerWidget_h__
#define __ContainerWidget_h__

/*
 *  ContainerWidget.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Mar 25 2012.
 *  Copyright (c) 2012 The Nuvie Team. All rights reserved.
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
#include "ObjManager.h"
#include "InventoryMessage.h"

class Configuration;
class TileManager;
class Actor;
class Text;

class ContainerWidget : public GUI_Widget {

protected:
 Configuration *config;

 int game_type;
 bool enable_doubleclick;

 TileManager *tile_manager;
 ObjManager *obj_manager;

 Actor *actor;
 Obj *container_obj;

 Obj *selected_obj, *target_obj, *ready_obj;
 Obj *target_cont;
 uint16 row_offset;

 uint8 bg_color;
 uint8 obj_font_color;
 
 bool fill_bg;

 const Tile *empty_tile;

 public:
 ContainerWidget(Configuration *cfg, GUI_CallBack *callback = NULL);
 ~ContainerWidget();

 bool init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, Text *t);
 void set_actor(Actor *a);
 Actor *get_actor() { return(actor); }
 Obj *get_container() { return(container_obj); }
 void set_container(Obj *obj) { container_obj = obj; row_offset = 0; Redraw(); }
 bool is_showing_container() { return (container_obj != NULL ? true : false); }
 void Display(bool full_redraw);

 virtual GUI_status MouseDown(int x, int y, int button);
 virtual GUI_status MouseUp(int x,int y,int button);
 virtual GUI_status MouseMotion(int x,int y,Uint8 state);
 GUI_status MouseDouble(int x, int y, int button);
 GUI_status MouseClick(int x,int y,int button);
 GUI_status MouseDelayed(int x, int y, int button);

 void drag_drop_success(int x, int y, int message, void *data);
 void drag_drop_failed(int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);

 void drag_draw(int x, int y, int message, void* data);

 protected:

 GUI_CallBack *callback_object; // object-selected callback

 inline uint16 get_list_position(int x, int y);
 void display_inventory_list();
 inline void display_qty_string(uint16 x, uint16 y, uint16 qty);
 inline void display_special_char(uint16 x, uint16 y, uint8 quality);
 void display_arrows();

 bool drag_set_target_obj(int x, int y);
 void try_click();

 public:
 bool up_arrow();
 bool down_arrow();
 Obj *get_obj_at_location(int x, int y);
};

#endif /* __ContainerWidget_h__ */

