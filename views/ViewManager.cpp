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

#include "nuvieDefs.h"
#include "Configuration.h"

#include "GUI.h"

#include "ViewManager.h"

#include "Actor.h"

#include "ActorView.h"
#include "PortraitView.h"
#include "InventoryView.h"
#include "PartyView.h"

ViewManager::ViewManager(Configuration *cfg)
{
 config = cfg;
 config->value("config/GameType",game_type);
 current_view = NULL;
}
 
ViewManager::~ViewManager()
{
 // only delete the views that are not currently active
 if (current_view != actor_view)     delete actor_view;
 if (current_view != inventory_view) delete inventory_view;
 if (current_view != party_view)     delete party_view;
 if (current_view != portrait_view)  delete portrait_view;
}

bool ViewManager::init(GUI *g, Text *t, Party *p, Player *player, TileManager *tm, ObjManager *om, Portrait *portrait)
{
 gui = g;
 text = t;
 party = p;
 tile_manager = tm;
 obj_manager = om;

 actor_view = new ActorView(config);
 actor_view->init(gui->get_screen(), this, 176,8, text, party, tile_manager, obj_manager, portrait);
 
 inventory_view = new InventoryView(config);
 inventory_view->init(gui->get_screen(), this, 176,8, text, party, tile_manager, obj_manager);
 
 party_view = new PartyView(config);
 party_view->init(this,176,8, text, party, player, tile_manager, obj_manager);

 portrait_view = new PortraitView(config);
 portrait_view->init(176,8, text, party, tile_manager, obj_manager, portrait);
 
 set_current_view((View *)inventory_view);
 
 return true;
}

bool ViewManager::set_current_view(View *view)
{
 uint8 cur_party_member;
 
 //actor_view->set_party_member(cur_party_member);
 if(view == NULL || game_type != NUVIE_GAME_U6) //HACK! remove this when views support MD and SE
   return false;

 if(current_view == view) // nothing to do if view is already the current_view.
   return false;
   
 if(current_view != NULL)
   {
    gui->removeWidget((GUI_Widget *)current_view);//remove current widget from gui

    if(current_view != party_view) // party_view doesn't use member num
       {
        cur_party_member = current_view->get_party_member_num();
        view->set_party_member(cur_party_member);
       }
   }
   
 current_view = view;
 gui->AddWidget((GUI_Widget *)view);
 
 view->Redraw();
 
 return true;
}

void ViewManager::update()
{
 if(current_view)
   current_view->Redraw();

 return;
}

// We only change to portrait mode if the actor has a portrait.
void ViewManager::set_portrait_mode(Actor *actor, char *name)
{
 if(portrait_view->set_portrait(actor, name) == true)
  {
   set_current_view((View *)portrait_view);
  }
}

void ViewManager::set_inventory_mode()
{
 set_current_view((View *)inventory_view);
}

void ViewManager::set_party_mode()
{
 set_current_view((View *)party_view);
 return;
}

void ViewManager::set_actor_mode()
{

 
 set_current_view((View *)actor_view);
}


// callbacks for switching views

GUI_status partyViewButtonCallback(void *data)
{
 ViewManager *view_manager;
 
 view_manager = (ViewManager *)data;
 
 view_manager->set_party_mode();
  
 return GUI_YUM;
}

GUI_status actorViewButtonCallback(void *data)
{
 ViewManager *view_manager;
 
 view_manager = (ViewManager *)data;
 
 view_manager->set_actor_mode();
  
 return GUI_YUM;
}

GUI_status inventoryViewButtonCallback(void *data)
{
 ViewManager *view_manager;
 
 view_manager = (ViewManager *)data;
 
 view_manager->set_inventory_mode();
  
 return GUI_YUM;
}
