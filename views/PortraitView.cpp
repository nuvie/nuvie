/*
 *  PortraitView.cpp
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

#include <string>

#include "nuvieDefs.h"

#include "Game.h"
#include "Actor.h"
#include "Portrait.h"
#include "Text.h"
#include "ViewManager.h"
#include "MsgScroll.h"
#include "GUI.h"
#include "DollWidget.h"
#include "PortraitView.h"

PortraitView::PortraitView(Configuration *cfg) : View(cfg)
{
 portrait_data = NULL;
 name_string = new string;
 doll_widget = NULL;
}

PortraitView::~PortraitView()
{
 if(portrait_data != NULL)
   free(portrait_data);

 delete name_string;
}

bool PortraitView::init(uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om, Portrait *port)
{
 View::init(x,y,t,p,tm,om);

 portrait = port;
 cur_actor_num = 0;
 waiting = false;
 show_cursor = false;
 display_doll = false;
 // cursor_x =
 // cursor_y =

 doll_widget = new DollWidget(config, this);
 doll_widget->init(NULL, 0, 16, tile_manager, obj_manager);

 AddWidget(doll_widget);
 doll_widget->Hide();
 
 return true;
}

void PortraitView::Display(bool full_redraw)
{
 if(portrait_data != NULL/* && (full_redraw || update_display)*/)
  {
   update_display = false;
   if(display_doll)
   screen->blit(area.x+72,area.y+16,portrait_data,8,56,64,56,false);
   else
   screen->blit(area.x+40,area.y+16,portrait_data,8,56,64,56,false);
   
   display_name();
   screen->update(area.x, area.y, area.w, area.h);
  }
  if(show_cursor) // FIXME: should we be using scroll's drawCursor?
   {
    screen->fill(0x31, area.x, area.y + area.h - 8, 8, 8);
    Game::get_game()->get_scroll()->drawCursor(area.x, area.y + area.h - 8);
   } 
   DisplayChildren(full_redraw);
}

bool PortraitView::set_portrait(Actor *actor, char *name)
{
 cur_actor_num = actor->get_actor_num();
 int doll_x_offset = 0;

 if(portrait_data != NULL)
   free(portrait_data);

 portrait_data = portrait->get_portrait_data(actor);

 if(actor->has_readied_objects())
   {
    if(portrait_data == NULL)
      doll_x_offset = 34;
      
    doll_widget->MoveRelativeToParent(doll_x_offset, 16);

    display_doll = true;
    doll_widget->Show();
    doll_widget->set_actor(actor);
   }
 else
   {
    display_doll = false;
    doll_widget->Hide();
    doll_widget->set_actor(NULL);
    
    if(portrait_data == NULL)
      return false;
   }

 if(name == NULL)
   name_string->assign("");  //FIX
 else
   name_string->assign(name);
 
 if(screen)
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   
 Redraw();
 return true;
}

void PortraitView::display_name()
{
 const char *name;

 name = name_string->c_str();

 text->drawString(screen, name, area.x + (136 - strlen(name) * 8) / 2, area.y+80, 0);

 return;
}


/* On any input return to previous status view if waiting.
 * Returns true if event was used.
 */
GUI_status PortraitView::HandleEvent(const SDL_Event *event)
{
    if(waiting
       && (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_KEYDOWN))
    {
        // FIXME revert to previous status view
        Game::get_game()->get_view_manager()->set_inventory_mode();
        // Game::get_game()->get_scroll()->set_input_mode(false);
        Game::get_game()->get_scroll()->message("\n");
        set_waiting(false);
        return(GUI_YUM);
    }
    return(GUI_PASS);
}


/* Start/stop waiting for input to continue, and (for now) steal cursor from
 * MsgScroll.
 */
void PortraitView::set_waiting(bool state)
{
    if(state == true && display_doll == false && portrait_data == NULL) // don't wait for nothing
        return;
    waiting = state;
    set_show_cursor(waiting);
    Game::get_game()->get_scroll()->set_show_cursor(!waiting);
    Game::get_game()->get_gui()->lock_input(waiting ? this : NULL);
}
