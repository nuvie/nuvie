#ifndef __ConverseGump_h__
#define __ConverseGump_h__

/*
 *  ConverseGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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
#include "CallBack.h"
#include "GUI_widget.h"
#include <stdarg.h>

#include <list>
#include <vector>
#include <string>
using std::list;


class Configuration;
class Font;
class MsgScroll;
class Actor;

class ConverseGump: public MsgScroll
{
	std::list<MsgText> conv_keywords;
	std::list<MsgText> permitted_input_keywords;

	std::list<MsgText> *keyword_list;

	unsigned char *npc_portrait;
	unsigned char *avatar_portrait;

	bool found_break_char;

	uint8 converse_bg_color;

 public:

 ConverseGump(Configuration *cfg, Font *f);
 ~ConverseGump();

 void set_actor_portrait(Actor *a);
 unsigned char *create_framed_portrait(Actor *a);
 virtual bool parse_token(MsgText *token);
 virtual const std::string get_token_string_at_pos(uint16 x, uint16 y);
 virtual void display_string(std::string s, Font *f);
 virtual void set_talking(bool state);
 virtual void set_font(uint8 font_type) {}

 void Display(bool full_redraw);

 GUI_status KeyDown(SDL_keysym key);
 GUI_status MouseUp(int x, int y, int button);

 GUI_status MouseDown(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseMotion(int x, int y, Uint8 state) { return GUI_YUM; }
 GUI_status MouseEnter(Uint8 state) { return GUI_YUM; }
 GUI_status MouseLeave(Uint8 state) { return GUI_YUM; }
 GUI_status MouseClick(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseDouble(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseDelayed(int x, int y, int button) { return GUI_YUM; }
 GUI_status MouseHeld(int x, int y, int button) { return GUI_YUM; }

 void set_found_break_char(bool val) { found_break_char = val; }

 virtual bool input_buf_add_char(char c);
 virtual bool input_buf_remove_char();

 protected:
 const std::string strip_whitespace_after_break(std::string s);
 void add_keyword(std::string keyword);

 virtual void set_permitted_input(const char *allowed);
 virtual void clear_permitted_input();
};


#endif /* __ConverseGump_h__ */

