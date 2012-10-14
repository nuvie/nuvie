#ifndef __FpsCounter_h__
#define __FpsCounter_h__
/*
 *  FpsCounter.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Oct 14 2012.
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

#include "GUI_widget.h"

class Game;

class FpsCounter: public GUI_Widget
{
protected:

	Game *game;
	Font *font;

	char fps_string[7]; // "000.00\0"

public:
    FpsCounter();
    FpsCounter(Game *g);
    ~FpsCounter();

    void setFps(float fps);

    virtual void Display(bool full_redraw);

    void update() { update_display = true; }
};

#endif /* __FpsCounter_h__ */
