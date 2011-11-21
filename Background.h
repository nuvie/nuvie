#ifndef __Background_h__
#define __Background_h__

/*
 *  Background.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Aug 24 2003.
 *  Copyright (c) 2003. All rights reserved.
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

class Configuration;
class U6Shape;

class Background: public GUI_Widget
{
 Configuration *config;
 int game_type;

 U6Shape *background;
 uint16 bg_w, bg_h;
 uint16 x_off, y_off;

 public:

 Background(Configuration *cfg);
 ~Background();

 bool init();

 void Display(bool full_redraw);

};


#endif /* __Background_h__ */

