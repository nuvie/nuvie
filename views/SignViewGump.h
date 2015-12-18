#ifndef __SignViewGump_h__
#define __SignViewGump_h__

/*
 *  SignViewGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Feb 07 2013.
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
class Font;
//class BMPFont;

class SignViewGump : public DraggableView {

	Font *font;
	char *sign_text;

public:
 SignViewGump(Configuration *cfg);
 ~SignViewGump();

 bool init(Screen *tmp_screen, void *view_manager, Font *f, Party *p, TileManager *tm, ObjManager *om, const char *text_string, uint16 length);

 void Display(bool full_redraw);

 GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);

 GUI_status MouseDown(int x, int y, int button);
 GUI_status KeyDown(SDL_Keysym key);

 protected:

};

#endif /* __SignViewGump_h__ */

