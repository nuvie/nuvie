/*
 *  Game.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Thu Mar 13 2003.
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

#include "Screen.h"
#include "GamePalette.h"
#include "GameClock.h"
#include "EggManager.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "Player.h"
#include "Party.h"
#include "Converse.h"
#include "Text.h"
#include "ViewManager.h"

#include "MsgScroll.h"
#include "Map.h"
#include "MapWindow.h"
#include "Event.h"
#include "Portrait.h"
#include "Background.h"

#include "U6UseCode.h"
#include "MDUseCode.h"
#include "SEUseCode.h"

#include "Game.h"

Game *Game::game = NULL;

Game::Game(Configuration *cfg)
{
 game = this;
 config = cfg;
}

Game::~Game()
{
    // note: don't delete objects that are added to the GUI object via
    // AddWidget()!
    delete tile_manager;
    delete obj_manager;
    delete palette;
    delete text;
    //delete scroll;
    delete game_map;
    delete actor_manager;
    //delete map_window;
    delete player;
    delete event;
    //delete background;
    delete converse;
    delete clock;
    delete party;
    delete portrait;
    delete view_manager;
    delete gui;
    delete usecode;
}
 
bool Game::loadGame(Screen *s, uint8 type)
{
 EggManager *egg_manager;
 screen = s;
 game_type = type;
 
 gui = new GUI(screen);

 try
  {
   palette = new GamePalette(screen,config);

   clock = new GameClock(config);
   clock->init();
   
   background = new Background(config);
   background->init();
   gui->AddWidget(background);
   
   text = new Text(config);
   text->loadFont();

   game_map = new Map(config);

   egg_manager = new EggManager(config, game_map);
         
   tile_manager = new TileManager(config);
   tile_manager->loadTiles();

   obj_manager = new ObjManager(config, egg_manager);
   obj_manager->loadObjs(tile_manager);

   game_map->loadMap(tile_manager, obj_manager);
   egg_manager->set_obj_manager(obj_manager);
   
   actor_manager = new ActorManager(config, game_map, tile_manager, obj_manager, clock);
   actor_manager->loadActors();

   game_map->set_actor_manager(actor_manager);
   egg_manager->set_actor_manager(actor_manager);
   
   map_window = new MapWindow(config);
   map_window->init(game_map, tile_manager, obj_manager, actor_manager);
   gui->AddWidget(map_window);
   
   player = new Player(config);
   party = new Party(config);
   player->init(obj_manager, actor_manager, map_window, clock, party);
   party->init(this, actor_manager); // (reverse Game reference test-case)
 
   portrait = new Portrait(config);
   portrait->init();
 
   view_manager = new ViewManager(config);
   view_manager->init(gui, text, party, player, tile_manager, obj_manager, portrait);
 
   scroll = new MsgScroll(config);
   scroll->init(text, player->get_name());
   gui->AddWidget(scroll);

   map_window->set_windowSize(11,11);
   //map_window->move(0x12e,0x16b);
   map_window->centerMapOnActor(player->get_actor());

   converse = new Converse();
   converse->init(config, game_type, scroll, actor_manager, clock, player, view_manager, obj_manager);

   // Correct usecode class for each game
   switch (game_type)
     { 
      case NUVIE_GAME_U6 : usecode = (UseCode *) new U6UseCode(config); break;
      case NUVIE_GAME_MD : usecode = (UseCode *) new MDUseCode(config); break;
      case NUVIE_GAME_SE : usecode = (UseCode *) new SEUseCode(config); break;
     }

   usecode->init(obj_manager, game_map, player, scroll);
   obj_manager->set_usecode(usecode);

 
   event = new Event(config);
   event->init(obj_manager, map_window, scroll, player, clock, converse, view_manager, usecode, gui);
  }
 catch(const char *error_string)
  {
   printf("Error: %s\n",error_string);
   return false;
  }
 
 return true;
}
  
void Game::play()
{
  bool game_play = true;

  scroll->display_prompt();
  
  //view_manager->set_inventory_mode(1); //FIX
  
  screen->update();
  
  //map_window->drawMap();
  
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

  screen->clearalphamap8( 8, 8, 160, 160, 0x00 );

  map_window->updateBlacking();
  
  for( ; game_play ; ) 
   {
     game_play = event->update();
     palette->rotatePalette();
     tile_manager->update();
     actor_manager->twitchActors();
     //map_window->drawMap();
     converse->continue_script();
     //scroll->updateScroll();

     gui->Display();

     screen->preformUpdate();
     event->wait();
   }
  return;
}


 
