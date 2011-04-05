#ifndef __GUI_CONSOLE_H__
#define __GUI_CONSOLE_H__
/*
 *  GUI_Console.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Apr 4 2011.
 *  Copyright (c) 2011. All rights reserved.
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

#include "SDL.h"

#include "GUI_widget.h"
#include "GUI_types.h"
#include "GUI_font.h"
#include "Screen.h"

class GUI_Console : public GUI_Widget {

	GUI_Color *bg_color;
	GUI_Font *font;
    uint16 num_cols;
    uint16 num_rows;
    std::list<std::string> data;

public:
	GUI_Console(uint16 w, uint16 h);
	~GUI_Console();

	/* Map the color to the display */
	virtual void SetDisplay(Screen *s);

	/* Show the widget  */
	virtual void Display(bool full_redraw);

    /* events, used for dragging the area. */
    GUI_status MouseDown(int x, int y, int button);
    GUI_status MouseUp(int x, int y, int button);
    GUI_status MouseMotion(int x,int y,Uint8 state);

    virtual void AddLine(std::string line);

protected:

};

#endif /* __GUI_CONSOLE_H__ */
