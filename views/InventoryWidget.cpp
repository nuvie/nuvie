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

#include "InventoryWidget.h"

InventoryWidget::InventoryWidget(Configuration *cfg): GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 container_obj = NULL;
 selected_obj = NULL;
}

InventoryWidget::~InventoryWidget()
{

}

bool InventoryWidget::init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om)
{
 tile_manager = tm;
 obj_manager = om;
 
 area.x = x;
 area.y = y;
 
 area.w = 72; // 4 * 16 + 8
 area.h = 64;
 
 set_actor(a);
 
 return true;
}

void InventoryWidget::set_actor(Actor *a)
{
 actor = a;
 Redraw();
}

void InventoryWidget::Display(bool full_redraw)
{
// if(full_redraw || update_display)
//  {
   update_display = false;
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   //screen->blit(area.x+40,area.y+16,portrait_data,8,56,64,56,false);
   display_inventory_container();
   display_inventory_list();
   screen->update(area.x, area.y, area.w, area.h);
//  }
 
}

//either an Actor or an object container.
void InventoryWidget::display_inventory_container()
{
 Tile *tile;

 if(!container_obj)
   tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->get_tile_num())+9); //FIX here for sherry
 else
   tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(container_obj->obj_n)+container_obj->frame_n);  
  
 screen->blit(area.x+32,area.y,tile->data,8,16,16,16,true);
 
 return;
}

void InventoryWidget::display_inventory_list()
{
 Tile *tile, *empty_tile;
 U6LList *inventory;
 U6Link *link;
 Obj *obj;
 uint16 i,j;
 
 empty_tile = tile_manager->get_tile(410);
 
 if(container_obj)
   inventory = container_obj->container;
 else
   inventory = actor->get_inventory_list();
 
 link = inventory->start();
 
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
       screen->blit((area.x+8)+j*16,area.y+16+i*16,tile->data,8,16,16,16,true);
      }
   }
}

GUI_status InventoryWidget::MouseDown(int x, int y, int button)
{ 
 x -= area.x;
 y -= area.y;
 
 if(actor)
   {
    if((selected_obj = get_obj_at_location(x,y)) != NULL)
       return GUI_YUM;
   }

	return GUI_PASS;
}

inline Obj *InventoryWidget::get_obj_at_location(int x, int y)
{
 uint8 location;
 U6LList *inventory;
 U6Link *link;
 Obj *obj =  NULL;
 uint16 i;

 if(x >= 8 && y >= 16)
   {
    location = ((y - 16) / 16) * 4 + (x - 8) / 16; //find the postion of the object we hit in the inventory

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

    if(i >= location && obj)
      return obj;
   }

 return NULL;
}

GUI_status InventoryWidget::MouseUp(int x,int y,int button)
{ 
 if(selected_obj && selected_obj->container) // open up the container.
   {
    container_obj = selected_obj;
    selected_obj = NULL;
    Redraw();
   }
 else
   {
    x -= area.x;
    y -= area.y;
    
    if(x >= 32 && x <= 48 && // hit top icon either actor or container
       y >= 0 && y <= 16)
      {
       container_obj = NULL; //return to main Actor inventory
      }
   }

	return GUI_YUM;
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
   actor->inventory_remove_obj(selected_obj);

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
    return true;
   }
   
 return false;
}

void InventoryWidget::drag_perform_drop(int x, int y, int message, void *data)
{
 Obj *obj, *target_obj;
 
 x -= area.x;
 y -= area.y;
 
 if(message == GUI_DRAG_OBJ)
   {
    printf("Drop into inventory");
    obj = (Obj *)data;
    target_obj = get_obj_at_location(x,y);
    if(target_obj && target_obj->container)
      {
       container_obj = target_obj; //swap to container ready to drop item inside
      }
    
    if(!container_obj)
     actor->inventory_add_object(obj);
    else
     container_obj->container->addAtPos(0,obj);
    Redraw();
   }
   
 return;
}

