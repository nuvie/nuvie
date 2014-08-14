#ifndef __MDSkyStripWidget_h__
#define __MDSkyStripWidget_h__

/*
 *  MDSkyStripWidget.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Aug 13 2014.
 *  Copyright (c) 2014. All rights reserved.
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
#include "U6Shape.h"

class GameClock;
class Configuration;
class Player;

class MDSkyStripWidget : public GUI_Widget {

protected:
 Configuration *config;
 GameClock *clock;
 U6Shape strip1, strip2;
 Player *player;

 public:
 MDSkyStripWidget(Configuration *cfg, GameClock *c, Player *p);
 ~MDSkyStripWidget();

 void init(sint16 x, sint16 y);
 void Display(bool full_redraw);

 private:
 void display_surface();
};

#endif /* __MDSkyStripWidget_h__ */

