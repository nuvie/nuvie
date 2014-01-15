#ifndef __DraggableView_h__
#define __DraggableView_h__

/*
 *  DraggableView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Mar 7 2012.
 *  Copyright (c) 2012. All rights reserved.
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

#include "View.h"

class Configuration;
class GUI_Button;

class DraggableView: public View 
{
protected:
	SDL_Surface *bg_image;
	Uint32 bg_color_key;

private:
	bool drag, need_full_redraw_when_moved, always_need_full_redraw_when_moved;
    int button_x, button_y;

public:
	DraggableView(Configuration *config);
	virtual ~DraggableView();

    /* events, used for dragging the area. */
    GUI_status MouseDown(int x, int y, int button);
    GUI_status MouseUp(int x, int y, int button);
    GUI_status MouseMotion(int x,int y,Uint8 state);
    void MoveRelative(int dx,int dy);

protected:
    void set_bg_color_key(Uint8 r, Uint8 g, Uint8 b);
    void force_full_redraw_if_needed();
};

#endif /* __DraggableView_h__ */
