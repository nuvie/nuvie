#ifndef __ConverseGumpWOU_h__
#define __ConverseGumpWOU_h__

/*
 *  ConverseGumpWOU.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Nov 20 2014.
 *  Copyright (c) 2014. All rights reserved.
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
#include "Font.h"

#include <list>
#include <vector>
#include <string>
using std::list;


class Configuration;
class Font;
class MsgScroll;
class Actor;

class ConverseGumpWOU: public MsgScroll
{

	uint8 converse_bg_color;

	uint8 frame_w;
	uint8 frame_h;
	uint16 min_w;

	nuvie_game_t game_type;
	bool found_break_char;
 public:

 ConverseGumpWOU(Configuration *cfg, Font *f, Screen *s);
 ~ConverseGumpWOU();

 virtual void set_talking(bool state, Actor *actor = NULL);
 virtual void set_font(uint8 font_type) {}
 virtual void display_converse_prompt();

 void Display(bool full_redraw);

 GUI_status MouseUp(int x, int y, int button) { return GUI_YUM; }

 GUI_status MouseDown(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseMotion(int x, int y, Uint8 state) { return GUI_YUM; }
 GUI_status MouseEnter(Uint8 state) { return GUI_YUM; }
 GUI_status MouseLeave(Uint8 state) { return GUI_YUM; }
 GUI_status MouseClick(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseDouble(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseDelayed(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseHeld(int x, int y, int button) { return GUI_YUM; }

 virtual bool is_converse_finished() { return (is_holding_buffer_empty() && !page_break); }

 protected:

 void input_add_string(std::string token_str);
 virtual void process_page_break();
 virtual uint8 get_input_font_color() { return FONT_COLOR_WOU_CONVERSE_INPUT; }
 void display_bg();

 private:

 SDL_Surface *bg_image;
};


#endif /* __ConverseGumpWOU_h__ */

