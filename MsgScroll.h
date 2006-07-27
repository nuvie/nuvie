#ifndef __MsgScroll_h__
#define __MsgScroll_h__

/*
 *  MsgScroll.h
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

#define MSGSCROLL_U6_WIDTH 17
#define MSGSCROLL_U6_HEIGHT 10

#define MSGSCROLL_MD_WIDTH 16
#define MSGSCROLL_MD_HEIGHT 8

#define MSGSCROLL_SE_WIDTH 16
#define MSGSCROLL_SE_HEIGHT 8

#define MSGSCROLL_CURSOR_DELAY 6 // used to slow down the animated cursor

#define MSGSCROLL_SCROLLBACK_HEIGHT 100

#include <list>
#include <vector>
using std::list;


class Configuration;
class Font;

class MsgText
{
 public:

 Font *font;
 std::string s;

 MsgText();
 MsgText(std::string new_string, Font *f);
 ~MsgText();

 void append(std::string new_string);
 void copy(MsgText *msg_text);
 uint32 length();
};

class MsgLine
{
 public:

 std::list<MsgText *> text;
 uint32 total_length;

 MsgLine() { total_length = 0; };
 ~MsgLine();

 void append(MsgText *new_text);
 void remove_char();
 uint32 length();
 MsgText *get_text_at_pos(uint16 pos);
};

class MsgScroll: public GUI_Widget, public CallBack
{
 Configuration *config;
 int game_type;
 uint16 screen_x; //x offset to top left corner of MsgScroll
 uint16 screen_y; //y offset to top left corner of MsgScroll

 Font *font;
 bool keyword_highlight;
 bool input_mode;
 const char *permit_input; // character list, or 0 = any string
 bool permit_inputescape; // can RETURN or ESCAPE be used to escape input entry
 bool talking;

 MsgText prompt;
 std::list<MsgText *> holding_buffer;

 bool page_break;
 bool show_cursor;
 bool autobreak; // if true, a page break will be added when the scroll is full

 std::list<MsgLine *> msg_buf;

 uint16 scroll_height;
 uint16 scroll_width;

 bool scroll_updated;
 uint8 cursor_char;
 uint16 cursor_x, cursor_y;

 uint16 cursor_wait;

 std::string input_buf;
 uint16 line_count; // count the number of lines since last page break.

 uint16 display_pos;

 // set by request_input()
 CallBack *callback_target;
 char *callback_user_data;

 public:

 MsgScroll(Configuration *cfg, Font *f);
 ~MsgScroll();

 bool init(char *player_name);

 void process_holding_buffer();

 MsgText *holding_buffer_get_token();
 bool add_token(MsgText *token);
 bool remove_char();

 void set_font(Font *f);

 void display_string(std::string s, Font *f);

 void display_string(std::string s, uint16 length, uint8 lang_num);

 void display_string(std::string s, uint8 lang_num=0);
 bool set_prompt(const char *new_prompt, Font *f=NULL);
 void display_prompt();

 void message(const char *string) { display_string(string); display_prompt(); }

 void set_keyword_highlight(bool state);

 void set_input_mode(bool state, const char *allowed = NULL,
                     bool can_escape = true);
 void set_talking(bool state) { talking = state; }

 void set_show_cursor(bool state) { show_cursor = state; }

 void set_autobreak(bool state) { autobreak = state; }

 bool get_page_break() { return(page_break); }

 GUI_status KeyDown(SDL_keysym key);
 GUI_status MouseUp(int x, int y, int button);
 MsgText *get_token_at_pos(uint16 x, uint16 y);
 //void updateScroll();
 void Display(bool full_redraw);

 void clearCursor(uint16 x, uint16 y);
 void drawCursor(uint16 x, uint16 y);

 void set_page_break();

 bool input_buf_add_char(char c);
 bool input_buf_remove_char();

 bool has_input();
 std::string get_input();
 const char *peek_at_input();
 void request_input(CallBack *caller, void *user_data);
 void cancel_input_request() { request_input(NULL, NULL); }

 protected:

 inline MsgLine *add_new_line();
 void drawLine(Screen *screen, MsgLine *msg_line, uint16 line_y);
 inline void clear_page_break();
};


#endif /* __MsgScroll_h__ */

