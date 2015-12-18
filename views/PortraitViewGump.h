#ifndef __PortraitViewGump_h__
#define __PortraitViewGump_h__

/*
 *  PortraitViewGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Apr 09 2012.
 *  Copyright (c) 2012. All rights reserved.
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

#include "DraggableView.h"

class Configuration;
class TileManager;
class ObjManager;
class Screen;
class Actor;
class Font;
class U6Bmp;
class Portrait;

class PortraitViewGump : public DraggableView {

	GUI_Button *gump_button;

	GUI_Font *font;

	Portrait *portrait;
	unsigned char *portrait_data;
	Actor *actor;
	bool show_cursor;
	const Tile *cursor_tile;
	gumpCursorPos cursor_pos;
	uint8 cursor_xoff, cursor_yoff;

public:
 PortraitViewGump(Configuration *cfg);
 ~PortraitViewGump();

 bool init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Font *f, Party *p, TileManager *tm, ObjManager *om, Portrait *por, Actor *a);

 void Display(bool full_redraw);

 GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);

 GUI_status MouseDown(int x, int y, int button);
 GUI_status MouseWheel(sint32 x, sint32 y);

 protected:

 void set_actor(Actor *a);
 void left_arrow();
 void right_arrow();
 GUI_status KeyDown(SDL_Keysym key);
 GUI_status set_cursor_pos(gumpCursorPos pos);
};

#endif /* __PortraitViewGump_h__ */

