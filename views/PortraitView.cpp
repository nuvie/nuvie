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
#include "PortraitView.h"

PortraitView::PortraitView(Configuration *cfg) : View(cfg)
{
 portrait_data = NULL;
 name_string = new string;
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
 // cursor_x =
 // cursor_y =

 return true;
}

void PortraitView::Display(bool full_redraw)
{
 if(portrait_data != NULL/* && (full_redraw || update_display)*/)
  {
   update_display = false;
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   screen->blit(area.x+40,area.y+16,portrait_data,8,56,64,56,false);
   display_name();
   screen->update(area.x, area.y, area.w, area.h);
  }
  if(show_cursor) // FIXME: should we be using scroll's drawCursor?
   Game::get_game()->get_scroll()->drawCursor(area.x, area.y + area.h - 8);
}

bool PortraitView::set_portrait(Actor *actor, char *name)
{
 cur_actor_num = actor->get_actor_num();

 if(portrait_data != NULL)
   free(portrait_data);
   
 portrait_data = portrait->get_portrait_data(actor);

 if(portrait_data == NULL)
   return false;

 if(name == NULL)
   name_string->assign("");  //FIX
 else
   name_string->assign(name);

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
bool PortraitView::handle_input(const SDL_keysym *input)
{
    if(waiting)
    {
        waiting = false;
        // FIXME revert to previous status view
        Game::get_game()->get_view_manager()->set_inventory_mode();
        // Game::get_game()->get_scroll()->set_input_mode(false);
        Game::get_game()->get_scroll()->message("\n");
        Game::get_game()->get_scroll()->set_show_cursor(true);
        return(true);
    }
    return(false);
}
