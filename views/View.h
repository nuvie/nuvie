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

#include "GUI_Widget.h"

GUI_status viewLeftButtonCallback(void *data);
GUI_status viewRightButtonCallback(void *data);

class View: public GUI_Widget
{
 protected:
 Configuration *config;
 
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Party *party;

 uint8 cur_party_member;
 
 public:
 
 View(Configuration *cfg);
 virtual ~View();
 
 virtual bool init(uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om);
 
 virtual bool set_party_member(uint8 party_member);
 uint8 get_party_member_num() { return cur_party_member; }
 bool next_party_member();
 bool prev_party_member();
};

#endif /* __View_h__ */

