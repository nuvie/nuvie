/*
 *  ConverseGumpWOU.cpp
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
#include "ConverseGumpWOU.h"
#include "ActorManager.h"
#include "Keys.h"
#include "MapWindow.h"

#define INPUT_FONT_COLOR 1

// ConverseGumpWOU Class

ConverseGumpWOU::ConverseGumpWOU(Configuration *cfg, Font *f, Screen *s)
{
// uint16 x, y;

 init(cfg, f);
 Game *game = Game::get_game();
 game_type = game->get_game_type();

 //scroll_width = 20;
 //scroll_height = 18;

 set_scroll_dimensions(18, 18);

 std::string height_str;
 min_w = game->get_min_converse_gump_width();
 uint16 x_off = game->get_game_x_offset();
 uint16 y_off = game->get_game_y_offset();

 if(game_type == NUVIE_GAME_U6)
 {
   GUI_Widget::Init(NULL, x_off + 8, y_off + 8, 160, 160);
   bg_color =converse_bg_color = 17;
 }
 else //MD and SE
 {
   GUI_Widget::Init(NULL, x_off + 8, y_off + 16, 160, 144);
   bg_color =converse_bg_color = Game::get_game()->get_palette()->get_bg_color();
 }

	 found_break_char = false;
	 left_margin = 8;
	 add_new_line();
//DEBUG(0, LEVEL_DEBUGGING, "\nMin w = %d\n", frame_w + 12 + 210);
}

ConverseGumpWOU::~ConverseGumpWOU()
{

}

void ConverseGumpWOU::set_talking(bool state, Actor *actor)
{
  if(state)
  {
    found_break_char = true;
    clear_scroll();
    Show();
  }
  else
  {
    if(talking)
    {
      MsgScroll::display_string("\nPress any key...*", MSGSCROLL_NO_MAP_DISPLAY);
    }
  }
	MsgScroll::set_talking(state);
}

void ConverseGumpWOU::process_page_break()
{
  page_break = false;
  just_finished_page_break = true;
  if(!input_mode)
    Game::get_game()->get_gui()->unlock_input();
  if(!input_mode)
  {
    //clear_scroll();

    process_holding_buffer(); // Process any text in the holding buffer.
  }
}

void ConverseGumpWOU::display_converse_prompt()
{
  MsgScroll::display_string("\nyou say:", INPUT_FONT_COLOR, MSGSCROLL_NO_MAP_DISPLAY);
}


void ConverseGumpWOU::Display(bool full_redraw)
{
  MsgScroll::Display(true);
  if(game_type == NUVIE_GAME_U6)
  {
    Game::get_game()->get_map_window()->drawBorder();
    screen->update(area.x,area.y, area.w, area.h);
  }
}
