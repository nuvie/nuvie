#ifndef __GUI_DIALOG_H__
#define __GUI_DIALOG_H__
/*
 *  GUI_Dialog.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Apr 03 2004.
 *  Copyright (c) 2004. All rights reserved.
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
#include "Screen.h"

#define GUI_DIALOG_MOVABLE true

class GUI_Dialog : public GUI_Widget {

    int old_x, old_y;
	Uint8 R, G, B;
	Uint32 bg_color;

    bool drag;
    SDL_Surface *border[8];

public:
	/* Passed the area, color and shape */
	GUI_Dialog(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, bool is_moveable);

	/* Map the color to the display */
	virtual void SetDisplay(Screen *s);

	/* Show the widget  */
	virtual void Display(bool full_redraw);

    /* events, used for dragging the area. */
    GUI_status MouseDown(int x, int y, int button);
    GUI_status MouseUp(int x, int y, int button);
    GUI_status MouseMotion(int x,int y,Uint8 state);

protected:
    void loadBorderImages();
};

#endif /* __GUI_DIALOG_H__ */
