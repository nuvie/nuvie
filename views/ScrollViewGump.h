#ifndef __ScrollViewGump_h__
#define __ScrollViewGump_h__

/*
 *  ScrollViewGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Nov 08 2013.
 *  Copyright (c) 2013. All rights reserved.
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
class ScrollWidgetGump;

class ScrollViewGump : public DraggableView {

  ScrollWidgetGump *scroll_widget;

public:
 ScrollViewGump(Configuration *cfg);
 ~ScrollViewGump();

 bool init(Screen *tmp_screen, void *view_manager, Font *f, Party *p, TileManager *tm, ObjManager *om, std::string text_string);

 void Display(bool full_redraw);

 GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);

 GUI_status MouseDown(int x, int y, int button);
 GUI_status MouseUp(int x, int y, int button) { return GUI_YUM; }
 GUI_status KeyDown(SDL_Keysym key);

 protected:

};

#endif /* __ScrollViewGump_h__ */

