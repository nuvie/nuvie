#ifndef __InventoryView_h__
#define __InventoryView_h__

/*
 *  InventoryView.h
 *  Nuvie
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
#include "View.h"
#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Party.h"

class InventoryView : public View {
 
 uint8 party_position; // the position in the party 0 = avatar, 1 dupre. etc.
 
 public:
 InventoryView(Configuration *cfg);
 ~InventoryView();
 
 bool init(Screen *tmp_screen, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om);
 
 bool show_party_member(uint8 party_member);
 bool show_next_party_member();
 bool show_prev_party_member();

 void Display(bool full_redraw);
 void display_doll(uint16 x, uint16 y);
 
 protected:
 
 void display_name();
 void display_inventory_list();
 void add_command_icons(Screen *tmp_screen);
 void display_inventory_weights();
 void display_actor_icon();
};

#endif /* __InventoryView_h__ */

