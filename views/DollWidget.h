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

#include <string>

#include "U6def.h"
#include "Configuration.h"
#include "GUI_widget.h"
#include "ObjManager.h"
#include "Actor.h"
#include "TileManager.h"

class TileManager;

class DollWidget : public GUI_Widget {
 
 Configuration *config;
 
 TileManager *tile_manager;
 ObjManager *obj_manager;
 
 Actor *actor;
 
 Obj *selected_obj;
 
 public:
 DollWidget(Configuration *cfg);
 ~DollWidget();
 
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

 void drag_draw(int x, int y, int message, void* data);

 protected:

 void DollWidget::display_doll();
 void display_readied_object(uint8 location, uint16 x, uint16 y, Actor *actor, Tile *empty_tile);
};

#endif /* __DollWidget_h__ */

