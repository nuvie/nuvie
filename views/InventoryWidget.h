#ifndef __InventoryWidget_h__
#define __InventoryWidget_h__

/*
 *  InventoryWidget.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Sep 01 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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

#include "U6def.h"
#include "Configuration.h"
#include "GUI_Widget.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Actor.h"

class InventoryWidget : public GUI_Widget {
 
 Configuration *config;
 
 TileManager *tile_manager;
 ObjManager *obj_manager;
 
 Actor *actor;
 Obj *container_obj;
 
 Obj* selected_obj, *target_obj;
 uint16 row_offset;
 
 public:
 InventoryWidget(Configuration *cfg);
 ~InventoryWidget();
 
 bool init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om);
 void set_actor(Actor *a);
 void Display(bool full_redraw);
 
 virtual GUI_status MouseDown(int x, int y, int button);
 virtual GUI_status MouseUp(int x,int y,int button);
 virtual GUI_status MouseMotion(int x,int y,Uint8 state);

 void drag_drop_success(int x, int y, int message, void *data);
 void drag_drop_failed(int x, int y, int message, void *data);

 bool drag_accept_drop(int x, int y, int message, void *data);
 void drag_perform_drop(int x, int y, int message, void *data);
 
 protected:
 inline uint16 get_list_position(int x, int y);
 inline Obj *get_obj_at_location(int x, int y);
 void display_inventory_container();
 void display_inventory_list();
};

#endif /* __InventoryWidget_h__ */

