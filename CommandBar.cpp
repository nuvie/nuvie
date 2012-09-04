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

    bg_color = game->get_palette()->get_bg_color();
    if(game->is_new_style())
        return;

    if(game->get_game_type() == NUVIE_GAME_U6)
        init_buttons();

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

void CommandBar::select_action(sint8 activate)
{
    if(game->is_orig_style() && game->get_game_type() == NUVIE_GAME_SE) // black out previous setting
        fill_square(0);
    if(selected_action == activate) // clear if already selected
        set_selected_action(-1);
    else
        set_selected_action(activate);
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
            select_action(activate);
        }
    }
    return(GUI_YUM);
}

GUI_status CommandBar::hit(uint8 num)
{
    if(!event) event = game->get_event();

    if(event->get_mode() != MOVE_MODE && event->get_mode() != EQUIP_MODE)
        return GUI_PASS;

    try_selected_action(num);

    return(GUI_YUM);
}

static const EventMode U6_mode_tbl[] ={ ATTACK_MODE, CAST_MODE, TALK_MODE, LOOK_MODE, GET_MODE,
                                        DROP_MODE, PUSH_MODE, USE_MODE, REST_MODE, COMBAT_MODE };
static const EventMode MD_mode_tbl[] ={ ATTACK_MODE, TALK_MODE, LOOK_MODE, GET_MODE,
                                        DROP_MODE, PUSH_MODE, USE_MODE, COMBAT_MODE };
static const EventMode SE_mode_tbl[] ={ PUSH_MODE, GET_MODE, DROP_MODE, USE_MODE, TALK_MODE,
                                        LOOK_MODE, ATTACK_MODE, REST_MODE, COMBAT_MODE };

bool CommandBar::try_selected_action(sint8 command_num) // return true if target is needed
{
	if(!event) event = game->get_event();

	if(command_num == -1)
		command_num = selected_action;

	if(command_num == -1) // might happen if changing selected action when in EQUIP_MODE
		return false;

	EventMode mode;

	if(game->get_game_type() == NUVIE_GAME_U6)
		mode = U6_mode_tbl[command_num];
	else if(game->get_game_type() == NUVIE_GAME_MD)
		mode = MD_mode_tbl[command_num];
	else // SE
		mode = SE_mode_tbl[command_num];

	switch(mode)
	{
		case CAST_MODE:
		case GET_MODE:
		case DROP_MODE:
		case PUSH_MODE:
			if(game->get_player()->is_in_vehicle())
			{
				event->display_not_aboard_vehicle();
				return false;
			}
			break;
		default: break;
	}

	event->newAction(mode);

	if(mode < REST_MODE) // needs target
		return true;
	else
		return false;
}

void CommandBar::set_combat_mode(bool mode)
{
    TileManager *tile_man = game->get_tile_manager();
    if(combat_mode != mode)
    {
        combat_mode = mode;
        if(game->is_orig_style() && game->get_game_type() == NUVIE_GAME_U6)
        {
            icon[9] = tile_man->get_tile(combat_mode ? 415 : 414);
            update_display = true;
        }
    }

    //FIXME this probably should be moved else where. I think this was added to get party to fight first. It may not be needed now.
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
