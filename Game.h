#ifndef __Game_h__
#define __Game_h__

/*
 *  Game.h
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

class Configuration;
class Screen;
class Background;
class GamePalette;
class Text;
class FontManager;
class TileManager;
class ObjManager;
class ActorManager;
class Map;
class MapWindow;
class MsgScroll;
class Player;
class Party;
class Converse;
class Cursor;
class GameClock;
class ViewManager;
class Portrait;
class UseCode;
class Event;
class GUI;
class SoundManager; //^^

typedef enum
{
    PAUSE_UNPAUSED = 0x00,
    PAUSE_USER     = 0x01, /* Don't allow user-input */
    PAUSE_ANIMS    = 0x02, /* TileManager & AnimManager */
    PAUSE_WORLD    = 0x04, /* game time doesn't pass, freeze actors */
    PAUSE_ALL      = 0xFF
} GamePauseState;

class Game
{
 uint8 game_type;
 static Game *game;
 Configuration *config;
 Screen *screen;
 Background *background; 
 GamePalette *palette;
 Text *text;
 FontManager *font_manager;
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
 SoundManager *sound_manager; //^^
 
 GameClock *clock;
 Portrait *portrait;
 UseCode *usecode;

 Cursor *cursor;

 Event *event;
 
 GUI *gui;

 GamePauseState pause_flags;
 
 public:
 
 Game(Configuration *cfg);
 ~Game();
 
 bool loadGame(Screen *screen, uint8 type);
 void init_cursor();

 void play();

 GamePauseState get_pause_flags()            { return(pause_flags); }
 void set_pause_flags(GamePauseState state)  { pause_flags = state; }
 void unpause()    { pause_flags = PAUSE_UNPAUSED; }
 void pause_all()  { pause_flags = PAUSE_ALL; }

 bool set_mouse_pointer(uint8 ptr_num);

 /* Pass back instance of Game classes... and why not? */
 static Game *get_game()           { return(game); }
 Configuration *get_config()       { return(config); }
 Screen *get_screen()              { return(screen); }
// U6Shape *get_background()         { return(background); }
 GamePalette *get_palette()        { return(palette); }
 Text *get_text()                  { return(text); }
 FontManager *get_font_manager()   { return(font_manager); }
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
 GUI *get_gui()                    { return(gui); }
 SoundManager *get_sound_manager() { return(sound_manager); } //^^
 Cursor *get_cursor()              { return(cursor); }

 protected:
 
};

#endif /* __Game_h__ */

