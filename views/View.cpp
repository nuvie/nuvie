/*
 *  View.cpp
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

#include "nuvieDefs.h"
#include "Party.h"
#include "ViewManager.h"
#include "View.h"

#include "GUI_widget.h"

View::View(Configuration *cfg) : GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
}

View::~View()
{
}

bool View::init(uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
 GUI_Widget::Init(NULL, x, y, 136, 96);

 text = t;
 party = p;
 tile_manager = tm;
 obj_manager = om;

 set_party_member(0);

 return true;
}

bool View::set_party_member(uint8 party_member)
{
 if(party_member < party->get_party_size())
   {
    cur_party_member = party_member;
    Redraw();
    return true;
   }

 return false;
}

bool View::next_party_member()
{
 return set_party_member(cur_party_member + 1);
}

bool View::prev_party_member()
{
 if(cur_party_member != 0)
   return set_party_member(cur_party_member - 1);

 return false;
}

GUI_status View::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
 ViewManager *view_manager;

 if(caller == (GUI_CallBack *)left_button)
   {
    prev_party_member();
    return GUI_YUM;
   }

 if(caller == (GUI_CallBack *)right_button)
   {
    next_party_member();
    return GUI_YUM;
   }

 if(caller == (GUI_CallBack *)actor_button)
   {
    view_manager = (ViewManager *)data;
    view_manager->set_actor_mode();
    return GUI_YUM;
   }

 if(caller == (GUI_CallBack *)party_button)
   {
    view_manager = (ViewManager *)data;
    view_manager->set_party_mode();
    return GUI_YUM;
   }

 if(caller == (GUI_CallBack *)inventory_button)
   {
    view_manager = (ViewManager *)data;
    view_manager->set_inventory_mode();
    return GUI_YUM;
   }

 return GUI_PASS;
}
