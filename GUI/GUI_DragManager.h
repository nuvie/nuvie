#ifndef __GUI_DragManager__
#define __GUI_DragManager__
/*
 *  GUI_DragManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Sep 01 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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

#include "GUI_status.h"
#include "GUI_DragArea.h"

// Drag Messages.
#define GUI_DRAG_OBJ 0

class Screen;

class GUI_DragManager {

protected:

 Screen *screen;

 GUI_DragArea *drag_source;
 int message;
 void *data;
 bool is_out_of_range;

public:

 GUI_DragManager(Screen *s);

 GUI_status start_drag(GUI_DragArea *src, int msg, void *d, unsigned char *icon_buf, uint16 w, uint16 h, uint8 bpp, bool out_of_range = false);
 void drop(GUI_DragArea *drag_target, int x, int y);

 // let us find out where the drag originated, because we might
 // need to do something different depending on where a dragged
 // object is coming from.
 GUI_DragArea *get_source();

 void draw(int x, int y);
};

#endif /* __GUI_DragManager__ */
