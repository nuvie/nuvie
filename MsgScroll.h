#ifndef __MsgScroll_h__
#define __MsgScroll_h__

/*
 *  MsgScroll.h
 *  Nuive
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

#include "U6def.h"
#include "Configuration.h"
#include "Screen.h"
#include "Text.h"

#define MSGSCROLL_WIDTH 17
#define MSGSCROLL_HEIGHT 10

class MsgScroll
{
 Configuration *config;
 Screen *screen;
 Text *text;
 bool keyword_highlight;
 bool input_mode;
 
 char *prompt;
 uint16 prompt_buf_len;
 
 
 char msg_buf[MSGSCROLL_HEIGHT][18];
 uint8 buf_pos;
 uint8 start_pos;
 bool buf_full;
 
 bool scroll_updated;
 uint8 cursor_char;
 uint16 cursor_x, cursor_y;
 
 uint16 cursor_wait;
 
 public:
 
 MsgScroll(Configuration *cfg);
 ~MsgScroll();
 
 bool init(Screen *s, Text *txt);
 
 void display_string(char *string);
 void set_keyword_highlight(bool state);
 
 void set_input_mode(bool state);
 bool set_prompt(char *new_prompt);
 
 char *get_input();
 
 void updateScroll();
 
 void clearCursor(uint16 x, uint16 y);
 void drawCursor(uint16 x, uint16 y);
 
 bool buf_addString(char *string, uint8 length);
 bool buf_next();
};


#endif /* __MsgScroll_h__ */

