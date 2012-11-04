#ifndef __ContainerWidgetGump_h__
#define __ContainerWidgetGump_h__

/*
 *  ContainerWidgetGump.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Mar 20 2012.
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
#include "ObjManager.h"
#include "ContainerWidget.h"

class Configuration;
class TileManager;
class Actor;
class Text;

#define CONTAINER_WIDGET_GUMP_HEIGHT 48

class ContainerWidgetGump : public ContainerWidget {

 public:
 ContainerWidgetGump(Configuration *cfg, GUI_CallBack *callback = NULL);
 ~ContainerWidgetGump();

 bool init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, Text *t);
};

#endif /* __ContainerWidgetGump_h__ */

