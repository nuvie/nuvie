/*
 *  ViewManager.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue May 20 2003.
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

#include "ViewManager.h"

ViewManager::ViewManager(Configuration *cfg)
{
 config = cfg;
 config->value("config/GameType",game_type);
}
 
ViewManager::~ViewManager()
{
 delete inventory_view;
}

bool ViewManager::init(Screen *s, Text *t, Party *p, TileManager *tm, ObjManager *om, Portrait *portrait)
{
 screen = s;
 text = t;
 party = p;
 tile_manager = tm;
 obj_manager = om;

 inventory_view = new InventoryView(config);
 inventory_view->init(176,8,screen, text, party, tile_manager, obj_manager);
 
 portrait_view = new PortraitView(config);
 portrait_view->init(176,8,screen, text, party, tile_manager, obj_manager, portrait);
 
 current_view = (View *)inventory_view;
 
 return true;
}

bool ViewManager::handle_input(SDLKey *input)
{
 return false;
}

void ViewManager::update_display()
{
 if(game_type == NUVIE_GAME_U6) //HACK! remove this when views support MD and SE
   {
    screen->fill(0x31, 176, 8, 136, 96);
    current_view->update_display();
    screen->update(176, 8, 136, 96);
   }
}

void ViewManager::set_portrait_mode(uint8 actor_num, char *name)
{
 portrait_view->set_portrait(actor_num, name);
 
 current_view = (View *)portrait_view;
 update_display();
}

void ViewManager::set_inventory_mode(uint8 actor_num)
{
 current_view = (View *)inventory_view;
 update_display();
}

void ViewManager::set_party_mode()
{
 return;
}

  