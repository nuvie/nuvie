#ifndef __ActorView_h__
#define __ActorView_h__

/*
 *  ActorView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Aug 22 2003.
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

#include <string>

#include "U6def.h"
#include "Configuration.h"
#include "View.h"
#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Party.h"
#include "Portrait.h"

class ActorView : public View {
 
 Portrait *portrait;
 std::string name_string;
 
 unsigned char *portrait_data;
 
 public:
 ActorView(Configuration *cfg);
 ~ActorView();
 
 bool init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om, Portrait *port);
 
 bool set_party_member(uint8 party_member);
  
 void Display(bool full_redraw);
  
 protected:
 
 void add_command_icons(Screen *tmp_screen, void *view_manager);
 void display_name();
 
};

#endif /* __ActorView_h__ */

