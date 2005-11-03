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

#include "nuvieDefs.h"
#include "Game.h"
#include "Screen.h"
#include "Event.h"
#include "TileManager.h"
#include "Text.h"
#include "GameClock.h"
#include "CommandBar.h"

#define USE_BUTTON 1
#define ACTION_BUTTON 3

using std::string;

CommandBar::CommandBar(Game *g) : GUI_Widget(NULL, 8, 168, 0, 0)
{
    game = g;
    event = NULL; // it's not set yet
    text = game->get_text();

    area.w = 16 * 10; // space for 10 icons
    area.h = 24 + 1; // extra space for the underlined default action

    default_action = -1;
    combat_mode = false;
    wind = "?";

    init_buttons();
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

GUI_status CommandBar::MouseDown(int x, int y, int button)
{
    x -= area.x;
    y -= area.y;
    if(y >= 8 && y <= 24)
    {
        uint8 activate = x / 16; // icon selected
        if(button == USE_BUTTON)
            return(hit(activate));
        else if(button == ACTION_BUTTON)
        {
            if(default_action == activate) // clear if already selected
                set_default_action(-1);
	    else if(try_default_action(activate)) // set in Event and here
                set_default_action(activate);
        }
    }
    return(GUI_YUM);
}

GUI_status CommandBar::hit(uint8 num)
{
    if(!event) event = game->get_event();
    switch(num) // hit button
    {
        case 0: event->newAction(ATTACK_MODE); break;
        case 1: event->newAction(CAST_MODE); break;
        case 2: event->newAction(TALK_MODE); break;
        case 3: event->newAction(LOOK_MODE); break;
        case 4: event->newAction(GET_MODE); break;
        case 5: event->newAction(DROP_MODE); break;
        case 6: event->newAction(PUSHSELECT_MODE); break;
        case 7: event->newAction(USE_MODE); break;
        case 8: printf("Rest\n"); break;
        case 9:
//            if(event->toggle_combat())
//            {
                if(combat_mode == false) set_combat_mode(true);
                else                     set_combat_mode(false);
//            }
            printf("%s Combat!\n",combat_mode?"Begin":"Break off");
    }
    return(GUI_PASS);
}

/* Returns true if the default action is set with Event. */
bool CommandBar::try_default_action(uint8 activate)
{
/*
    return((activate == 0 && event->set_default_action(ATTACK_MODE))
        || (activate == 1 && event->set_default_action(CAST_MODE))
        || (activate == 2 && event->set_default_action(TALK_MODE))
        || (activate == 3 && event->set_default_action(LOOK_MODE))
        || (activate == 4 && event->set_default_action(GET_MODE))
        || (activate == 5 && event->set_default_action(DROP_MODE))
        || (activate == 6 && event->set_default_action(PUSHSELECT_MODE))
        || (activate == 7 && event->set_default_action(USE_MODE))
        || (activate == 8 && event->set_default_action(REST_MODE))
        || (activate == 9 && event->set_default_action(COMBAT_MODE)));*/
    return(true);
}

void CommandBar::set_combat_mode(bool mode)
{
    TileManager *tile_man = game->get_tile_manager();
    if(combat_mode != mode)
    {
        combat_mode = mode;
        icon[9] = tile_man->get_tile(combat_mode ? 415 : 414);
        update_display = true;
    }
}

void CommandBar::Display(bool full_redraw)
{
    Screen *screen = game->get_screen();

    if(full_redraw || update_display)
    {
        update_display = false;
        screen->fill(0x31, area.x, area.y, area.w, area.h);

        display_information();
        for(uint32 i = 0; i < 10; i++)
            screen->blit(area.x+i*16, area.y+8, icon[i]->data, 8, 16, 16, 16);

        if(default_action >= 0 && default_action <= 9)
            screen->fill(9, area.x + default_action*16, area.y + 24, 16, 1);
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
