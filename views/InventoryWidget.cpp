/*
 *  InventoryWidget.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Sep 01 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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
#include "U6LList.h"

#include "InventoryWidget.h"
#include "Actor.h"
#include "Text.h"
#include "GameClock.h"
#include "Event.h"
#include "MsgScroll.h"
#include "UseCode.h"

#include "InventoryFont.h"

static SDL_Rect arrow_rects[2] = {{0,16,8,8},{0,3*16+8,8,8}};

InventoryWidget::InventoryWidget(Configuration *cfg): GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 container_obj = NULL;
 selected_obj = NULL;
 target_obj = NULL;
 ready_obj = NULL;
 row_offset = 0;
}

InventoryWidget::~InventoryWidget()
{

}

bool InventoryWidget::init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, Text *t)
{
 tile_manager = tm;
 obj_manager = om;
 text = t;

 //72 =  4 * 16 + 8
 GUI_Widget::Init(NULL, x, y, 72, 64);

 set_actor(a);

 return true;
}

void InventoryWidget::set_actor(Actor *a)
{
 actor = a;
 container_obj = NULL;
 Redraw();
}

void InventoryWidget::Display(bool full_redraw)
{
 if(full_redraw || update_display)
  {
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   display_inventory_container();
   display_arrows();
  }
   //screen->blit(area.x+40,area.y+16,portrait_data,8,56,64,56,false);
 display_inventory_list();

 if(full_redraw || update_display)
  {
   update_display = false;
   screen->update(area.x, area.y, area.w, area.h);
  }
 else
  {
   screen->update(area.x+8,area.y+16,area.w-8,area.h-16); // update only the inventory list
  }

}

//either an Actor or an object container.
void InventoryWidget::display_inventory_container()
{
 Tile *tile;

 if(!container_obj) //display actor
   tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());
 else // display container object
   tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(container_obj->obj_n)+container_obj->frame_n);

 screen->blit(area.x+32,area.y,tile->data,8,16,16,16,true);

 return;
}

void InventoryWidget::display_inventory_list()
{
 Tile *tile, *empty_tile;
 U6LList *inventory;
 U6Link *link;
 Obj *obj = NULL;
 uint16 i,j;
 uint16 skip_num;

 empty_tile = tile_manager->get_tile(410);

 if(container_obj)
   inventory = container_obj->container;
 else
   inventory = actor->get_inventory_list();

 link = inventory->start();

 //skip row_offset rows of objects.
 skip_num = row_offset * 4;
 for(i=0;link != NULL && i < skip_num; link = link->next)
   {
    obj = (Obj *)link->data;
    if((obj->status & 0x18) != 0x18)
      i++;
   }

  //clear the screen first inventory icons, 4 x 3 tiles

  screen->fill(0x31, area.x +8, area.y + 16, 16 * 4, 16 * 3);

  for(i=0;i<3;i++)
   {
    for(j=0;j<4;j++)
      {
       if(link != NULL)
         {
          obj = (Obj *)link->data;
          if((obj->status & 0x18) == 0x18) //skip any readied objects
            {
             for(;link != NULL && (obj->status & 0x18) == 0x18; link = link->next)
                obj = (Obj *)link->data;
            }
          else
            link = link->next;

          tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
          if(link == NULL)
            {
             if((obj->status & 0x18) == 0x18) //last object is readied so skip it.
                tile = empty_tile;
            }
         }
        else
          tile = empty_tile;

       //tile = tile_manager->get_tile(actor->indentory_tile());

       //draw qty string for stackable items
       if(tile != empty_tile && obj_manager->is_stackable(obj))
         display_qty_string((area.x+8)+j*16,area.y+16+i*16,obj->qty);

       screen->blit((area.x+8)+j*16,area.y+16+i*16,tile->data,8,16,16,16,true);
      }
   }
}

void InventoryWidget::display_qty_string(uint16 x, uint16 y, uint8 qty)
{
 uint8 len, i, offset;
 char buf[4];

 sprintf(buf,"%d",qty);
 len = strlen(buf);

 offset = (16 - len*4) / 2;

 for(i=0;i<len;i++)
  screen->blitbitmap(x+offset+4*i,y+11,inventory_font[buf[i]-48],3,5,0x48,0x31);

 return;
}

void InventoryWidget::display_arrows()
{
 uint32 num_objects;

 num_objects = actor->inventory_count_objects(false);

 if(num_objects <= 12) //reset row_offset if we only have one page of objects
   row_offset = 0;

 if(row_offset > 0) //display top arrow
    text->drawChar(screen, 24, area.x, area.y + 16, 0x48);

 if(num_objects - row_offset * 4 > 12) //display bottom arrow
    text->drawChar(screen, 25, area.x, area.y + 3 * 16 + 8, 0x48);
}

GUI_status InventoryWidget::MouseDown(int x, int y, int button)
{
 Event *event = Game::get_game()->get_event();
 MsgScroll *scroll = Game::get_game()->get_scroll();
 //MapWindow *map_window = Game::get_game()->get_map_window();
 x -= area.x;
 y -= area.y;

 // ABOEING
 if(actor)
   {
    Obj *obj; // FIXME: duplicating code in DollWidget
    if((obj = get_obj_at_location(x,y)) != NULL)
      {
       switch(event->get_mode())
          {
           case LOOK_MODE:
               if(event->look(obj)) // returns FALSE if prompt already displayed
                scroll->display_prompt();
               event->endAction(); // FIXME: should be done in look()
               break;
           case USE_MODE:
               event->use(obj);
               break;
           case DROP_MODE:
               event->drop_select(selected_obj);
               break;
           default:
               selected_obj = obj;
               break;
          }
       return GUI_YUM;
      }
   }
 return GUI_PASS;
}

inline uint16 InventoryWidget::get_list_position(int x, int y)
{
 uint16 list_pos;

 list_pos = ((y - 16) / 16) * 4 + (x - 8) / 16;
 list_pos += row_offset * 4;

 return list_pos;
}

Obj *InventoryWidget::get_obj_at_location(int x, int y)
{
 uint8 location;
 U6LList *inventory;
 U6Link *link;
 Obj *obj =  NULL;
 uint16 i;

 if(x >= 8 && y >= 16)
   {
    location = get_list_position(x,y); //find the postion of the object we hit in the inventory

    if(container_obj)
      inventory = container_obj->container;
    else
      inventory = actor->get_inventory_list();

    for(i=0,link = inventory->start();link != NULL && i <= location;link=link->next)
     {
      obj = (Obj *)link->data;
      if((obj->status & 0x18) != 0x18)
        i++;
     }

    if(i >= location && obj && (obj->status & 0x18) != 0x18) // don't return readied or non existent objects
      return obj;
   }

 return NULL;
}

GUI_status InventoryWidget::MouseUp(int x,int y,int button)
{
 Event *event = Game::get_game()->get_event();
 UseCode *usecode = Game::get_game()->get_usecode();

 if(button == 1)
   {
    x -= area.x;
    y -= area.y;

    if(x >= 32 && x <= 48 && // hit top icon either actor or container
       y >= 0 && y <= 16)
      {
       container_obj = NULL; //return to main Actor inventory
       Redraw();
      }

    if(HitRect(x,y,arrow_rects[0])) //up arrow hit rect
      {
       if(up_arrow())
         Redraw();
      }

    if(HitRect(x,y,arrow_rects[1])) //down arrow hit rect
      {
       if(down_arrow())
         Redraw();
      }

    // only act now if not usable, else wait for possible double-click
    if(selected_obj && !usecode->has_usecode(selected_obj))
      {
        ready_obj = NULL;
        if(selected_obj->container) // open up the container.
          {
            container_obj = selected_obj;
            Redraw();
          }
        else // attempt to ready selected object.
          {
//            actor->add_readied_object(selected_obj);
            event->ready(selected_obj);
            Redraw();
          }
      }
    else
        ready_obj = selected_obj;

   }

 selected_obj = NULL;

 return GUI_YUM;
}

bool InventoryWidget::up_arrow()
{
 if(row_offset > 0)
  {
   row_offset--;
   return true;
  }

 return false;
}

bool InventoryWidget::down_arrow()
{
 uint32 num_objects;

 num_objects = actor->inventory_count_objects(false);

 if(num_objects - row_offset * 4 > 12)
   {
    row_offset++;
    return true;
   }

 return false;
}

GUI_status InventoryWidget::MouseMotion(int x,int y,Uint8 state)
{
 Tile *tile;

 if(selected_obj && !dragging)
   {
    dragging = true;
    tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(selected_obj->obj_n)+selected_obj->frame_n);
    return gui_drag_manager->start_drag(this, GUI_DRAG_OBJ, selected_obj, tile->data, 16, 16, 8);
   }

	return GUI_PASS;
}

void InventoryWidget::drag_drop_success(int x, int y, int message, void *data)
{
 dragging = false;

 if(container_obj)
   container_obj->container->remove(selected_obj);
 else
   actor->inventory_remove_obj(selected_obj, container_obj);

 selected_obj = NULL;
 Redraw();
}

void InventoryWidget::drag_drop_failed(int x, int y, int message, void *data)
{
 printf("Drop Failed\n");
 dragging = false;
 selected_obj = NULL;
}

bool InventoryWidget::drag_accept_drop(int x, int y, int message, void *data)
{
 if(message == GUI_DRAG_OBJ)
   {
    x -= area.x;
    y -= area.y;

    if(target_obj == NULL) //we need to check this so we don't screw up target_obj on subsequent calls
      target_obj = get_obj_at_location(x,y);
    return true;
   }

 return false;
}

void InventoryWidget::drag_perform_drop(int x, int y, int message, void *data)
{
 Obj *obj;

 x -= area.x;
 y -= area.y;

 if(message == GUI_DRAG_OBJ)
   {
    printf("Drop into inventory\n");
    obj = (Obj *)data;

    if(target_obj && target_obj->container && target_obj != obj)
      {
       container_obj = target_obj; //swap to container ready to drop item inside
      }

#if 0 /* trying to connect dragndrop to Actions, but it cant be done yet */
    if((obj->status & OBJ_STATUS_READIED) == OBJ_STATUS_READIED) // unready
      {
       assert(obj->x == actor->get_actor_num());
       event->unready(obj);
      }
    else if(!(obj->status & OBJ_STATUS_IN_INVENTORY)) // get
      {
       scroll->display_string("Get-");
       event->get(obj, container_obj, actor);
      }
#endif
    if(!container_obj)
     actor->inventory_add_object(obj);
    else
     container_obj->container->addAtPos(0,obj);
    Redraw();
   }

 target_obj = NULL;

 return;
}

void InventoryWidget::drag_draw(int x, int y, int message, void* data)
{
	Tile* tile;

	if (!selected_obj)
		return;

	tile = tile_manager->get_tile(obj_manager->get_obj_tile_num (selected_obj->obj_n) + selected_obj->frame_n);

	int	nx = x - 8;
	int	ny = y - 8;

	if (nx + 16 >= 320)
		nx = 303;
	else if (nx < 0)
		nx = 0;

	if (ny + 16 >= 200)
		ny = 183;
	else if (ny < 0)
		ny = 0;

	screen->blit(nx, ny, tile->data, 8, 16, 16, 16, true);
	screen->update(nx, ny, 16, 16);
}


/* Use object.
 */
GUI_status InventoryWidget::MouseDouble(int x, int y, int button)
{
    MsgScroll *scroll = Game::get_game()->get_scroll();
    Event *event = Game::get_game()->get_event();
    Obj *obj = ready_obj;
    ready_obj = NULL;

    if(!(actor && obj && button == 1 && event->get_mode() == MOVE_MODE))
        return(GUI_YUM);

    scroll->display_string("Use-");

    if(event->use(obj))
    {
        scroll->display_string("\n");
        scroll->display_prompt();
    }
    return(GUI_PASS);
}


// waited for double-click
GUI_status InventoryWidget::MouseClick(int x, int y, int button)
{
 Event *event = Game::get_game()->get_event();

 if(button != 1)
   return GUI_YUM;

 // change to or from a container, ready/unready object
 if(ready_obj && ready_obj->container) // open up the container.
   {
    container_obj = ready_obj;
    Redraw();
   }
 else if(ready_obj) // attempt to ready selected object.
   {
//     actor->add_readied_object(selected_obj);
     event->ready(ready_obj);
     Redraw();
   }

 ready_obj = NULL;

 return GUI_YUM;
}

