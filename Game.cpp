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
#include "U6misc.h"

#include "GUI.h"
#include "Console.h"
#include "Dither.h"

#include "SoundManager.h"

#include "Actor.h"
#include "Script.h"
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
#include "FontManager.h"
#include "ViewManager.h"
#include "EffectManager.h"

#include "MsgScroll.h"
#include "Map.h"
#include "MapWindow.h"
#include "Event.h"
#include "Portrait.h"
#include "Background.h"
#include "CommandBar.h"
#include "PartyView.h"
#include "ActorView.h"

#include "U6UseCode.h"
#include "MDUseCode.h"
#include "SEUseCode.h"

#include "Cursor.h"
#include "SaveManager.h"
#include "Weather.h"

#include "Game.h"

Game *Game::game = NULL;

Game::Game(Configuration *cfg, Script *s, GUI *g)
{
 game = this;
 config = cfg;
 script = s;
 gui = g;
 cursor = NULL;
 dither = NULL;
 tile_manager = NULL;
 obj_manager = NULL;
 palette = NULL;
 text = NULL;
 font_manager = NULL;
 game_map = NULL;
 actor_manager = NULL;
 player = NULL;
 event = NULL;
 converse = NULL;
 clock = NULL;
 party = NULL;
 portrait = NULL;
 view_manager = NULL;
 sound_manager = NULL;
 usecode = NULL;
 effect_manager = NULL;
 weather = NULL;
 
 pause_flags = PAUSE_UNPAUSED;
 ignore_event_delay = 0;
}

Game::~Game()
{
    // note: don't delete objects that are added to the GUI object via
    // AddWidget()!
    if(dither) delete dither;
    if(tile_manager) delete tile_manager;
    if(obj_manager) delete obj_manager;
    if(palette) delete palette;
    if(text) delete text;
    if(font_manager) delete font_manager;
    //delete scroll;
    if(game_map) delete game_map;
    if(actor_manager) delete actor_manager;
    //delete map_window;
    if(player) delete player;
    if(event) delete event;
    //delete background;
    if(converse) delete converse;
    if(clock) delete clock;
    if(party) delete party;
    if(portrait) delete portrait;
    if(view_manager) delete view_manager;
    if(sound_manager) delete sound_manager;
    if(gui) delete gui;
    if(usecode) delete usecode;
    if(effect_manager) delete effect_manager;
    if(save_manager) delete save_manager;
    if(cursor) delete cursor;
    if(egg_manager) delete egg_manager;
    if(weather) delete weather;
}

bool Game::loadGame(Screen *s, nuvie_game_t type)
{
 screen = s;
 game_type = type;


   dither = new Dither(config);
   sound_manager = new SoundManager();
   sound_manager->nuvieStartup(config);
   //sound_manager->LoadSongs(NULL);
   //sound_manager->LoadObjectSamples(NULL);

   save_manager = new SaveManager(config);
   if(save_manager->init() == false)
	   return false;

   palette = new GamePalette(screen,config);

   clock = new GameClock(config);

   background = new Background(config);
   background->init();
   background->Hide();
   gui->AddWidget(background);

   text = new Text(config);
   text->loadFont();

   font_manager = new FontManager(config);
   font_manager->init();

   scroll = new MsgScroll(config, font_manager->get_font(0));

   game_map = new Map(config);

   egg_manager = new EggManager(config, game_type, game_map);

   tile_manager = new TileManager(config);
   if(tile_manager->loadTiles() == false)
	   return false;

   ConsoleAddInfo("Loading ObjManager()");
   obj_manager = new ObjManager(config, tile_manager, egg_manager);

   // Correct usecode class for each game
   switch (game_type)
     {
      case NUVIE_GAME_U6 : usecode = (UseCode *) new U6UseCode(this, config); break;
      case NUVIE_GAME_MD : usecode = (UseCode *) new MDUseCode(this, config); break;
      case NUVIE_GAME_SE : usecode = (UseCode *) new SEUseCode(this, config); break;
     }

   obj_manager->set_usecode(usecode);
   //obj_manager->loadObjs();

   ConsoleAddInfo("Loading map data.");
   game_map->loadMap(tile_manager, obj_manager);
   egg_manager->set_obj_manager(obj_manager);

   ConsoleAddInfo("Loading actor data.");
   actor_manager = new ActorManager(config, game_map, tile_manager, obj_manager, clock);

   game_map->set_actor_manager(actor_manager);
   egg_manager->set_actor_manager(actor_manager);

   map_window = new MapWindow(config);
   map_window->init(game_map, tile_manager, obj_manager, actor_manager);
   map_window->Hide();
   gui->AddWidget(map_window);

   weather = new Weather(config, clock, game_type);

   command_bar = new CommandBar(this);
   command_bar->Hide();
   gui->AddWidget(command_bar);


   player = new Player(config);
   party = new Party(config);
   player->init(obj_manager, actor_manager, map_window, clock, party);
   party->init(this, actor_manager);

   portrait = new Portrait(config);
   if(portrait->init() == false)
	   return false;

   view_manager = new ViewManager(config);
   view_manager->init(gui, text, party, player, tile_manager, obj_manager, portrait);
   scroll->Hide();
   gui->AddWidget(scroll);


   map_window->set_windowSize(11,11);

   converse = new Converse();
   converse->init(config, game_type, scroll, actor_manager, clock, player, view_manager, obj_manager);

   usecode->init(obj_manager, game_map, player, scroll);

   effect_manager = new EffectManager;

   init_cursor();

   event = new Event(config);
   event->init(obj_manager, map_window, scroll, player, clock, converse, view_manager, usecode, gui);
   
   if(save_manager->load_latest_save() == false)
   {
    return false;
   }

   ConsoleAddInfo("Polishing Anhk");

   //ConsolePause();
   ConsoleHide();

   background->Show();
   command_bar->Show();
   scroll->Show();
   map_window->Show();
   view_manager->set_party_mode();
   view_manager->update();

 return true;
}


void Game::init_cursor()
{
    cursor = new Cursor();
    if(cursor->init(config, screen))
       SDL_ShowCursor(false); // won't need the system default
    else
    {
        delete cursor;
        cursor = NULL; // no game cursor
    }
}


bool Game::set_mouse_pointer(uint8 ptr_num)
{
    return(cursor && cursor->set_pointer(ptr_num));
}

//FIXME pausing inside a script function causes problems with yield/resume logic.
void Game::set_pause_flags(GamePauseState state)
{
    pause_flags = state; // set

    // if stopped user input set event to wait mode and block GUI
    // else set event back to move mode and unblock GUI
    if(user_paused())
    {
        if(event->get_mode() != WAIT_MODE)
            event->set_mode(WAIT_MODE);
        if(!gui->get_block_input())
            gui->block();
    }
    else
    {
        if(event->get_mode() == WAIT_MODE)
            event->endAction(); // change to MOVE_MODE, hide cursors
        if(gui->get_block_input())
            gui->unblock();
    }

    // if stopped world, freeze actormanager and gameclock
    // (other classes will check the pauseflags themselves)
    if(world_paused())
    {
        if(actor_manager->get_update() == true) // ActorMgr is running
            game->get_actor_manager()->set_update(false); // pause

        if(clock->get_active() == true) // stop time
            clock->set_active(false);
    }
    else // unpaused
    {
        if(actor_manager->get_update() == false) // ActorMgr is not running
            game->get_actor_manager()->set_update(true); // resume

        if(clock->get_active() == false) // start time
            clock->set_active(true);
    }
}


void Game::dont_wait_for_interval()
{
    if(ignore_event_delay < 255)
        ++ignore_event_delay;
    event->set_ignore_timeleft(true);
}


void Game::wait_for_interval()
{
    if(ignore_event_delay > 0)
        --ignore_event_delay;
    if(ignore_event_delay == 0)
        event->set_ignore_timeleft(false);
}


void Game::time_changed()
{
    get_command_bar()->update(); // date & wind
    get_view_manager()->get_party_view()->update(); // sky
    get_map_window()->updateAmbience();
}

// FIXME: should this be in ViewManager?
void Game::stats_changed()
{
    get_view_manager()->get_actor_view()->update();
    get_view_manager()->get_party_view()->update();
}


void Game::play()
{
  bool game_play = true;
  pause_flags = PAUSE_UNPAUSED;

  //view_manager->set_inventory_mode(1); //FIX

  screen->update();

  //map_window->drawMap();

  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2,SDL_DEFAULT_REPEAT_INTERVAL*2);

  map_window->updateBlacking();

  for( ; game_play ; )
   {
     if(cursor) cursor->clear(); // restore cursor area before GUI events

     game_play = event->update();
     if(clock->get_timer(GAMECLOCK_TIMER_U6_TIME_STOP) == 0)
     {
       palette->rotatePalette();
       tile_manager->update();
       actor_manager->twitchActors();
     }
     actor_manager->moveActors(); // update/move actors for this turn
     map_window->update();
     //map_window->drawMap();
     converse->continue_script();
     //scroll->updateScroll();
     effect_manager->update_effects();

     gui->Display();
     if(cursor) cursor->display();

     screen->preformUpdate();
     sound_manager->update();
     event->wait();
   }
  return;
}

void Game::update_once(bool process_gui_input)
{
    if(cursor) cursor->clear(); // restore cursor area before GUI events

    event->update_timers();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      if(process_gui_input)
         gui->HandleEvent(&event);
    }

    if(clock->get_timer(GAMECLOCK_TIMER_U6_TIME_STOP) == 0)
    {
      palette->rotatePalette();
      tile_manager->update();
      actor_manager->twitchActors();
    }
    map_window->update();
    effect_manager->update_effects();
}

void Game::update_once_display()
{
    gui->Display();
    if(cursor) cursor->display();

    screen->preformUpdate();
    sound_manager->update();
    event->wait();
}
