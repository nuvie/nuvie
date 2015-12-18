#ifndef __GUI_TextInput_h__
#define __GUI_TextInput_h__

/*
 *  GUI_TextInput.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jun 26 2004.
 *  Copyright (c) Nuvie Team 2004. All rights reserved.
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

#include "GUI_text.h"

class GUI_Font;


#define TEXTINPUT_CB_TEXT_READY 0x1

class GUI_TextInput : public GUI_Text
{
 protected:
 uint16 max_height;
 uint16 pos;
 uint16 length;

 GUI_CallBack *callback_object;

 Uint32 cursor_color;
 Uint32 selected_bgcolor;

 public:

 GUI_TextInput(int x, int y, Uint8 r, Uint8 g, Uint8 b,
               char *str, GUI_Font *gui_font, uint16 width, uint16 height, GUI_CallBack *callback);
 ~GUI_TextInput();

 void release_focus();

 GUI_status MouseUp(int x, int y, int button);
 GUI_status KeyDown(SDL_Keysym key);

 void add_char(char c);
 void remove_char();
 void set_text(const char *new_text);
 char *get_text() { return text; }
void SetDisplay(Screen *s);
void display_cursor();

	/* Show the widget  */
	virtual void Display(bool full_redraw);

};

#endif /* __GUI_TextInput_h__ */

