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
#include "GUI_button.h"

#include "InventoryView.h"

static const char combat_mode_tbl[][8] = {"COMMAND", " FRONT", "  REAR", " FLANK", "BERSERK", "RETREAT", "ASSAULT"};

extern GUI_status partyViewButtonCallback(void *data);
extern GUI_status actorViewButtonCallback(void *data);

static GUI_status combatButtonCallback(void *data);

InventoryView::InventoryView(Configuration *cfg) : View(cfg),
   doll_widget(NULL), inventory_widget(NULL)
{

}

InventoryView::~InventoryView()
{
}

bool InventoryView::set_party_member(uint8 party_member)
{
 if(View::set_party_member(party_member))
  {
   if(doll_widget)
     doll_widget->set_actor(party->get_actor(cur_party_member));
   if(inventory_widget)
     inventory_widget->set_actor(party->get_actor(cur_party_member));

   return true;
  }

 return false;
}

bool InventoryView::init(Screen *tmp_screen, void *view_manager, uint16 x, uint16 y, Text *t, Party *p, TileManager *tm, ObjManager *om)
{
 View::init(x,y,t,p,tm,om);
 
 doll_widget = new DollWidget(config);
 doll_widget->init(party->get_actor(cur_party_member), 0, 8, tile_manager, obj_manager);
 
 AddWidget(doll_widget);
 
 inventory_widget = new InventoryWidget(config);
 inventory_widget->init(party->get_actor(cur_party_member), 64, 8, tile_manager, obj_manager);
 
 AddWidget(inventory_widget);

 add_command_icons(tmp_screen, view_manager);
 
 return true;
}



void InventoryView::Display(bool full_redraw)
{
 if(full_redraw || update_display)
   { 
    screen->fill(0x31, area.x, area.y, area.w, area.h);

    display_name();
    //display_command_icons();
    display_inventory_weights();
    display_combat_mode();
    display_actor_icon();
   }

 //display_doll(area.x,area.y+8);
 //display_inventory_list();

 DisplayChildren(full_redraw);
 
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

void InventoryView::display_name()
{
 char *name;
 
 name = party->get_actor_name(cur_party_member);
 
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
       screen->blit((area.x+4*16+8)+j*16,area.y+16+8+i*16,tile->data,8,16,16,16,true);
      }
   }
}

void InventoryView::add_command_icons(Screen *tmp_screen, void *view_manager)
{
 Tile *tile;
 SDL_Surface *button_image;
 SDL_Surface *button_image2;
 GUI_Widget *button;
 
 //FIX need to handle clicked button image, check image free on destruct.
 
 tile = tile_manager->get_tile(387); //left arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(this, 0, 80, button_image, button_image2, viewLeftButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(384); //party view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(view_manager, 16, 80, button_image, button_image2, partyViewButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(385); //actor view icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(view_manager, 2*16, 80, button_image, button_image2, actorViewButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(388); //right arrow icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(this, 3*16, 80, button_image, button_image2, viewRightButtonCallback);
 this->AddWidget(button);
 
 tile = tile_manager->get_tile(391); //combat icon
 button_image = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button_image2 = tmp_screen->create_sdl_surface_from(tile->data, 8, 16, 16, 16);
 button = new GUI_Button(this, 4*16, 80, button_image, button_image2, combatButtonCallback);
 this->AddWidget(button);
 
}

void InventoryView::display_inventory_weights()
{
 uint8 strength;
 float inv_weight;
 float equip_weight;
 Actor *actor = party->get_actor(cur_party_member);
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
 text->drawString(screen, combat_mode_tbl[actor->get_combat_mode() - 2], area.x+5*16, area.y+88, 0);
}

void InventoryView::display_actor_icon()
{
 Actor *actor = party->get_actor(cur_party_member);
 Tile *actor_tile;
  
 actor_tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(actor->get_tile_num())+9); //FIX here for sherry
 
 screen->blit(area.x+6*16,area.y+8,actor_tile->data,8,16,16,16,true);
}

static GUI_status combatButtonCallback(void *data)
{
 InventoryView *view;
 
 view = (InventoryView *)data;
 
 //view->set_next_combat_mode();
 printf("Set party member combat mode here!\n");
  
 return GUI_YUM;
}



