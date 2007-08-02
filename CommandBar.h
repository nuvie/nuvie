#ifndef __CommandBar_h__
#define __CommandBar_h__
/*
 *  CommandBar.h
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
#include <string>
#include "GUI_widget.h"
#include "CallBack.h"

class Event;
class Game;
class GUI_Button;
class GUI_CallBack;
class Text;

/* U6 command buttons.
 * [Attack][Cast][Talk][Look][Get][Drop][Move][Use][Rest][Combat]
 * Left click: New action if none is pending.
 * Right click: Set default action.
 * Information: [A]M-DD-YYYY Wind: W[B]
 */
class CommandBar: public GUI_Widget, public CallBack
{
protected:
    Game *game;
    Event *event;
    Text *text;
    Tile *icon[10];

    sint8 default_action; // underlined icon (-1 = none)
    bool combat_mode; // state of combat icon
    std::string wind; // wind direction

    uint8 bg_color;

    virtual void display_information();
    virtual GUI_status hit(uint8 num);

public:
    CommandBar(Game *g);
    ~CommandBar();
    virtual bool init_buttons();

    virtual void Display(bool full_redraw);
    virtual GUI_status MouseDown(int x, int y, int button);
    void update() { update_display = true; }

    void set_combat_mode(bool mode);
    void set_wind(std::string dir)        { wind = dir; update_display = true; }
    void set_default_action(sint8 action) { default_action = action;
                                            update_display = true; }
    bool try_default_action(uint8 activate);
    
    uint16 callback(uint16 msg, CallBack *caller, void * data);
};

#endif /* __CommandBar_h__ */
