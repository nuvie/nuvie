/*
 *  InventoryView.h
 *  Nuive
 *
 *  Created by Eric Fry on Tue May 13 2003.
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

#include "U6def.h"
#include "Configuration.h"
#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Party.h"

class InventoryView {
 
 Configuration *config;
 Screen *screen;
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 Party *party;
 uint8 current_player;
 
 //cursor pos
 
 public:
 
 InventoryView(Configuration *cfg);
 ~InventoryView();
 
 bool init(Screen *s, Text *t, Party *p, TileManager *tm, ObjManager *om);
 void update_display();
 bool handle_input(SDLKey *input);
 void display_doll(uint16 x, uint16 y);
 
 protected:
 
 void display_name();
 void display_inventory_list();
 void display_command_icons();
};
