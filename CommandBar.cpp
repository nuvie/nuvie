/*
 *  CommandBar.cpp
 *  Nuvie
 *
 *  Created by Joseph Applegate on Wed Mar 2 2005.
 *  Copyright (c) 2005 The Nuvie Team. All rights reserved.
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
#include <cassert>
#include "SDL.h"

#include "Configuration.h"

#include "nuvieDefs.h"
#include "U6misc.h"
#include "Game.h"
#include "Screen.h"
#include "Event.h"
#include "TileManager.h"
#include "Text.h"
#include "GameClock.h"
#include "GamePalette.h"
#include "CommandBar.h"
#include "Weather.h"
#include "Party.h"
#include "Player.h"
#include "Objlist.h"
#include "NuvieIO.h"

#define USE_BUTTON 1
#define ACTION_BUTTON 3

using std::string;

CommandBar::CommandBar(Game *g) : GUI_Widget(NULL)
{
    game = g;
    Weather *weather;
    Configuration *config = g->get_config();

    uint16 x_off = config_get_video_x_offset(config);
    uint16 y_off = config_get_video_y_offset(config);
    if(game->get_game_type() == NUVIE_GAME_U6)
    {
        offset = OBJLIST_OFFSET_U6_COMMAND_BAR;
        Init(NULL, 8+x_off, 168+y_off, 0, 0);
        area.w = 16 * 10; // space for 10 icons
        area.h = 24 + 1; // extra space for the underlined default action
    }
    else if(game->get_game_type() == NUVIE_GAME_MD)
    {
        offset = OBJLIST_OFFSET_MD_COMMAND_BAR;
        Init(NULL, 15+x_off, 163+y_off, 146, 33);
    }
    else // SE
    {
        offset = OBJLIST_OFFSET_SE_COMMAND_BAR;
        Init(NULL, 8+x_off, 178+y_off, 163, 19);
    }

    event = NULL; // it's not set yet
    text = game->get_text();
    
    weather = game->get_weather();
    
    selected_action = -1;
    combat_mode = false;
    wind = "?";

    if(game->get_game_type() == NUVIE_GAME_U6)
        init_buttons();
    
    bg_color = game->get_palette()->get_bg_color();
    
    weather->add_wind_change_notification_callback((CallBack *)this); //we want to know when the wind direction changes.
}

CommandBar::~CommandBar()
{
}

bool CommandBar::init_buttons()
{
    TileManager *tile_man = game->get_tile_manager();
    for(uint32 i = 0; i < 9; i++)
        icon[i] = tile_man->get_tile(i+400);
    // NOTE: combat button has two states
    icon[9] = tile_man->get_tile(combat_mode ? 415 : 414);
    return(true);
}

bool CommandBar::load(NuvieIO *objlist)
{
	objlist->seek(offset);
	uint8 action = objlist->read1();
	selected_action = (action == 0xff ? -1 : action - 0x81);
	return true;
}

bool CommandBar::save(NuvieIO *objlist)
{
	objlist->seek(offset);
	objlist->write1(selected_action > 0 ? selected_action + 0x81: 0xff);

	return true;
}

void CommandBar::fill_square(uint8 pal_index)
{
	screen->fill(pal_index, area.x + selected_action*18, area.y, 19, 1); // top row
	screen->fill(pal_index, area.x + selected_action*18, area.y +18, 19, 1); // bottom row
	screen->fill(pal_index, area.x + selected_action*18, area.y +1, 1, 17); // left side
	screen->fill(pal_index, area.x + selected_action*18 +18, area.y +1, 1, 17); // right side
}

GUI_status CommandBar::MouseDown(int x, int y, int button)
{
    x -= area.x;
    y -= area.y;

    if((game->get_game_type() == NUVIE_GAME_U6 && y >= 8 && y <= 24)
        || game->get_game_type() != NUVIE_GAME_U6)
    {
        uint8 activate = x / 16; // icon selected
        if(game->get_game_type() == NUVIE_GAME_SE)
              activate = x/18;
        else if(game->get_game_type() == NUVIE_GAME_MD)
              activate = (x-1)/18;
        if(button == USE_BUTTON)
            return(hit(activate));
        else if(button == ACTION_BUTTON)
        {
            if(game->get_game_type() == NUVIE_GAME_SE) // black out previous setting
                fill_square(0);
            if(selected_action == activate) // clear if already selected
                set_selected_action(-1);
            else
                set_selected_action(activate);
        }
    }
    return(GUI_YUM);
}

GUI_status CommandBar::hit(uint8 num)
{
    if(!event) event = game->get_event();

    if(event->get_mode() != MOVE_MODE)
        return GUI_PASS;

	if(game->get_game_type() == NUVIE_GAME_U6)
    {
    	switch(num) // hit button
    	{
			case 0: event->newAction(ATTACK_MODE); break;
			case 1: event->newAction(CAST_MODE); break;
			case 2: event->newAction(TALK_MODE); break;
			case 3: event->newAction(LOOK_MODE); break;
			case 4: event->newAction(GET_MODE); break;
			case 5: event->newAction(DROP_MODE); break;
			case 6: event->newAction(PUSH_MODE); break;
			case 7: event->newAction(USE_MODE); break;
			case 8: event->newAction(REST_MODE); break;
			case 9: event->newAction(COMBAT_MODE); break;
    	}
    }
	else if(game->get_game_type() == NUVIE_GAME_MD)
	{
		switch(num) // hit button
		{
			case 0: event->newAction(ATTACK_MODE); break;
			case 1: event->newAction(TALK_MODE); break;
			case 2: event->newAction(LOOK_MODE); break;
			case 3: event->newAction(GET_MODE); break;
			case 4: event->newAction(DROP_MODE); break;
			case 5: event->newAction(PUSH_MODE); break;
			case 6: event->newAction(USE_MODE); break;
			case 7: event->newAction(COMBAT_MODE); break;
		}
	}
	else // SE
	{
		switch(num) // hit button
		{
			case 0: event->newAction(PUSH_MODE); break;
			case 1: event->newAction(GET_MODE); break;
			case 2: event->newAction(DROP_MODE); break;
			case 3: event->newAction(USE_MODE); break;
			case 4: event->newAction(TALK_MODE); break;
			case 5: event->newAction(LOOK_MODE); break;
			case 6: event->newAction(ATTACK_MODE); break;
			case 7: event->newAction(REST_MODE); break;
			case 8: event->newAction(COMBAT_MODE); break;
		}
	}
    return(GUI_PASS);
}

bool CommandBar::try_selected_action() // return true if target is needed
{
	if(!event) event = game->get_event();

  if(game->get_game_type() == NUVIE_GAME_U6)
  {
	switch(selected_action)
	{
		case 1:
		case 4:
		case 5:
		case 6:
			if(game->get_player()->is_in_vehicle())
			{
				event->display_not_aboard_vehicle();
				return false;
			}
			break;
		default: break;
	}
	switch(selected_action)
	{
		case 0: return event->newAction(ATTACK_MODE);
		case 1: event->newAction(CAST_MODE); return false;
		case 2: return event->newAction(TALK_MODE);
		case 3: return event->newAction(LOOK_MODE);
		case 4: return event->newAction(GET_MODE);
		case 5: return event->newAction(DROP_MODE);
		case 6: return event->newAction(PUSH_MODE);
		case 7: return event->newAction(USE_MODE);
		case 8: event->newAction(REST_MODE); return false;
		case 9: event->newAction(COMBAT_MODE); return false;
		default: return false;
	}
  }
	else if(game->get_game_type() == NUVIE_GAME_MD)
	{
		switch(selected_action)
		{
			case 0: return event->newAction(ATTACK_MODE);
			case 1: return event->newAction(TALK_MODE);
			case 2: return event->newAction(LOOK_MODE);
			case 3: return event->newAction(GET_MODE);
			case 4: return event->newAction(DROP_MODE);
			case 5: return event->newAction(PUSH_MODE);
			case 6: return event->newAction(USE_MODE);
			case 7: event->newAction(COMBAT_MODE); return false;
			default: return false;
		}
	}
	else // SE
	{
		switch(selected_action)
		{
			case 0:
			case 1:
			case 2:
				if(game->get_player()->is_in_vehicle())
				{
					event->display_not_aboard_vehicle();
					return false;
				}
			default: break;
		}
		switch(selected_action)
		{
			case 0: return event->newAction(PUSH_MODE);
			case 1: return event->newAction(GET_MODE);
			case 2: return event->newAction(DROP_MODE);
			case 3: return event->newAction(USE_MODE);
			case 4: return event->newAction(TALK_MODE);
			case 5: return event->newAction(LOOK_MODE);
			case 6: return event->newAction(ATTACK_MODE);
			case 7: event->newAction(REST_MODE); return false;
			case 8: event->newAction(COMBAT_MODE); return false;
			default: return false;
		}
	}
	return false;
}

void CommandBar::set_combat_mode(bool mode)
{
    TileManager *tile_man = game->get_tile_manager();
    if(combat_mode != mode)
    {
        combat_mode = mode;
        if(game->get_game_type() == NUVIE_GAME_U6)
        {
            icon[9] = tile_man->get_tile(combat_mode ? 415 : 414);
            update_display = true;
        }
    }
    if (combat_mode) // make sure party attacks
    {
        Player *player = game->get_player();
        Party *party = player->get_party();
        party->follow(0, 0);
    }
}

void CommandBar::Display(bool full_redraw)
{
    Screen *screen = game->get_screen();

    if(full_redraw || update_display)
    {
        update_display = false;
      if(game->get_game_type() == NUVIE_GAME_U6)
      {
        screen->fill(bg_color, area.x, area.y, area.w, area.h);

        display_information();
        for(uint32 i = 0; i < 10; i++)
            screen->blit(area.x+i*16, area.y+8, icon[i]->data, 8, 16, 16, 16);

        if(selected_action >= 0 && selected_action <= 9)
            screen->fill(9, area.x + selected_action*16, area.y + 24, 16, 1);
      }
        else if(game->get_game_type() == NUVIE_GAME_SE
                && selected_action >= 0 && selected_action <= 8)
            fill_square(6);

        screen->update(area.x, area.y, area.w, area.h);
    }
}

void CommandBar::display_information()
{
    string infostring(game->get_clock()->get_date_string());
    infostring += " Wind:";
    infostring += wind;
    text->drawString(screen, infostring.c_str(), area.x + 8, area.y, 0);
}

uint16 CommandBar::callback(uint16 msg, CallBack *caller, void * data)
{
    Weather *weather = game->get_weather();

    if(caller == (CallBack *)weather && msg == WEATHER_CB_CHANGE_WIND_DIR)
    {
        wind = weather->get_wind_dir_str();
        update_display = true;
    }
    
    return 1;
}
