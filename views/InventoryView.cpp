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

#include "InventoryView.h"

InventoryView::InventoryView(Configuration *cfg) : View(cfg)
{

}

InventoryView::~InventoryView()
{
}

bool InventoryView::init(uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);
 
 cur_actor_num = 0;
 
 return true;
}

void InventoryView::Display(bool full_redraw)
{
 if(full_redraw || update_display)
   { 
    screen->fill(0x31, area.x, area.y, area.w, area.h);

    display_name();
    display_command_icons();
    display_inventory_weights();
    display_actor_icon();
   }

 display_doll(area.x,area.y+8);
 display_inventory_list();

 if(full_redraw || update_display)
   {
    update_display = false;
    screen->update(area.x, area.y, area.w, area.h);
   }
 else
   {
    screen->update(area.x+4*16+8,area.y+16+8,4*16,3*16); // item display
    //FIX add doll update rect.
   }

 return;
}

void InventoryView::display_doll(uint16 x, uint16 y)
{
 Tile *tile;
 uint8 i,j;
 
 for(i=0;i<2;i++)
   {
    for(j=0;j<2;j++) // draw doll
      {
       tile = tile_manager->get_tile(368+i*2+j);
       screen->blit(x+16+j*16,y+16+i*16,tile->data,8,16,16,16,true);
      }
   }

 tile = tile_manager->get_tile(410);
 
 for(i=0;i<3;i++)
   {
    screen->blit(x,(y+8)+i*16,tile->data,8,16,16,16,true); //left slots
    screen->blit(x+3*16,(y+8)+i*16,tile->data,8,16,16,16,true); //right slots
   }

 screen->blit(x+16+8,y,tile->data,8,16,16,16,true); //top slot
 screen->blit(x+16+8,y+3*16,tile->data,8,16,16,16,true); //bottom slot

 return;
}

void InventoryView::display_name()
{
 char *name;
 
 name = party->get_actor_name(cur_actor_num);
 
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
 
 empty_tile = tile_manager->get_tile(410);
 
 actor = party->get_actor(cur_actor_num);
 
 inventory = actor->get_inventory_list();
 
 link = inventory->start();
 
  for(i=0;i<3;i++)
   {
    for(j=0;j<4;j++)
      {
       if(link != NULL)
         {
          obj = (Obj *)link->data;
          tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
          link = link->next;
         }
        else
          tile = empty_tile;
          
       //tile = tile_manager->get_tile(actor->indentory_tile());
       screen->blit((area.x+4*16+8)+j*16,area.y+16+8+i*16,tile->data,8,16,16,16,true);
      }
   }
}

void InventoryView::display_command_icons()
{
 Tile *tile;
 
 tile = tile_manager->get_tile(387); //left arrow icon
 screen->blit(area.x,area.y+80,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(384); //party icon
 screen->blit(area.x+16,area.y+80,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(385); //actor icon
 screen->blit(area.x+2*16,area.y+80,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(388); //actor icon
 screen->blit(area.x+3*16,area.y+80,tile->data,8,16,16,16,true);

 tile = tile_manager->get_tile(391); //battle icon
 screen->blit(area.x+4*16,area.y+80,tile->data,8,16,16,16,true);

}

void InventoryView::display_inventory_weights()
{
 uint8 strength;
 float inv_weight;
 float equip_weight;
 Actor *actor = party->get_actor(cur_actor_num);
 char string[9]; //  "E:xx/xx"
 
 strength = actor->get_strength();
 inv_weight = actor->get_inventory_weight();
 equip_weight = actor->get_inventory_equip_weight();
  
 snprintf(string,8,"E:%d/%d",(int)equip_weight,strength);
 text->drawString(screen, string, area.x, area.y+72, 0);
 
 snprintf(string,8,"I:%d/%d",(int)inv_weight,strength*2);
 text->drawString(screen, string, area.x+4*16+8, area.y+72, 0);
}

void InventoryView::display_actor_icon()
{
 Actor *actor = party->get_actor(cur_actor_num);
 Tile *actor_tile;
  
 actor_tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->get_tile_num())+9); //FIX here for sherry
 
 screen->blit(area.x+6*16,area.y+8,actor_tile->data,8,16,16,16,true);
}
