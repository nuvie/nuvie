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

#include "View.h"

class Configuration;
class TileManager;
class ObjManager;
class Screen;
class Actor;
class Text;
class Party;
class DollWidget;
class InventoryWidget;

class InventoryView : public View {
 
 DollWidget *doll_widget;
 InventoryWidget *inventory_widget;
 
 public:
 InventoryView(Configuration *cfg);
 ~InventoryView();
 
 bool init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om);
 bool set_party_member(uint8 party_member);
 
 void Display(bool full_redraw);
  
 protected:

 void display_name();
 void display_inventory_list();
 void add_command_icons(Screen *tmp_screen, void *view_manager);
 void display_inventory_weights();
 void display_combat_mode();
};

#endif /* __InventoryView_h__ */

