#ifndef __View_h__
#define __View_h__

/*
 *  View.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri May 16 2003.
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

#include <SDL.h>

#include "U6def.h"
#include "Configuration.h"
#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Party.h"

class View
{
 protected:
 
 Configuration *config;
 uint16 origin_x;
 uint16 origin_y;
 
 Screen *screen;
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Party *party;

 public:
 
 View(Configuration *cfg);
 virtual ~View();
 
 virtual bool init(uint16 x, uint16 y, Screen *s, Text *t, Party *p, TileManager *tm, ObjManager *om);
 
 virtual void update_display()=0;
 virtual bool handle_input(SDLKey *input);

};

#endif /* __View_h__ */

