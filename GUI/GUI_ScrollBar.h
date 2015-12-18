#ifndef __GUI_SCROLLBAR_H__
#define __GUI_SCROLLBAR_H__
/*
 *  GUI_ScrollBar.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Apr 04 2004.
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

#define SCROLLBAR_WIDTH 14

// Callback message types

#define SCROLLBAR_CB_DOWN_BUTTON  0x1
#define SCROLLBAR_CB_UP_BUTTON    0x2
#define SCROLLBAR_CB_SLIDER_MOVED 0x3
#define SCROLLBAR_CB_PAGE_DOWN    0x4
#define SCROLLBAR_CB_PAGE_UP      0x5

class GUI_Button;

class GUI_ScrollBar : public GUI_Widget {
    GUI_CallBack *callback_object;
    GUI_Button *up_button, *down_button;

    bool drag;
    uint16 button_height;

    // Various colours.

    uint32 slider_highlight_c;
    uint32 slider_shadow_c;
    uint32 slider_base_c;
    uint32 track_border_c;
    uint32 track_base_c;

    uint16 track_length;
    uint16 slider_length;
    uint16 slider_y;
    uint16 slider_click_offset; // where on the slider were we clicked?

public:
	/* Passed the area, color and shape */
	GUI_ScrollBar(int x, int y, int h, GUI_CallBack *callback);

    void set_slider_length(float percentage);
    void set_slider_position(float percentage);

	/* Map the color to the display */
	virtual void SetDisplay(Screen *s);

	/* Show the widget  */
	virtual void Display(bool full_redraw);

    /* events, used for dragging the area. */
    GUI_status MouseDown(int x, int y, int button);
    GUI_status MouseUp(int x, int y, int button);
    GUI_status MouseMotion(int x,int y,Uint8 state);
    GUI_status MouseWheel(sint32 x, sint32 y);

protected:
    void loadButtons();
    void DisplaySlider();

    void send_slider_moved_msg();
    bool move_slider(int new_slider_y);
    GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);

    void send_up_button_msg();
    void send_down_button_msg();
};

#endif /* __GUI_SCROLLBAR_H__ */
