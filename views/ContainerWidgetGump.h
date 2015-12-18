#ifndef __ContainerWidgetGump_h__
#define __ContainerWidgetGump_h__

/*
 *  ContainerWidgetGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Mar 20 2012.
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
#include "ContainerWidget.h"

class Configuration;
class TileManager;
class Actor;
class Font;

class ContainerWidgetGump : public ContainerWidget {

 private:
 sint16 cursor_x;
 uint16 cursor_y;
 sint16 check_x, check_y;
 const Tile *cursor_tile;
 bool show_cursor;

 public:
 ContainerWidgetGump(Configuration *cfg, GUI_CallBack *callback = NULL);
 ~ContainerWidgetGump();

 bool init(Actor *a, uint16 x, uint16 y, uint8 Cols, uint8 Rows, TileManager *tm, ObjManager *om, Font *f, uint8 check_xoff, uint8 check_yoff);

 void Display(bool full_redraw);
 GUI_status KeyDown(SDL_Keysym key);

 virtual void set_actor(Actor *a);
 private:

 void cursor_right();
 void cursor_left();
 void cursor_up();
 void cursor_down();
};

#endif /* __ContainerWidgetGump_h__ */

