#ifndef __Event_h__
#define __Event_h__

/*
 *  Event.h
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Mar 26 2003.
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
#include <list>

#include "SDL.h"

#include "ObjManager.h"
#include "GUI_CallBack.h"

class CallbackTarget;
class Configuration;
class Book;
class Game;
class TimeQueue;
class MapWindow;
class MsgScroll;
class GameClock;
class Player;
class Converse;
class ViewManager;
class UseCode;
class GUI;

#define NUVIE_INTERVAL    65

typedef enum {
 LOOK_MODE,
 USE_MODE,
 CAST_MODE,
 GET_MODE,
 MOVE_MODE,
 DROP_MODE,
 TALK_MODE, /* finding an actor to talk to */
 PUSHSELECT_MODE,
 EQUIP_MODE,
 USESELECT_MODE,
 FREESELECT_MODE, /*...or use a single free-move mode for get,talk,select?*/
 PUSH_MODE,
 DROPTARGET_MODE,
 DROPCOUNT_MODE, /* select how many objects to drop */
 WAIT_MODE /* waiting for something, optionally display prompt when finished */
} EventMode;

extern uint32 nuvieGameCounter;


class Event : public GUI_CallBack
{
 Configuration *config;
 GUI *gui;
 ObjManager *obj_manager;
 MapWindow *map_window;
 MsgScroll *scroll;
 GameClock *clock;
 Player *player;
 EventMode mode;
 Book *book;
 Converse *converse;
 ViewManager *view_manager;
 UseCode *usecode;

 SDL_Event event;
 int ts; //timestamp for TimeLeft() method.
 char alt_code_str[4]; // string representation of alt-code input
 uint8 alt_code_len; // how many characters have been input for alt-code
 uint16 active_alt_code; // alt-code that needs more input
 uint8 alt_code_input_num; // alt-code can get multiple inputs

 TimeQueue *time_queue, *game_time_queue;
 Obj *use_obj; // used for many things right now (use/move/drop)
 Actor *selected_actor; // for PUSHSELECT
 uint16 drop_qty;

 bool showingQuitDialog;
 bool ignore_timeleft; // do not wait for NUVIE_INTERVAL

 public:
 Event(Configuration *cfg);
 virtual ~Event();

 bool init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p,
           GameClock *gc, Converse *c, ViewManager *vm, UseCode *uc, GUI *g);
 Book *get_book() { return(book); }
 TimeQueue *get_time_queue() { return(time_queue); }
 TimeQueue *get_game_time_queue() { return(game_time_queue); }
 EventMode get_mode() { return(mode); }
 void set_mode(EventMode new_mode) { mode = new_mode; }

 bool update();
 bool handleEvent(const SDL_Event *event);
 void useselect_mode(Obj *src, const char *prompt = NULL);
 void freeselect_mode(Obj *src, const char *prompt = NULL);
 void get_scroll_input(const char *allowed = NULL, bool can_escape = true);
 void display_portrait(Actor *actor, const char *name = NULL);
 bool newAction(EventMode new_mode);
 void doAction(sint16 rel_x = 0, sint16 rel_y = 0);
 void doAction(Obj *obj);
 void cancelAction();
 void endAction(bool prompt = false);

 /* The upper-level calls (look/talk/use) encapsulate everything about the
    action, except printing the action name ("Talk-"). The lower-level methods
    (perform_look/perform_talk/perform_use) only contain the core functionality
    for any type of target. (object, action, or location) */
 // FIXME: but barely anything follows this scheme yet

 bool move(sint16 rel_x, sint16 rel_y);
 bool use(sint16 rel_x, sint16 rel_y);
 bool use(Obj *obj);
 bool use(Actor *actor);
 bool get(sint16 rel_x, sint16 rel_y);
 bool get(Obj *obj, Obj *container_obj = NULL, Actor *actor = NULL);
 bool look();
 bool look(Obj *obj);
 bool look(Actor *actor);
 bool search(Obj *obj);
 bool talk();
 bool talk(Actor *actor);
 bool talk(Obj *obj);
 bool perform_talk(Actor *actor);
 bool pushFrom(sint16 rel_x, sint16 rel_y);
 bool pushTo(sint16 rel_x, sint16 rel_y);
 bool select_obj(Obj *obj = NULL, Actor *actor = NULL);
 bool select_obj(sint16 rel_x, sint16 rel_y);
 void solo_mode(uint32 actor_num);
 void party_mode();
 bool ready(Obj *obj);
 bool unready(Obj *obj);
 bool drop_select(Obj *obj, uint16 qty = 0);
 bool drop_count(uint16 qty);
 bool drop();
 bool drop(Obj *obj, uint16 qty, uint16 x, uint16 y);
 bool drop(uint16 x, uint16 y) { return(drop(use_obj, drop_qty, x, y)); }
 void walk_to_mouse_cursor(uint32 mx, uint32 my);
 void multiuse(uint16 wx, uint16 wy);

 void alt_code(const char *cs);
 void alt_code_input(const char *in);
 void clear_alt_code() { alt_code_str[0] = '\0'; alt_code_len = 0; }
 bool alt_code_teleport(const char *location_string);
 void alt_code_infostring();
 void alt_code_teleport_menu(uint32 selection);
 bool alt_code_teleport_to_person(uint32 npc);

 void wait();
 void set_ignore_timeleft(bool newsetting) { ignore_timeleft = newsetting; }

/* FIXME: Some of the above (action) functions can be removed from public, so
   that we don't need to check for WAIT mode in all of them. */
 protected:

 inline Uint32 TimeLeft();

 void quitDialog();
 void saveDialog();
 GUI_status callback(uint16 msg, GUI_CallBack *caller, void *data);
 bool handleSDL_KEYDOWN (const SDL_Event *event);

};


#endif /* __Event_h__ */
