/*
 *  InventoryView.cpp
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
#include <cassert>
#include "nuvieDefs.h"

#include "Screen.h"
#include "U6LList.h"
#include "GUI_button.h"
#include "DollWidget.h"
#include "InventoryWidget.h"
#include "InventoryView.h"
#include "Party.h"
#include "Text.h"
#include "Actor.h"
#include "Event.h"
#include "MapWindow.h"
#include "MsgScroll.h"
#include "UseCode.h"
#include "ViewManager.h"

static const char combat_mode_tbl[][8] = {"COMMAND", " FRONT", "  REAR", " FLANK", "BERSERK", "RETREAT", "ASSAULT"};
static const char combat_mode_tbl_se[][6] = {"CMND", "RANGE", "FLEE", "CLOSE"};
static const char combat_mode_tbl_md[][6] = {"CMND", "RANGE", "FLEE", "ATTK"};
static const int first_combat_mode = 0x2;
static const int last_combat_mode = 0x8;
#define MD Game::get_game()->get_game_type()==NUVIE_GAME_MD

InventoryView::InventoryView(Configuration *cfg) : View(cfg),
   doll_widget(NULL), inventory_widget(NULL)
{
 cursor_pos.area = INVAREA_LIST;
 cursor_pos.x = cursor_pos.y = 0;
 cursor_pos.px = cursor_pos.py = 0;
 cursor_tile = NULL;
 show_cursor = false;
 is_party_member = false;
}

InventoryView::~InventoryView()
{
}

bool InventoryView::set_party_member(uint8 party_member)
{

 if(View::set_party_member(party_member)
    && party->main_actor_is_in_party())
  {
   is_party_member = true;
   if(doll_widget)
     doll_widget->set_actor(party->get_actor(cur_party_member));
   if(inventory_widget)
     inventory_widget->set_actor(party->get_actor(cur_party_member));

   show_buttons();

   if(combat_button)
   {
     if(party_member == 0)
       combat_button->Hide();
     else
       combat_button->Show();
   }
   
   return true;
  }
 is_party_member = false;
 hide_buttons();

 return false;
}

bool InventoryView::set_actor(Actor *actor)
{
   is_party_member = false;
   if(doll_widget)
     doll_widget->set_actor(actor);
   if(inventory_widget)
     inventory_widget->set_actor(actor);

   hide_buttons();

   return true;
}

bool InventoryView::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
 if(Game::get_game()->get_game_type() == NUVIE_GAME_U6)
	View::init(x,y,t,p,tm,om);
 else
	View::init(x-8,y-2,t,p,tm,om);

 doll_widget = new DollWidget(config, this);
 doll_widget->init(party->get_actor(cur_party_member), 0, 8, tile_manager, obj_manager);

 AddWidget(doll_widget);

 inventory_widget = new InventoryWidget(config, this);
 inventory_widget->init(party->get_actor(cur_party_member), 64, 8, tile_manager, obj_manager, text);

 AddWidget(inventory_widget);

 add_command_icons(tmp_screen, view_manager);
 if(Game::get_game()->get_game_type() == NUVIE_GAME_U6)
	cursor_tile = tile_manager->get_tile(365);
 else if(MD)
	cursor_tile = tile_manager->get_tile(265);
 else // SE
	cursor_tile = tile_manager->get_tile(381);

// update_cursor(); moved to PlaceOnScreen

 return true;
}


void InventoryView::PlaceOnScreen(Screen *s, GUI_DragManager *dm, int x, int y)
{
 GUI_Widget::PlaceOnScreen(s,dm,x,y);
 update_cursor(); // initial position; uses area
}


void InventoryView::Display(bool full_redraw)
{
	full_redraw = true;
 if(full_redraw || update_display)
   {
    screen->fill(bg_color, area.x, area.y, area.w, area.h);

    if(is_party_member)
        display_combat_mode();
    display_name();
    display_inventory_weights();
    //display_command_icons();


   }

 //display_doll(area.x,area.y+8);
 //display_inventory_list();

 DisplayChildren(full_redraw);

 if(full_redraw || update_display)
   {
    update_display = false;
    screen->update(area.x, area.y, area.w, area.h);
   }

 if(show_cursor && cursor_tile != NULL)
   {
    screen->blit(cursor_pos.px, cursor_pos.py, (unsigned char *)cursor_tile->data,
                 8, 16, 16, 16, true, NULL);
    screen->update(cursor_pos.px, cursor_pos.py, 16, 16);
   }

 return;
}

void InventoryView::display_name()
{
 char *name;

 if(is_party_member)
  name = party->get_actor_name(cur_party_member);
 else
  name = (char *) Game::get_game()->get_player()->get_actor()->get_name(true);
 if(name == NULL)
  return;

 text->drawString(screen, name, area.x + ((136) - strlen(name) * 8) / 2, area.y, 0);

 return;
}

void InventoryView::display_inventory_list()
{
 Tile *tile, *empty_tile;
 Actor *actor;
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 uint16 i,j;

 if(Game::get_game()->get_game_type() == NUVIE_GAME_U6)
   empty_tile = tile_manager->get_tile(410);
 else if(MD) // FIXME: different depending on npc
   empty_tile = tile_manager->get_tile(273);
 else
   empty_tile = tile_manager->get_tile(392);

 actor = party->get_actor(cur_party_member);

 inventory = actor->get_inventory_list();

 link = inventory->start();

  for(i=0;i<3;i++)
   {
    for(j=0;j<4;j++)
      {
       if(link != NULL)
         {
          obj = (Obj *)link->data;
          if(obj->is_readied()) //skip any readied objects
            {
             for(;link != NULL && obj->is_readied(); link = link->next)
                obj = (Obj *)link->data;
            }
          else
            link = link->next;

          tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
          if(link == NULL)
            {
             if(obj->is_readied()) //last object is readied so skip it.
                tile = empty_tile;
            }
         }
        else
          tile = empty_tile;

       //tile = tile_manager->get_tile(actor->indentory_tile());
       screen->blit((area.x+4*16+8)+j*16,area.y+16+8+i*16,tile->data,8,16,16,16,true);
      }
   }
}

void InventoryView::add_command_icons(Screen *tmp_screen, void *view_manager)
{
 Tile *tile;
 int y = 96;
 if(MD)
    y = 100;
 else if(Game::get_game()->get_game_type() == NUVIE_GAME_U6)
	y = 80;
 SDL_Surface *button_image;
 SDL_Surface *button_image2;
 //FIX need to handle clicked button image, check image free on destruct.

 tile = tile_manager->get_tile(MD?282:387); //left arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 left_button = new GUI_Button(this, 0, y, button_image, button_image2, this);
 this->AddWidget(left_button);

 tile = tile_manager->get_tile(MD?279:384); //party view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 party_button = new GUI_Button(view_manager, 16, y, button_image, button_image2, this);
 this->AddWidget(party_button);

 tile = tile_manager->get_tile(MD?280:385); //actor view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 actor_button = new GUI_Button(view_manager, 2*16, y, button_image, button_image2, this);
 this->AddWidget(actor_button);

 tile = tile_manager->get_tile(MD?283:388); //right arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 right_button = new GUI_Button(this, 3*16, y, button_image, button_image2, this);
 this->AddWidget(right_button);

 if(MD)
	tile = tile_manager->get_tile(285); //combat icon
 else if(Game::get_game()->get_game_type() == NUVIE_GAME_SE)
	tile = tile_manager->get_tile(365); //combat icon
 else
	tile = tile_manager->get_tile(391); //combat icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 combat_button = new GUI_Button(this, 4*16, y, button_image, button_image2, this); //FIX combat
 this->AddWidget(combat_button);

}

void InventoryView::display_inventory_weights()
{
 uint8 strength;
 float inv_weight;
 float equip_weight;
 Actor *actor;
 if(is_party_member)
	actor = party->get_actor(cur_party_member);
 else
	actor = Game::get_game()->get_player()->get_actor();
 char string[9]; //  "E:xx/xxs"

 strength = actor->get_strength();
 //FIX weight isn't correct.
 inv_weight = actor->get_inventory_weight();
 equip_weight = actor->get_inventory_equip_weight();

 snprintf(string,9,"E:%d/%ds",(int)equip_weight,strength);
 text->drawString(screen, string, area.x, area.y+72, 0);

 snprintf(string,9,"I:%d/%ds",(int)inv_weight,strength*2);
 text->drawString(screen, string, area.x+4*16+8, area.y+72, 0);
}

void InventoryView::display_combat_mode()
{
 Actor *actor = party->get_actor(cur_party_member);
 if(Game::get_game()->get_game_type() == NUVIE_GAME_U6)
   text->drawString(screen, combat_mode_tbl[actor->get_combat_mode() - 2], area.x+5*16, area.y+88, 0);
 else if(MD) // FIXME: selection isn't right
   text->drawString(screen, combat_mode_tbl_md[actor->get_combat_mode()], area.x+5*16, area.y+88, 0);
 else // SE - FIXME: selection isn't right
   text->drawString(screen, combat_mode_tbl_se[actor->get_combat_mode()], area.x+5*16, area.y+88, 0);
}

/* Move the cursor around, ready or unready objects, select objects, switch
 * to container view, use command icons.
 */
GUI_status InventoryView::KeyDown(SDL_keysym key)
{
//    Event *event = Game::get_game()->get_event();
//    ViewManager *view_manager = Game::get_game()->get_view_manager();

    if(!show_cursor) // FIXME: don't rely on show_cursor to get/pass focus
        return(GUI_PASS);
    switch(key.sym)
    {
		//	keypad arrow keys (moveCursorRelative doesn't accept diagonals)
        case SDLK_KP1:
            moveCursorRelative(0, 1); moveCursorRelative(-1, 0);
            break;
        case SDLK_KP3:
            moveCursorRelative(0, 1); moveCursorRelative(1, 0);
            break;
        case SDLK_KP7:
            moveCursorRelative(0, -1); moveCursorRelative(-1, 0);
            break;
        case SDLK_KP9:
            moveCursorRelative(0, -1); moveCursorRelative(1, 0);
            break;

        case SDLK_UP:
        case SDLK_KP8:
            moveCursorRelative(0, -1);
            break;
        case SDLK_DOWN:
        case SDLK_KP2:
            moveCursorRelative(0, 1);
            break;
        case SDLK_LEFT:
        case SDLK_KP4:
            moveCursorRelative(-1, 0);
            break;
        case SDLK_RIGHT:
        case SDLK_KP6:
            moveCursorRelative(1, 0);
            break;
        case SDLK_SPACE:
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            select_objAtCursor();
            break;
        case SDLK_TAB :
        	if (is_party_member) // when in pickpocket mode we don't want to allow tabing to map window.
        	{
        		set_show_cursor(false);
        		return GUI_PASS;
        	}
        	break;
/*        case SDLK_F1: // FIXME: these should pass down to Event (global actions)
        case SDLK_F2:
        case SDLK_F3:
        case SDLK_F4:
        case SDLK_F5:
        case SDLK_F6:
        case SDLK_F7:
        case SDLK_F8:
            if(view_manager->get_inventory_view()->set_party_member(key.sym - 282))
            {
                view_manager->set_inventory_mode();
                moveCursorToInventory(0, 0);
            }
            break;
        case SDLK_F10:
            view_manager->set_party_mode();
            set_show_cursor(false);
            break;
        case SDLK_ESCAPE:
        default:
            event->cancelAction();
            break;*/
        default:
            set_show_cursor(false); // newAction() can move cursor here
            return GUI_PASS;
    }
    return(GUI_YUM);
}


/* Put cursor over one of the readied-item slots. */
void InventoryView::moveCursorToSlot(uint8 slot_num)
{
    cursor_pos.area = INVAREA_DOLL;
    cursor_pos.x = slot_num;
}

/* Put cursor over one of the visible inventory slots. (column inv_x, row inv_y) */
void InventoryView::moveCursorToInventory(uint8 inv_x, uint8 inv_y)
{
    cursor_pos.area = INVAREA_LIST;
    cursor_pos.x = inv_x;
    cursor_pos.y = inv_y;
}

/* Put cursor over one of the command icons. */
void InventoryView::moveCursorToButton(uint8 button_num)
{
    cursor_pos.area = INVAREA_COMMAND;
    cursor_pos.x = button_num;
}

/* Put cursor over the container or actor icon above the inventory widget. */
void InventoryView::moveCursorToTop()
{
    cursor_pos.area = INVAREA_TOP;
}

/* Put cursor over the next slot or icon in relative direction new_x, new_y. */
void InventoryView::moveCursorRelative(sint8 new_x, sint8 new_y)
{
    uint32 x = cursor_pos.x, y = cursor_pos.y;
    if(cursor_pos.area == INVAREA_LIST)
    {
        if(x == 0 && new_x < 0)
        {
            if(y == 0)
                moveCursorToSlot(2);
            else if(y == 1)
                moveCursorToSlot(4);
            else if(y == 2)
                moveCursorToSlot(6);
        }
        else if(y == 0 && new_y < 0)
        {
            if(inventory_widget->up_arrow()) // scroll up
                update_display = true;
            else
                moveCursorToTop(); // move to container icon
        }
        else if(y == 2 && new_y > 0)
        {
            if(inventory_widget->down_arrow()) // scroll down
                update_display = true;
            else
                moveCursorToButton((x == 0) ? 3 : 4); // move to command icon
        }
        else if((x + new_x) <= 3)
            moveCursorToInventory(x + new_x, y + new_y);
    }
    else if(cursor_pos.area == INVAREA_DOLL)
    {
        // moves from these readied items can jump to inventory list
        if(new_x > 0 && x == 2)
            moveCursorToInventory(0, 0);
        else if(new_x > 0 && x == 4)
            moveCursorToInventory(0, 1);
        else if(new_x > 0 && x == 6)
            moveCursorToInventory(0, 2);
        // moves from these readied items can jump to command icons
        else if(new_y > 0 && x == 5)
            moveCursorToButton(0);
        else if(new_y > 0 && x == 6)
            moveCursorToButton(2);
        else if(new_y > 0 && x == 7)
            moveCursorToButton(1);
        // the rest move between readied items
        else if(x == 0)
            moveCursorToSlot((new_x < 0) ? 1
                             : (new_x > 0) ? 2
                             : (new_y > 0) ? 7 : 0);
        else if(x == 7)
            moveCursorToSlot((new_x < 0) ? 5
                             : (new_x > 0) ? 6
                             : (new_y < 0) ? 0 : 7);
        else if(x == 1)
            moveCursorToSlot((new_x > 0) ? 0
                             : (new_y > 0) ? 3 : 1);
        else if(x == 3)
            moveCursorToSlot((new_x > 0) ? 4
                             : (new_y < 0) ? 1
                             : (new_y > 0) ? 5 : 3);
        else if(x == 5)
            moveCursorToSlot((new_x > 0) ? 7
                             : (new_y < 0) ? 3 : 5);
        else if(x == 2)
            moveCursorToSlot((new_x < 0) ? 0
                             : (new_y > 0) ? 4 : 2);
        else if(x == 4)
            moveCursorToSlot((new_x < 0) ? 3
                             : (new_y < 0) ? 2
                             : (new_y > 0) ? 6 : 4);
        else if(x == 6)
            moveCursorToSlot((new_x < 0) ? 7
                             : (new_y < 0) ? 4 : 6);
    }
    else if(cursor_pos.area == INVAREA_COMMAND)
    {
        if(new_y < 0)
        {
            if(x == 0)
                moveCursorToSlot(5);
            else if(x == 1)
                moveCursorToSlot(7);
            else if(x == 2)
                moveCursorToSlot(6);
            else if(x == 3)
                moveCursorToInventory(0, 2);
            else if(x == 4)
                moveCursorToInventory(1, 2);
        }
        else if((x + new_x) >= 0 && (x + new_x) <= 4)
            moveCursorToButton(x + new_x);
        update_display = true;
    }
    else if(cursor_pos.area == INVAREA_TOP)
        if(new_y > 0)
        {
            moveCursorToInventory(cursor_pos.x, 0);
            update_display = true;
        }
    update_cursor();
}


/* Update on-screen location (px,py) of cursor.
 */
void InventoryView::update_cursor()
{
    SDL_Rect *ready_loc;

    switch(cursor_pos.area)
    {
        case INVAREA_LIST:
            cursor_pos.px = (4 * 16 + 8) + cursor_pos.x * 16;
            cursor_pos.py = 16 + 8 + cursor_pos.y * 16;
            cursor_pos.px += area.x;
            cursor_pos.py += area.y;
            break;
        case INVAREA_TOP:
            cursor_pos.px = 32 + inventory_widget->area.x;
            cursor_pos.py = 0 + inventory_widget->area.y;
            break;
        case INVAREA_DOLL:
            ready_loc = doll_widget->get_item_hit_rect(cursor_pos.x);
            cursor_pos.px = ready_loc->x + doll_widget->area.x;
            cursor_pos.py = ready_loc->y + doll_widget->area.y;
            break;
        case INVAREA_COMMAND:
            cursor_pos.px = ((cursor_pos.x + 1) * 16) - 16;
            cursor_pos.py = 80;
            cursor_pos.px += area.x;
            cursor_pos.py += area.y;
            break;
    }
}

void InventoryView::set_show_cursor(bool state)
{
    
    show_cursor = state;
    update_display = true;
    if(state == true)
        Game::get_game()->get_view_manager()->set_inventory_mode();
    
}

void InventoryView::hide_buttons()
{
	if(left_button) left_button->Hide();
	if(right_button) right_button->Hide();
//	if(actor_button) actor_button->Hide();
	if(party_button) party_button->Hide();
	if(combat_button) combat_button->Hide();
}

void InventoryView::show_buttons()
{
//	if(left_button) left_button->Show(); //   these two shouldn't be needed
//	if(right_button) right_button->Show(); // and cause problems
	if(actor_button) actor_button->Show();
	if(party_button) party_button->Show();
	if(combat_button) combat_button->Show();
}

/* Returns pointer to object at cursor position, or NULL.
 */
Obj *InventoryView::get_objAtCursor()
{
   // emulate mouse; use center of cursor
    uint32 hit_x = cursor_pos.px + 8 - inventory_widget->area.x,
           hit_y = cursor_pos.py + 8 - inventory_widget->area.y;
    if(cursor_pos.area == INVAREA_LIST)
        return(inventory_widget->get_obj_at_location(hit_x, hit_y));
    else if(cursor_pos.area == INVAREA_DOLL)
        return(inventory_widget->get_actor()->inventory_get_readied_object(cursor_pos.x));

    return(NULL);
}


/* Do an action with the object under the cursor, or call the function for a
   selected button. This is called when pressing ENTER. */
void InventoryView::select_objAtCursor()
{
    //Event *event = Game::get_game()->get_event();
    ViewManager *view_manager = Game::get_game()->get_view_manager();
    Obj *obj = get_objAtCursor();

    if(is_party_member)
    {
    	// special areas
    	if(cursor_pos.area == INVAREA_COMMAND)
    	{
    		if(cursor_pos.x == 0) // left
    			View::callback(BUTTON_CB, left_button, view_manager);
    		if(cursor_pos.x == 1) // party
    			View::callback(BUTTON_CB, party_button, view_manager);
    		if(cursor_pos.x == 2) // status
    			View::callback(BUTTON_CB, actor_button, view_manager);
    		if(cursor_pos.x == 3) // right
    			View::callback(BUTTON_CB, right_button, view_manager);
    		if(cursor_pos.x == 4) // strategy
    			callback(BUTTON_CB, combat_button, view_manager);
    		return;
    	}
    	else if(cursor_pos.area == INVAREA_TOP)
    	{
    		if(inventory_widget->is_showing_container())
    			inventory_widget->set_prev_container();
    		else
    			Game::get_game()->get_view_manager()->set_party_mode();

    		return;
    	}
    }

    if(cursor_pos.area == INVAREA_DOLL || cursor_pos.area == INVAREA_LIST)
    	select_obj(obj); // do action with an object
}


/* Ready an object or pass it to Event. Pass NULL if an empty space is selected.
 * Returns true if the object was "used". The caller is free to handle the
 * object if false is returned.
 */
bool InventoryView::select_obj(Obj *obj)
{
    Event *event = Game::get_game()->get_event();

    switch(event->get_mode())
    {
        case MOVE_MODE:
        case EQUIP_MODE:
            if(obj && Game::get_game()->get_usecode()->is_container(obj) && !Game::get_game()->get_usecode()->is_chest(obj))
                inventory_widget->set_container(obj);
            else if(obj)
            {
                if(obj->is_readied())
                    return event->unready(obj);
                else
                    return event->ready(obj);
            }
            break;
        default:
            event->select_obj(obj, inventory_widget->get_actor());
            return true;
    }
    return false;
}


/* Messages from child widgets, Inventory & Doll.
 **INVSELECT is called when an object is selected with MouseDown.*
 **BUTTON is called when one of the command buttons is selected.*
 * Returns GUI_PASS if the data was not used.
 */
GUI_status InventoryView::callback(uint16 msg, GUI_CallBack *caller, void *data)
{
    if(msg != INVSELECT_CB) // hit one of the command buttons
    {
        if(caller == combat_button)
        {
            if(cur_party_member != 0) // You can't change combat modes for the avatar.
            {
                Actor *actor = party->get_actor(cur_party_member);
                uint8 combat_mode = actor->get_combat_mode();
                combat_mode++;
                if(combat_mode > last_combat_mode)
                    combat_mode = first_combat_mode;
                actor->set_combat_mode(combat_mode);
                update_display = true;
            }

            return GUI_YUM;
        }
        else
            return View::callback(msg, caller, data);
    }

    // selecting an object from InventoryWidget only works while getting input
    Event *event = Game::get_game()->get_event();
    if(event->get_mode() == INPUT_MODE)
    {
        if(select_obj((Obj *)data))
            return GUI_YUM;
    }
    return GUI_PASS;
}

