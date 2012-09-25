/*
 *  MsgScrollNewUI.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Sep 23 2012.
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
#include <string>
#include <cctype>
#include <iostream>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "FontManager.h"
#include "Font.h"
#include "GamePalette.h"
#include "GUI.h"
#include "MsgScroll.h"
#include "Portrait.h"
#include "Player.h"
#include "ConvFont.h"
#include "MsgScrollNewUI.h"
#include "ActorManager.h"
#include "TimedEvent.h"

// MsgScrollNewUI Class

MsgScrollNewUI::MsgScrollNewUI(Configuration *cfg, Screen *s)
{
 uint16 x, y;

 font_normal = new ConvFont();
 font_normal->init(NULL, 256, 0);

 font_garg = new ConvFont();
 font_garg->init(NULL, 256, 128);

 init(cfg, font_normal);

 scroll_width = 30;
 scroll_height = 19;
 scrollback_height = 19;

 x = 8;
 y = 8;

 uint16 x_off = config_get_video_x_offset(config);
 uint16 y_off = config_get_video_y_offset(config);

 GUI_Widget::Init(NULL, x_off, y_off, s->get_width(), s->get_height());

 cursor_wait = 0;

 timer = NULL;
}

MsgScrollNewUI::~MsgScrollNewUI()
{
	delete font;
}

void MsgScrollNewUI::display_string(std::string s, Font *f, bool include_on_map_window)
{
	timer = new TimedCallback(this, NULL, 2000);

	MsgScroll::display_string(s, f, include_on_map_window);
}

void MsgScrollNewUI::set_font(uint8 font_type)
{
	if(font_type == NUVIE_FONT_NORMAL)
	{
		font = font_normal;
	}
	else
	{
		font = font_garg;
	}
}

bool MsgScrollNewUI::is_garg_font()
{
	return (font==font_garg);
}

uint16 MsgScrollNewUI::callback(uint16 msg, CallBack *caller, void *data)
{
	if(msg == CB_TIMED && (timer == NULL || timer == caller))
	{
		timer = NULL;
		if(input_mode)
		{
			new TimedCallback(this, NULL, 100);
		}
		else
		{
			delete_front_line();

			if(msg_buf.size() > 0)
			{
				new TimedCallback(this, NULL, 50);
			}
			else
			{
				add_new_line(); //buffer cannot be empty.
			}
		}
	}

	return 1;
}

void MsgScrollNewUI::Display(bool full_redraw)
{
	MsgText *token;
	 //std::list<MsgText>::iterator iter;
	 uint16 total_length = 0;
	 uint16 y = area.y + 8 + 3;

	 y = area.y + 4;
	 total_length = 0;
	 std::list<MsgLine *>::iterator iter;
	 for(iter=msg_buf.begin();iter != msg_buf.end();iter++)
	     {
		  MsgLine *msg_line = *iter;
		  std::list<MsgText *>::iterator iter1;

		  for(iter1=msg_line->text.begin();iter1 != msg_line->text.end() ; iter1++)
		  {
			  token = *iter1;

			  total_length += token->font->drawString(screen, token->s.c_str(), area.x + 4 + 4 + total_length, y + 4, 0); //FIX for hardcoded font height
			  //token->s.length();
			  //token->font->drawChar(screen, ' ', area.x + PORTRAIT_WIDTH + 8 + total_length * 8, y, 0);
			  //total_length += 1;

		  }
		  y+=10;
		  total_length = 0;
	     }

	//font->drawString(screen, conv_str.c_str(), area.x, area.y);
	screen->update(area.x,area.y, area.w, area.h);
}

