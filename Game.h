#ifndef __Game_h__
#define __Game_h__

/*
 *  Game.h
 *  Nuive
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
#include <SDL.h>

#include "Configuration.h"

#include "U6Shape.h"

#include "Screen.h"
#include "GamePalette.h"
#include "GameClock.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "ActorManager.h"
#include "Player.h"
#include "Party.h"
#include "Converse.h"
#include "ViewManager.h"

#include "MsgScroll.h"
#include "Map.h"
#include "MapWindow.h"
#include "Event.h"
#include "Portrait.h"
#include "UseCode.h"

class Game
{
 uint8 game_type;
 
 Configuration *config;
 Screen *screen;
 U6Shape *background;
 GamePalette *palette;
 Text *text;
 TileManager *tile_manager;
 ObjManager *obj_manager;
 ActorManager *actor_manager;
 Map *game_map;
 MapWindow *map_window;
 MsgScroll *scroll;
 Player *player;
 Party *party;
 Converse *converse;
 
 ViewManager *view_manager;
 
 GameClock *clock;
 Portrait *portrait;
 UseCode *usecode;
 
 Event *event;
 
 public:
 
 Game(Configuration *cfg);
 ~Game();
 
 bool loadGame(Screen *screen, uint8 type);
 
 bool loadBackground();
 void drawBackground();
 
 void play();

 /* Pass back instance of Game classes... and why not? */
 Configuration *get_config()       { return(config); }
 Screen *get_screen()              { return(screen); }
 U6Shape *get_background()         { return(background); }
 GamePalette *get_palette()        { return(palette); }
 Text *get_text()                  { return(text); }
 TileManager *get_tile_manager()   { return(tile_manager); }
 ObjManager *get_obj_manager()     { return(obj_manager); }
 ActorManager *get_actor_manager() { return(actor_manager); }
 Map *get_game_map()               { return(game_map); }
 MapWindow *get_map_window()       { return(map_window); }
 MsgScroll *get_scroll()           { return(scroll); }
 Player *get_player()              { return(player); }
 Party *get_party()                { return(party); }
 Converse *get_converse()          { return(converse); } 
 ViewManager *get_view_manager()   { return(view_manager); }
 GameClock *get_clock()            { return(clock); }
 Portrait *get_portrait()          { return(portrait); }
 UseCode *get_usecode()            { return(usecode); }
 Event *get_event()                { return(event); }

 protected:
 
};

#endif /* __Game_h__ */

