#ifndef __CommandBarNewUI_h__
#define __CommandBarNewUI_h__
/*
 *  CommandBarNewUI.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Oct 7 2012.
 *  Copyright (c) 2012 The Nuvie Team. All rights reserved.
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
#include "CommandBar.h"

class NuvieIO;
class Event;
class Game;
class GUI_Button;
class GUI_CallBack;
class Text;
class Font;

class CommandBarNewUI: public CommandBar
{
protected:
	uint8 cur_pos;
	uint8 icon_w;
	uint8 icon_h;
	uint8 icon_y_offset;
	uint16 num_icons;

	Font *font;
public:
    CommandBarNewUI(Game *g);
    ~CommandBarNewUI();

    virtual void Display(bool full_redraw);
    virtual GUI_status KeyDown(SDL_Keysym key);
    virtual GUI_status MouseDown(int x, int y, int button);
    virtual GUI_status MouseUp(int x, int y, int button);

private:
    const char *get_command_name(sint8 command_num);
};

#endif /* __CommandBarNewUI_h__ */
