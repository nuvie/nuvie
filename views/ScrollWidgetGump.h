#ifndef __ScrollWidgetGump_h__
#define __ScrollWidgetGump_h__

/*
 *  ScrollWidgetGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Nov 06 2013.
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
#include "CallBack.h"
#include "GUI_widget.h"
#include "MsgScroll.h"
#include "Font.h"
#include <stdarg.h>

#include <list>
#include <vector>
#include <string>
using std::list;


class Configuration;
class Actor;
class CallBack;

typedef enum {
	SCROLL_UP,
	SCROLL_DOWN,
	SCROLL_PAGE_UP,
	SCROLL_PAGE_DOWN,
	SCROLL_ESCAPE,
	SCROLL_TO_BEGINNING,
	SCROLL_TO_END
} ScrollEventType;

#define SCROLLWIDGETGUMP_W 200
#define SCROLLWIDGETGUMP_H 100

class ScrollWidgetGump: public MsgScroll
{

 Font *font_normal;
 Font *font_garg;

 uint8 font_color;
 uint8 font_highlight;
 uint16 position;

 std::string trailing_whitespace;

 bool show_up_arrow;
 bool show_down_arrow;

 public:

 ScrollWidgetGump(Configuration *cfg, Screen *s);
 ~ScrollWidgetGump();

 bool parse_token(MsgText *token);

 bool can_display_prompt() { return false; }

 void Display(bool full_redraw);

 void display_prompt() {}
 void display_string(std::string s);

 void set_font(uint8 font_type);
 bool is_garg_font();

 bool can_fit_token_on_msgline(MsgLine *msg_line, MsgText *token);

 GUI_status KeyDown(SDL_Keysym key);
 GUI_status MouseDown(int x, int y, int button);
 GUI_status MouseUp(int x, int y, int button) { return GUI_YUM; } // otherwise we do Msgscroll::MouseUp
 GUI_status MouseWheel(sint32 x, sint32 y);

 void move_scroll_down() { scroll_movement_event(SCROLL_DOWN); }
 void move_scroll_up() { scroll_movement_event(SCROLL_UP); }

 protected:




 private:
 GUI_status scroll_movement_event(ScrollEventType event);
 void update_arrows();


};


#endif /* __ScrollWidgetGump_h__ */

