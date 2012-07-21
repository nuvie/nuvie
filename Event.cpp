/*
 *  Event.cpp
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

#include <cassert>
#include <cstring>
#include "SDL.h"

#include "nuvieDefs.h"
#include "U6misc.h"
#include "Configuration.h"
#include "Game.h"
#include "GameClock.h"
#include "MapWindow.h"
#include "Map.h"
#include "MsgScroll.h"
#include "ActorManager.h"
#include "Actor.h"
#include "Converse.h"
#include "Player.h"
#include "Party.h"
#include "Book.h"
#include "ViewManager.h"
#include "PortraitView.h"
#include "TimedEvent.h"
#include "InventoryView.h"
#include "PartyView.h"
#include "ActorView.h"
#include "CommandBar.h"
#include "U6LList.h"
#include "Event.h"
#include "U6objects.h"
#include "Effect.h"
#include "EffectManager.h"
#include "NuvieIOFile.h"

#include "UseCode.h"
#include "SaveManager.h"
#include "Magic.h"

#include "GUI.h"
#include "GUI_YesNoDialog.h"

#include "views/InventoryWidget.h"
#include "Script.h"

#include <math.h>

using std::string;

EventInput_s::~EventInput_s()
{
    if(target_init) delete target_init;
    if(str) delete str;
    if(loc) delete loc;
}

void EventInput_s::set_loc(MapCoord c)
{
    if((type == EVENTINPUT_MAPCOORD || type == EVENTINPUT_MAPCOORD_DIR) && loc != 0) delete loc;
    loc = new MapCoord(c);
}

Event::Event(Configuration *cfg)
{
 config = cfg;
 clear_alt_code();
 active_alt_code = 0;
 alt_code_input_num = 0;

 game = Game::get_game();
 gui = NULL;
 obj_manager = NULL;
 map_window = NULL;
 scroll = NULL;
 clock = NULL;
 player = NULL;
 converse = NULL;
 view_manager = NULL;
 usecode = NULL;
 magic = NULL;
 drop_obj = NULL;
 ts = 0;
 drop_qty = 0;
 drop_x = drop_y = -1;
 rest_time = 0;
 rest_guard = 0;
 push_obj = NULL;
 push_actor = NULL;
 move_in_inventory = false;
 time_queue = game_time_queue = NULL;
 showingQuitDialog = false;
 ignore_timeleft = false;
 drop_okay_to_take = true;

 mode = MOVE_MODE;
 last_mode = MOVE_MODE;
}

Event::~Event()
{
 delete time_queue;
 delete game_time_queue;
}

bool Event::init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p, Magic *mg,
                 GameClock *gc, Converse *c, ViewManager *vm, UseCode *uc, GUI *g)
{
 gui = g;
 obj_manager = om;
 map_window = mw;
 scroll = ms;
 clock = gc;
 player = p;
 converse = c;
 view_manager = vm;
 usecode = uc;

 mode = MOVE_MODE;
 last_mode = MOVE_MODE;
 input.get_direction = false;
 input.get_text = false;
 input.target_init = NULL;

 time_queue = new TimeQueue;
 game_time_queue = new TimeQueue;
 magic = mg;

 return true;
}

void Event::update_timers()
{
	 time_queue->call_timers(clock->get_ticks());
	 game_time_queue->call_timers(clock->get_game_ticks());
}

bool Event::update()
{
 bool idle = true;
 // timed
 time_queue->call_timers(clock->get_ticks());
 game_time_queue->call_timers(clock->get_game_ticks());

 // polled
 SDL_Event event;
 while(SDL_PollEvent(&event))
  {
   idle = false;
   switch(gui->HandleEvent(&event))
     {
      case GUI_PASS : if(handleEvent(&event) == false)
                         return false;
                      break;

      case GUI_QUIT : return false;

      default : break;
     }
  }

  if(idle)
    gui->Idle(); // run Idle() for all widgets

  if(showingQuitDialog) // temp. fix to show normal cursor over quit dialog
    game->set_mouse_pointer(0);

 return true;
}

bool Event::handleSDL_KEYDOWN (const SDL_Event *event)
{
	SDLMod mods = SDL_GetModState();
	// alt-code input
	if(mods & KMOD_ALT)
	{
		if(mode == MOVE_MODE) switch(event->key.keysym.sym)
		{
			case SDLK_KP0:
			case SDLK_0: alt_code_str[alt_code_len++] = '0'; break;

			case SDLK_KP1:
			case SDLK_1: alt_code_str[alt_code_len++] = '1'; break;

			case SDLK_KP2:
			case SDLK_2: alt_code_str[alt_code_len++] = '2'; break;

			case SDLK_KP3:
			case SDLK_3: alt_code_str[alt_code_len++] = '3'; break;

			case SDLK_KP4:
			case SDLK_4: alt_code_str[alt_code_len++] = '4'; break;

			case SDLK_KP5:
			case SDLK_5: alt_code_str[alt_code_len++] = '5'; break;

			case SDLK_KP6:
			case SDLK_6: alt_code_str[alt_code_len++] = '6'; break;

			case SDLK_KP7:
			case SDLK_7: alt_code_str[alt_code_len++] = '7'; break;

			case SDLK_KP8:
			case SDLK_8: alt_code_str[alt_code_len++] = '8'; break;

			case SDLK_KP9:
			case SDLK_9: alt_code_str[alt_code_len++] = '9'; break;

			case SDLK_x: // quit
				quitDialog();
				return true;

			case SDLK_d: 
				DEBUG(0,LEVEL_EMERGENCY,"!!decrease!!\n");
				return false;
			case SDLK_i:
				DEBUG(0,LEVEL_EMERGENCY,"!!increase!!\n");
				return false;

			case SDLK_KP_ENTER:
			case SDLK_RETURN:
//				game->get_screen()->toggleFullScreen();
				break;
			default: return true;
		}
		if(alt_code_len != 0)
		{
			alt_code_str[alt_code_len] = '\0';
			if(alt_code_len == 3)
			{
				alt_code(alt_code_str);
				clear_alt_code();
			}
		}
		return true;
	}
	if(mods & KMOD_CTRL)
	{
		switch(event->key.keysym.sym)
		{
			case SDLK_s: // save
				saveDialog();
				return true;
			case SDLK_l: { // load
				SaveManager *save_manager = game->get_save_manager();
				scroll->display_string("Load game!\n");
				save_manager->load_latest_save();
				return true; }
			case SDLK_q: // quit
				quitDialog();
				return true;
			default:
				if(event->key.keysym.sym != SDLK_LCTRL
				   && event->key.keysym.sym != SDLK_RCTRL)
				{
					if(mode != MOVE_MODE)
						cancelAction();
					else
					{
						scroll->display_string("what?\n\n");
						scroll->display_prompt();
					}
				}
		}
		return true;
	}

	// when casting the magic class will handle keyboard events
	if(mode == KEYINPUT_MODE)
	{
		input.type = EVENTINPUT_KEY;
		input.key = event->key.keysym.sym;
        // callback should return a true value if it handled the event
		if(input.key != SDLK_ESCAPE && message(CB_DATA_READY, (char*)&input))
    		return true;
        callback_target = 0;
        endAction(); // no more keys for you! (end KEYINPUT_MODE)
	}
	switch (event->key.keysym.sym)
	{
		//	keypad arrow keys
		case SDLK_KP7   :
			move(-1,-1);
			break;
		case SDLK_KP9   :
			move(1,-1);
			break;
		case SDLK_KP1   :
			move(-1,1);
			break;
		case SDLK_KP3   :
			move(1,1);
			break;
		case SDLK_KP8   :
			move(0,-1);
			break;
		case SDLK_KP2   :
			move(0,1);
			break;
		case SDLK_KP4   :
			move(-1,0);
			break;
		case SDLK_KP6   :
			move(1,0);
			break;

		//	standard arrow keys
		case SDLK_UP    :
			move(0,-1);
			break;
		case SDLK_DOWN  :
			move(0,1);
			break;
		case SDLK_LEFT  :
			move(-1,0);
			break;
		case SDLK_RIGHT :
			move(1,0);
			break;

		case SDLK_BACKQUOTE :
			if(game->is_orig_style()
			   && view_manager->get_current_view()
			      == view_manager->get_inventory_view())
				view_manager->get_inventory_view()->simulate_CB_callback();
			break;
		case SDLK_TAB :
            // cursor is on mapwindow or hidden
            if(input.select_from_inventory == false)
    		    moveCursorToInventory();
		    else // cursor is on inventory
                moveCursorToMapWindow();
            break;
		case SDLK_s     :
				saveDialog();
			break;

		case SDLK_q     :
				quitDialog();

			break;
		case SDLK_c     :
			if(player->is_in_vehicle())
				display_not_aboard_vehicle();
			else
				newAction(CAST_MODE);
			break;
		case SDLK_l     :
			newAction(LOOK_MODE);
			break;
		case SDLK_t     :
			newAction(TALK_MODE);
			break;
		case SDLK_u     :
			newAction(USE_MODE);
			break;
		case SDLK_g     :
			if(player->is_in_vehicle())
				display_not_aboard_vehicle();
			else
				newAction(GET_MODE);
			break;
		case SDLK_m     :
			if(player->is_in_vehicle())
				display_not_aboard_vehicle();
			else
				newAction(PUSH_MODE);
			break;
		case SDLK_d     :
			if(player->is_in_vehicle())
				display_not_aboard_vehicle();
			else
				newAction(DROP_MODE);
			break;
		case SDLK_b     :
			newAction(COMBAT_MODE);
			break;
		case SDLK_a     :
			newAction(ATTACK_MODE);
			break;
		case SDLK_r     :
			newAction(REST_MODE);
            break;
		case SDLK_i     :
			view_manager->open_doll_view(NULL);
			break;
		case SDLK_F1:
		case SDLK_F2:
		case SDLK_F3:
		case SDLK_F4:
		case SDLK_F5:
		case SDLK_F6:
		case SDLK_F7:
		case SDLK_F8:
		case SDLK_F9:
			if(!player->get_party()->main_actor_is_in_party())
				break;
			if(player->get_party()->get_party_size()
			   >= event->key.keysym.sym - 281)
			{
				view_manager->set_inventory_mode();
				view_manager->get_inventory_view()
				->set_party_member(event->key.keysym.sym - 282);
			}
			break;
		case SDLK_F10:
		case SDLK_KP_DIVIDE :
			if(player->get_party()->main_actor_is_in_party())
		            view_manager->set_party_mode();
		        break;
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
		case SDLK_EQUALS:
			if(!player->get_party()->main_actor_is_in_party()
			   || player->get_party()->get_party_size()
			      < view_manager->get_inventory_view()->get_party_member_num()+2)
				break;
			if(game->is_orig_style() && view_manager->get_inventory_view()
			   ->set_party_member(view_manager->get_inventory_view()
			   ->get_party_member_num()+1))
			    view_manager->set_inventory_mode();
            break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
			if(!player->get_party()->main_actor_is_in_party()
			   || view_manager->get_inventory_view()->get_party_member_num() < 1)
				break;
			if(game->is_orig_style() && view_manager->get_inventory_view()
			   ->set_party_member(view_manager->get_inventory_view()
			   ->get_party_member_num()-1))
			    view_manager->set_inventory_mode();
            break;
		case SDLK_1:
		case SDLK_2:
		case SDLK_3:
		case SDLK_4:
		case SDLK_5:
		case SDLK_6:
		case SDLK_7:
		case SDLK_8:
		case SDLK_9:
            if(mode == INPUT_MODE)
                select_party_member(event->key.keysym.sym - 48 - 1);
            else if(player->is_in_vehicle())
                display_not_aboard_vehicle();
            else
    			solo_mode(event->key.keysym.sym - 48 - 1);
			break;
		case SDLK_0:
			if(mode == MOVE_MODE)
    			party_mode();
            else
                cancelAction();
			break;
		case SDLK_RETURN  :
		case SDLK_KP_ENTER:
            doAction();
			break;
		case SDLK_ESCAPE:
		case SDLK_SPACE :
			cancelAction();
			break;
		default :
			if (event->key.keysym.sym != SDLK_LALT
				&& event->key.keysym.sym != SDLK_RALT)
			{
				if(mode != MOVE_MODE)
					cancelAction();
				else
				{
					scroll->display_string("what?\n\n");
					scroll->display_prompt();
				}
			}
			break;
	}	//	switch (event->key.keysym.sym)
	return	true;
}

bool Event::handleEvent(const SDL_Event *event)
{
	if(game->user_paused())
		return true;

	switch (event->type)
	{
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_KEYUP:
			if(event->key.keysym.sym == SDLK_RALT
			   || event->key.keysym.sym == SDLK_LALT)
			{
				clear_alt_code();
			}
			break;

		case SDL_KEYDOWN:
			handleSDL_KEYDOWN (event);
			break;

		case SDL_QUIT :
				quitDialog();
			break;

		default:
			break;
	}

    if(input.get_text && scroll->has_input())
    {
    	if(active_alt_code)
        {
            endAction(); // exit INPUT_MODE
    		alt_code_input(scroll->get_input().c_str());
        }
        else
        {
            doAction();
        }
	}
	return true;
}


void Event::get_direction(const char *prompt)
{
//    use_obj = src;
    assert(mode != INPUT_MODE);
    set_mode(INPUT_MODE); // saves previous mode
    if(prompt)
        scroll->display_string(prompt);
    input.get_direction = true;

    moveCursorToMapWindow();
    map_window->centerCursor();
    map_window->set_show_cursor(false);
    map_window->set_show_use_cursor(true);

    input.target_init = new MapCoord(map_window->get_cursorCoord()); // depends on MapWindow size
}
/* This version of get_direction() doesn't show the cursor. */
void Event::get_direction(const MapCoord &from, const char *prompt)
{
    get_direction(prompt);
    map_window->moveCursor(from.x, from.y);
    input.target_init->x = from.x;
    input.target_init->y = from.y;

    map_window->set_show_use_cursor(false);
}

void Event::get_target(const char *prompt)
{
//    use_obj = src;
    assert(mode != INPUT_MODE);
    set_mode(INPUT_MODE); // saves previous mode
    if(prompt)
        scroll->display_string(prompt);
    input.get_direction = false;

    map_window->centerCursor();
    moveCursorToMapWindow();
}

void Event::get_target(const MapCoord &init, const char *prompt)
{
    get_target(prompt);
    map_window->moveCursor(init.x, init.y);
}

/* Switch focus to MsgScroll and start getting user input. */
void Event::get_scroll_input(const char *allowed, bool can_escape)
{
    assert(scroll);
    assert(mode != INPUT_MODE);
    set_mode(INPUT_MODE); // saves previous mode
    input.get_text = true;
    scroll->set_input_mode(true, allowed, can_escape);
//no need to grab focus because any input will eventually reach MsgScroll,
//    scroll->grab_focus();
}

void Event::get_inventory_obj(Actor *actor)
{
	get_target("");
	moveCursorToInventory();
	view_manager->get_inventory_view()->set_actor(actor);
}

void Event::get_spell_num(Actor *caster, Obj *spell_container)
{
	//get_target("");
	view_manager->set_spell_mode(caster, spell_container, true);
	view_manager->get_current_view()->grab_focus();
}

/* Send all keyboard input to caller, with user_data.
   ESC always cancels sending any further input. */
void Event::key_redirect(CallBack *caller, void *user_data)
{
    assert(mode != INPUT_MODE && mode != KEYINPUT_MODE);
    request_input(caller, user_data);
    set_mode(KEYINPUT_MODE); // saves previous mode
}

void Event::cancel_key_redirect()
{
    assert(mode == KEYINPUT_MODE);
    endAction();
}

/* Switch focus to PortraitView, display a portrait, and wait for user input. */
void Event::display_portrait(Actor *actor, const char *name)
{
    view_manager->set_portrait_mode(actor, (char *)name);
    view_manager->get_portrait_view()->set_waiting(true);
}

/* Set callback & callback_user_data so that a message will be sent to the
 * caller when input has been gathered. */
void Event::request_input(CallBack *caller, void *user_data)
{
    callback_target = caller;
    callback_user_data = (char *)user_data;
}

// typically this will be coming from inventory
bool Event::select_obj(Obj *obj, Actor *actor)
{
    assert(mode == INPUT_MODE);
    //assert(input.select_from_inventory == true);

    input.type = EVENTINPUT_OBJECT;
    input.obj = obj;
    input.actor = actor;
    endAction(); // mode = prev_mode
    doAction();
    return true;
}

bool Event::select_actor(Actor *actor)
{
    assert(mode == INPUT_MODE);

    input.type = EVENTINPUT_MAPCOORD;
    input.actor = actor;
    input.set_loc(actor->get_location());
    endAction(); // mode = prev_mode
    doAction();
    return true;
}

bool Event::select_direction(sint16 rel_x, sint16 rel_y)
{
    assert(mode == INPUT_MODE);
    assert(input.get_direction == true);

    input.type = EVENTINPUT_MAPCOORD_DIR;
    input.set_loc(MapCoord(rel_x, rel_y));
    // assumes mapwindow cursor is at the location
    input.actor = map_window->get_actorAtCursor();
    input.obj = map_window->get_objAtCursor();
    endAction(); // mode = prev_mode
    doAction();
    return true;
}

// automatically converted to direction if requested
bool Event::select_target(uint16 x, uint16 y, uint8 z)
{
    // FIXME: is this even correct behavior?! if an arrow key is used, a direction
    // should be returned, but you can still select any target with the mouse
    // (which works, but then what's the point of using directions?)
    if(input.get_direction)
        return select_direction(x-input.target_init->x,
                                y-input.target_init->y);
if(mode != ATTACK_MODE) // FIXME: make ATTACK_MODE use INPUT_MODE
{                       // need to handle weapon range
    assert(mode == INPUT_MODE);

    input.type = EVENTINPUT_MAPCOORD;
    input.set_loc(MapCoord(x, y, z));
    // assumes mapwindow cursor is at the location
    input.actor = map_window->get_actorAtCursor();
    input.obj = map_window->get_objAtCursor();
    endAction(); // mode = prev_mode
}
    doAction();
    return true;
}

// called when selecting an actor by number
bool Event::select_party_member(uint8 num)
{
    Party *party = player->get_party();
    if(num < party->get_party_size())
    {
        select_actor(party->get_actor(num));
        return true;
    }
    return false;
}

bool Event::select_spell_num(sint16 spell_num)
{
    //assert(mode == INPUT_MODE);
    //assert(input.select_from_inventory == true);



    input.type = EVENTINPUT_SPELL_NUM;
    input.spell_num = spell_num;
    //endAction(); // mode = prev_mode
    game->get_view_manager()->close_spell_mode();
    doAction();
    return true;
}

// move the cursor or walk around; do action for direction-targeted modes
bool Event::move(sint16 rel_x, sint16 rel_y)
{
 MapCoord cursor_coord;
 
 if(game->user_paused())
    return false;

 switch(mode)
  {
   case ATTACK_MODE     : cursor_coord = map_window->get_cursorCoord();
                          cursor_coord.x = WRAPPED_COORD(cursor_coord.x + rel_x,cursor_coord.z);
                          cursor_coord.y = WRAPPED_COORD(cursor_coord.y + rel_y,cursor_coord.z);
                          if(player->weapon_can_hit(cursor_coord.x, cursor_coord.y) == false)
                            break;
                          DEBUG(0,LEVEL_DEBUGGING,"attack select(%d,%d)\n", cursor_coord.x, cursor_coord.y);
/* // (handled by INPUT_MODE)
   case LOOK_MODE       :
   case TALK_MODE       :
   case EQUIP_MODE      :
   case DROP_MODE       : map_window->moveCursorRelative(rel_x,rel_y);
                          break;
   case USE_MODE        : use(rel_x,rel_y);
                          break;
*/
   case INPUT_MODE      : map_window->moveCursorRelative(rel_x,rel_y);
                          if(input.get_direction) select_direction(rel_x,rel_y);
                          break;
/* // (handled by INPUT_MODE)
   case PUSH_MODE       : if(push_obj == NULL && push_actor == NULL)
                            pushFrom(rel_x,rel_y);
                          else
                            pushTo(rel_x,rel_y,PUSH_FROM_OBJECT);
                          break;
   case GET_MODE        : get(rel_x,rel_y);
                          break;
*/
   default              : if(player->check_walk_delay())
                            {
                             player->moveRelative(rel_x, rel_y);
                             game->time_changed();
                            }
                          break;
  }

 return true;
}


/* Begin a conversation with an actor if him/her/it is willing to talk.
 * Returns true if conversation starts.
 */
bool Event::perform_talk(Actor *actor)
{
    ActorManager *actor_manager = game->get_actor_manager();
    Actor *pc = player->get_actor();
    uint8 id = actor->get_actor_num();

    if(actor->is_in_vehicle())
    {
    	 scroll->display_string("Not in vehicle.\n");
    	 return false;
    }
    if(id == pc->get_actor_num())    // actor is controlled by player
    {
        // Note: being the player, this should ALWAYS use the real name
        scroll->display_string(actor->get_name());
        scroll->display_string("\n");
        scroll->display_string("Talking to yourself?\n");
        return false;
    }
    if(actor->is_in_party() && !actor->is_onscreen())
    {
        scroll->display_string(actor->get_name());
        scroll->display_string("\n");
        scroll->display_string("Not on screen.\n");
        return false;
    }
    // FIXME: this check and the "no response" messages should be in Converse
    if(!player->in_party_mode() && pc->get_actor_num() != 1) //only the avatar can talk in solo mode
    {
        // always display look-string on failure
        scroll->display_string(actor_manager->look_actor(actor));
        scroll->display_string("\n");
        scroll->display_string("Not in solo mode.\n");
    }
    else if(actor->is_sleeping())
    {
        // always display look-string on failure
        scroll->display_string(actor_manager->look_actor(actor));
        scroll->display_string("\n");
        scroll->display_string("No response.\n");
    }
    else if(converse->start(actor))    // load and begin npc script
    {
        // try to use real name
        scroll->display_string(actor->get_name());
        scroll->display_string("\n");
        // turn towards eachother
        pc->face_actor(actor);
        if(!actor->is_immobile())
            actor->face_actor(pc);
        return(true);
    }
    else    // some actor that has no script
    {
        // always display look-string on failure
        scroll->display_string(actor_manager->look_actor(actor));
        scroll->display_string("\n");
        scroll->display_string("Funny, no response.\n");
    }
    return(false);
}

/* Talk to `actor'. Return to the prompt if no conversation starts.
 * Returns the result of the talk function.
 */
bool Event::talk(Actor *actor)
{
    bool talking = true;
    if(game->user_paused())
        return(false);

    endAction();

    if(!actor)
    {
        scroll->display_string("nothing!\n");
        talking = false;
    }
    else if(!perform_talk(actor))
        talking = false;

    if(!talking)
    {
//        scroll->display_string("\n");
//        scroll->display_prompt();
        endAction(true);
    }
    return(talking);
}

bool Event::talk_cursor()
{
    Actor *actor = map_window->get_actorAtCursor();
    if(actor)
        return talk(actor);
    return talk(map_window->get_objAtCursor());
}

bool Event::talk_start()
{
   if(game->user_paused())
        return(false);
   get_target("Talk-");
   return true;
}

/* You can talk to some objects using their quality as actor number. */
bool Event::talk(Obj *obj)
{
    ActorManager *actor_manager = game->get_actor_manager();
    if(obj)
    {
    	if(game->get_game_type() == NUVIE_GAME_U6)
    	{
    		if(obj->obj_n == OBJ_U6_SHRINE
    				|| obj->obj_n == OBJ_U6_STATUE_OF_MONDAIN
    				|| obj->obj_n == OBJ_U6_STATUE_OF_MINAX
    				|| obj->obj_n == OBJ_U6_STATUE_OF_EXODUS)
    			return(talk(actor_manager->get_actor(obj->quality)));
    	}
    }
    scroll->display_string("nothing!\n");
    endAction();
    scroll->display_string("\n");
    scroll->display_prompt();
    return(false);
}

bool Event::attack()
{
	MapCoord target = map_window->get_cursorCoord();
    Actor *actor = map_window->get_actorAtCursor();

    if(actor)
        {
    		if(actor->get_actor_num() == player->get_actor()->get_actor_num()) //don't attack yourself.
    		{
    			scroll->display_string("pass.\n");
    			endAction(true);
    			return true;
    		}

            scroll->display_string(actor->get_name());
            scroll->display_string(".\n");

        }
    else
        {
    		Obj *obj = map_window->get_objAtCursor();
    		if(obj)
    		{
    			scroll->display_string(obj_manager->get_obj_name(obj->obj_n, obj->frame_n));
    			scroll->display_string(".\n");
    		}
    		else
    		{
    			scroll->display_string("nothing!\n");
    		}
        }

    map_window->set_show_cursor(false);
    player->attack(target);

    if(player->attack_select_next_weapon() == false)
    {
    	game->get_actor_manager()->startActors(); // end player turn
      endAction(true);
    }
    else
        {
            map_window->set_show_cursor(true);
            mode = ATTACK_MODE; // FIXME: need to return after WAIT_MODE
            //endAction(false);
            //newAction(ATTACK_MODE);
        }
    return true;
}

bool Event::get_start()
{
    if(game->user_paused())
        return false;
    get_direction("Get-");
    return true;
}

bool Event::push_start()
{
    if(game->user_paused())
        return false;
    push_obj = NULL;
    push_actor = NULL;
    get_direction("Move-");
    return true;
}

/* Get object into an actor. (no mode change) */
bool Event::perform_get(Obj *obj, Obj *container_obj, Actor *actor)
{
    bool got_object = false;
    //float weight;
    if(game->user_paused())
        return(false);

    if(!actor)
        actor = player->get_actor();

    if(obj && obj->is_on_map())
    {
        scroll->display_string(obj_manager->look_obj(obj));

        // perform GET usecode (can't add to container)
        if(usecode->has_getcode(obj) && (usecode->get_obj(obj, actor) == false))
        {
            scroll->display_string("\n");
            scroll->display_prompt();
            map_window->updateBlacking();
            return(false); // ???
        }
        MapCoord target(obj->x, obj->y, obj->z);

        if(player->get_actor()->get_location().distance(target) > 1)
            scroll->display_string("\nOut of range!");
        else if(game->get_script()->call_actor_get_obj(actor, obj))
        {
            obj_manager->remove_obj_from_map(obj); //remove object from map.

            actor->inventory_add_object(obj, container_obj);
            got_object = true;
        }

        if(obj_manager->is_damaging(obj->x,obj->y,obj->z))
        {
            scroll->display_string("\n");
            actor->display_condition(); // indicate that object hurt the player
        }
    }
    else
        scroll->display_string("nothing");

    scroll->display_string("\n\n");
    scroll->display_prompt();
    map_window->updateBlacking();
    return(got_object);
}

/* Get object at selected position, and end action. */
bool Event::get(sint16 rel_x, sint16 rel_y)
{
 Obj *obj;
 uint16 x,y;
 uint8 level;

 player->get_location(&x,&y,&level);

 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level, OBJ_SEARCH_TOP, OBJ_EXCLUDE_IGNORED);
 bool got_object = perform_get(obj, view_manager->get_inventory_view()->get_inventory_widget()->get_container(),
                               player->get_actor());

 view_manager->update(); //redraw views to show new item.
 endAction();

 return got_object;
}

bool Event::use_start()
{
    if(game->user_paused())
        return false;
    get_direction("Use-");
    return true;
}

bool Event::use(Obj *obj)
{
    if(game->user_paused())
        return false;
    if(!obj)
    {
        scroll->display_string("nothing\n");
        endAction(true);
        return true;
    }
    MapCoord target(obj->x, obj->y, obj->z);
    bool display_prompt = true;

    scroll->display_string(obj_manager->look_obj(obj));
    scroll->display_string("\n");

    if(!obj->is_in_inventory()
        && player->get_actor()->get_location().distance(target) > 1)
    {
        scroll->display_string("\nOut of range!\n");
        DEBUG(0,LEVEL_DEBUGGING,"distance to object: %d\n", player->get_actor()->get_location().distance(target));
    }
    else if(usecode->has_usecode(obj)) // Usable
        display_prompt = usecode->use_obj(obj, player->get_actor());
    else
    {
        scroll->display_string("\nNot usable\n");
        DEBUG(0,LEVEL_DEBUGGING,"Object %d:%d\n", obj->obj_n, obj->frame_n);
    }

    if(mode == USE_MODE) // check mode because UseCode may have changed it
        endAction(display_prompt);
    return true;
}

bool Event::use(Actor *actor)
{
    if(game->user_paused())
        return false;
    bool display_prompt = true;
    MapCoord target = actor->get_location();
    Obj *obj = actor->make_obj();

    if(usecode->has_usecode(actor))
    {
        scroll->display_string(obj_manager->look_obj(obj));
        scroll->display_string("\n");
        if(player->get_actor()->get_location().distance(target) > 1)
        {
            scroll->display_string("\nOut of range!\n");
            DEBUG(0,LEVEL_DEBUGGING,"distance to object: %d\n", player->get_actor()->get_location().distance(target));
        }
        else
            display_prompt = usecode->use_obj(obj, player->get_actor());
    }
    else
    {
        scroll->display_string("nothing\n");
        DEBUG(0,LEVEL_DEBUGGING,"Object %d:%d\n", obj->obj_n, obj->frame_n);
    }
// FIXME: usecode might request input, causing the obj to be accessed again,
// so we can't delete it in that case
assert(mode == USE_MODE || game->user_paused());
    delete_obj(obj); // we were using an actor so free the temp Obj
    if(mode == USE_MODE) // check mode because UseCode may have changed it
        endAction(display_prompt);
    return(true);
}

bool Event::use(sint16 rel_x, sint16 rel_y)
{
 map_window->centerCursor();
 map_window->moveCursorRelative(rel_x, rel_y);
 Actor *actor = map_window->get_actorAtCursor();
 Obj *obj = map_window->get_objAtCursor();

 if(game->user_paused())
    return false;

 if(obj) // FIXME: try actor first
    return(use(obj));
 else if(actor && actor->is_visible())
    return(use(actor));

 scroll->display_string("nothing\n");
 endAction(true);
 return true;
}

bool Event::look_start()
{
    if(game->user_paused())
        return(false);
    get_target("Look-");
    return true;
}

/* Returns true if object can be searched. (false if prompt shouldn't be shown)
 */
bool Event::look(Obj *obj)
{
    if(game->user_paused())
        return(false);
   
   if(obj)
   {
      obj_manager->print_obj(obj, false); // DEBUG
      if(game->get_script()->call_look_obj(obj) == false)
      {
         scroll->display_prompt();
         return false;
      }
      scroll->display_string("\n");
   }
   
   return true;
}


/* Returns true if there was a portrait for actor. */
bool Event::look(Actor *actor)
{
    ActorManager *actor_manager = game->get_actor_manager();
    sint16 p_id = -1; // party member number of actor
    bool had_portrait = true;

    if(game->user_paused())
        return(false);

    display_portrait(actor);
    had_portrait = view_manager->get_portrait_view()->get_waiting();

    actor_manager->print_actor(actor); // DEBUG
    scroll->display_string("Thou dost see ");
    // show real actor name and portrait if in avatar's party
    if((p_id = player->get_party()->get_member_num(actor)) >= 0)
        scroll->display_string(player->get_party()->get_actor_name(p_id));
    else
        scroll->display_string(actor_manager->look_actor(actor, true));
    scroll->display_string("\n");
    return(had_portrait);
}


bool Event::search(Obj *obj)
{
    MapCoord player_loc = player->get_actor()->get_location(),
             target_loc = map_window->get_cursorCoord();

    if(game->user_paused())
        return(false);

    if(!(obj->status & OBJ_STATUS_IN_INVENTORY) && player_loc.distance(target_loc) <= 1)
    {
        scroll->display_string("Searching here, you find ");
        if(!obj || !usecode->search_obj(obj, player->get_actor()))
            scroll->display_string("nothing.");
        else
        {
            scroll->display_string(".");
            map_window->updateBlacking(); // secret doors
        }
        return(true);
    }
    return(false);
}

// looks at the whatever is at MapWindow cursor location
bool Event::look_cursor()
{
 bool display_prompt = true;
 Obj *obj = map_window->get_objAtCursor();
 Actor *actor = map_window->get_actorAtCursor();

 if(game->user_paused())
   return false;

 if(actor)
   display_prompt = !look(actor);
 else if(obj && !(obj->status & OBJ_STATUS_INVISIBLE)) // don't display weight or do usecode for obj under actor or invisible objects
  {
   if(look(obj))
     search(obj);
   else
     display_prompt = false;
  }
 else // ground
  {
   scroll->display_string("Thou dost see ");
   scroll->display_string(map_window->lookAtCursor());
   scroll->display_string("\n");
  }

 endAction(display_prompt);
 return true;
}

bool Event::pushTo(Obj *obj, Actor *actor)
{
	bool ok = false;

	if(obj)
	{
		scroll->display_string(obj_manager->look_obj(obj));
		scroll->display_string("\n");
		if(obj_manager->can_store_obj(obj,push_obj))
		{
			if(obj != push_obj)
				ok = obj_manager->moveto_container(push_obj, obj);
		}
	}
	else
	{
		if(actor)
		{
			scroll->display_string(actor->get_name());
			scroll->display_string("\n");
			ok = obj_manager->moveto_inventory(push_obj, actor);
		}
	}

	if(!ok)
	{
		if(game->get_game_type() == NUVIE_GAME_U6 && obj->obj_n == OBJ_U6_VORTEX_CUBE)
			scroll->display_string("\nOnly moonstones can go into the vortex cube.\n");
		else if(obj->is_in_inventory() && !obj->container)
			scroll->display_string("\nnot a container\n");
		else
			scroll->display_string("Not possible!\n");
	}

    scroll->display_prompt();
    map_window->reset_mousecenter(); // FIXME: put this in endAction()?
    endAction();
    return(true);
}

/* Move selected object in direction relative to object.
 * (coordinates can be relative to player or object)
 */
bool Event::pushTo(sint16 rel_x, sint16 rel_y, bool push_from)
{
    Tile *obj_tile;
    bool can_move = false; // some checks must determine if object can_move
    Map *map = game->get_game_map();
    MapCoord pusher = player->get_actor()->get_location();
    MapCoord from, to; // absolute locations: object, target
    sint16 pushrel_x, pushrel_y; // direction relative to object
    LineTestResult lt;

    if(game->user_paused())
        return(false);

    if(!push_actor && !push_obj)
    {
        scroll->display_string("what?\n\n");
        scroll->display_prompt();
        map_window->reset_mousecenter();
        endAction();
        return(false);
    }

    if(push_actor)
        from = push_actor->get_location();
    else
    {
        if(push_obj->is_on_map())
    	{
            from = MapCoord(push_obj->x, push_obj->y, push_obj->z);
    	} 
    	else
    	{
            // exchange inventory.
            Actor *src_actor = push_obj->get_actor_holding_obj();
            if(src_actor)
            {
            	Actor *target_actor = map->get_actor(rel_x, rel_y, from.z);
            	if(can_move_obj_between_actors(push_obj, src_actor, target_actor, true))
            		obj_manager->moveto_inventory(push_obj, target_actor);
            }

            scroll->display_prompt();
            map_window->reset_mousecenter(); // FIXME: put this in endAction()?
            endAction();
            return(true);
    	}
    }

    if(push_from == PUSH_FROM_PLAYER) // coordinates must be converted
    {
        to.x = pusher.x + rel_x;
        to.y = pusher.y + rel_y;
    }
    else
    {
        to.x = from.x + rel_x;
        to.y = from.y + rel_y;
    }
    pushrel_x = to.x - from.x; pushrel_y = to.y - from.y;
    // you can only push one space at a time
    pushrel_x = (pushrel_x == 0) ? 0 : (pushrel_x < 0) ? -1 : 1;
    pushrel_y = (pushrel_y == 0) ? 0 : (pushrel_y < 0) ? -1 : 1;
    to.x = from.x + pushrel_x; to.y = from.y + pushrel_y;
    to.z = from.z;

    scroll->display_string(get_direction_name(pushrel_x, pushrel_y));
    scroll->display_string(".\n\n");
    DEBUG(0,LEVEL_WARNING,"deduct moves from player\n");
    // FIXME: the random chance here is just made up, I don't know what
    //        kind of check U6 did ("Failed.\n\n")
    if(push_actor)
    {
        // if actor can take a step, do so; else 50% chance of pushing them
        if(map->lineTest(to.x, to.y, to.x, to.y, to.z, LT_HitActors | LT_HitUnpassable, lt))
            scroll->display_string("Blocked.\n\n");
        else if(!push_actor->moveRelative(pushrel_x, pushrel_y))
        {
            if(push_actor->can_be_moved() && NUVIE_RAND() % 2) // already checked if target is passable
                push_actor->move(to.x, to.y, from.z, ACTOR_FORCE_MOVE);
            else
                scroll->display_string("Failed.\n\n");
        }
    }
    else
    {
        if(map->lineTest(to.x, to.y, to.x, to.y, to.z, LT_HitActors | LT_HitUnpassable, lt))
            {
             if(lt.hitObj)
              {
               if(obj_manager->can_store_obj(lt.hitObj, push_obj)) //if we are moving onto a container.
               {
            	   can_move = obj_manager->moveto_container(push_obj, lt.hitObj);
               }
               else
               {
				   // We can place an object on a bench or table. Or on any other object if
				   // the object is passable and not on a boundary.

				   obj_tile = obj_manager->get_obj_tile(lt.hitObj->obj_n, lt.hitObj->frame_n);
				   if(obj_tile->flags3 & TILEFLAG_CAN_PLACE_ONTOP ||
					  (obj_tile->passable && !map->is_boundary(lt.hit_x, lt.hit_y, lt.hit_level)) )
				   {
					 /* do normal move if no usecode or return from usecode was true */
					 if(!usecode->has_movecode(push_obj) || usecode->move_obj(push_obj,pushrel_x,pushrel_y))
						can_move = obj_manager->move(push_obj,to.x,to.y,from.z);
				   }
               }
              }
            }
         else
         {
        	 Obj *obj = obj_manager->get_obj(to.x,to.y,to.z);
        	 if(obj && obj_manager->can_store_obj(obj, push_obj)) //if we are moving onto a container.
        	 {
        		 can_move = obj_manager->moveto_container(push_obj, obj);
        	 }
        	 else
        	 {
        		 /* do normal move if no usecode or return from usecode was true */
        		 if(!usecode->has_movecode(push_obj) || usecode->move_obj(push_obj,pushrel_x,pushrel_y))
        			 can_move = obj_manager->move(push_obj,to.x,to.y,from.z);
        	 }
         }

        if(!can_move)
          scroll->display_string("Blocked.\n\n");
    }
    scroll->display_prompt();
    map_window->reset_mousecenter(); // FIXME: put this in endAction()?
    endAction();
    return(true);
}

bool Event::pushFrom(Obj *obj)
{
	scroll->display_string(obj_manager->look_obj(obj));
	push_obj = obj;
	get_target("\nTo ");
	return true;
}

/* Select object to move. */
bool Event::pushFrom(sint16 rel_x, sint16 rel_y)
{
    ActorManager *actor_manager = game->get_actor_manager();
    MapCoord from = player->get_actor()->get_location();

    if(game->user_paused())
        return(false);

    map_window->set_show_use_cursor(false);
    if(rel_x || rel_y)
    {
        push_obj = obj_manager->get_obj((uint16)(from.x+rel_x), (uint16)(from.y+rel_y), from.z);
        push_actor = actor_manager->get_actor((uint16)(from.x+rel_x), (uint16)(from.y+rel_y), from.z);
    }
    if(push_obj
       && (obj_manager->get_obj_weight(push_obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS) == 0))
        push_obj = NULL;

    MapCoord target;
    if(push_actor)
    {
        target = push_actor->get_location();
        scroll->display_string(push_actor->get_name());
        push_obj = NULL;
    }
    else if(push_obj)
    {
        target = MapCoord(push_obj->x, push_obj->y, push_obj->z);
        scroll->display_string(obj_manager->look_obj(push_obj));
        push_actor = NULL;
    }
    else
    {
        scroll->display_string("nothing.\n\n");
        scroll->display_prompt();
        endAction();
        return false;
    }

    if(from.distance(target) > 1)
    {
        scroll->display_string("\n\nOut of range!\n\n");
        scroll->display_prompt();
        endAction();
    }
    else
    {
        // FIXME: not taking win_width into account
        map_window->set_mousecenter(target.x - from.x + 5, target.y - from.y + 5);
        get_direction(MapCoord(target.x, target.y), "\nTo ");
    }
    return true;
}


/* Send input to active alt-code. */
void Event::alt_code_input(const char *in)
{
    ActorManager *am = game->get_actor_manager();
    Actor *a = am->get_actor((uint8)strtol(in, NULL, 10));
    static string teleport_string = "";
    static Obj obj;
	uint8 a_num = 0;
    switch(active_alt_code)
    {
        case 300: // show NPC portrait (FIXME: should be show portrait number)
            if(a)
            {
            	am->print_actor(a); //print actor debug info
                display_portrait(a);
            }
            scroll->display_string("\n");
            active_alt_code = 0;
            break;

        case 400: // talk to NPC (FIXME: get portrait and inventory too)
        	a_num = (uint8)strtol(in, NULL, 10);
            if(a_num == 0 || !converse->start(a_num))
            {
                scroll->display_string("\n");
                scroll->display_prompt();
            }
            active_alt_code = 0;
            break;

/*        case 214:
            alt_code_teleport(in); //teleport player & party to location string
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;
*/

        case 214: // teleport player & party to location string
            teleport_string += " ";
            teleport_string += in;
            ++alt_code_input_num;
            if(alt_code_input_num == 1)
            {
                if(game->get_game_type() ==NUVIE_GAME_U6)
                    scroll->display_string("\n<uai>: ");
                else
                    scroll->display_string("\ny: ");
                get_scroll_input();
            }
            else if(alt_code_input_num == 2)
            {
                if(game->get_game_type() ==NUVIE_GAME_U6)
                    scroll->display_string("\n<zi>: ");
                else
                    scroll->display_string("\nz: ");
                get_scroll_input();
            }
            else
            {
                alt_code_teleport(teleport_string.c_str());
                scroll->display_string("\n");
                scroll->display_prompt();
                teleport_string = "";
                alt_code_input_num = 0;
                active_alt_code = 0;
            }
            break;

        case 314: // teleport player & party to selected location
            if(strtol(in, NULL, 10) != 0)
                alt_code_teleport_menu((uint32)strtol(in, NULL, 10));
            if(strtol(in, NULL, 10) == 0 || alt_code_input_num > 2)
            {
                scroll->display_string("\n");
                scroll->display_prompt();
                alt_code_input_num = 0;
                active_alt_code = 0;
            }
            break;

        case 414: // teleport player & party to NPC location
            if(a->get_z() > 5)
                scroll->display_string("\nnpc is invalid or at invalid location");
            else
                alt_code_teleport_to_person((uint32)strtol(in, NULL, 10));
            scroll->display_string("\n\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 500: // control/watch anyone
            if(a->get_z() > 5 || a->get_actor_num() == 0)
                scroll->display_string("\nnpc is invalid or at invalid location\n\n");
          else
          {
            player->set_actor(a);
            player->set_mapwindow_centered(true);
            view_manager->set_inventory_mode(); // reset inventoryview
            view_manager->get_inventory_view()->set_actor(player->get_actor());
            scroll->display_string("\n");
          }
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 456: // polymorph
            if(alt_code_input_num == 0)
            {
                obj.obj_n = strtol(in, NULL, 10);
                scroll->display_string("\nNpc number? ");
                get_scroll_input();
                ++alt_code_input_num;
            }
            else
            {
                a->morph(obj.obj_n);
                scroll->display_string("\nMorphed!\n\n");
                scroll->display_prompt();
                alt_code_input_num = 0;
                active_alt_code = 0;
            }
            break;
    }
}


/* Get an alt-code from `cs' and use it.
 */
void Event::alt_code(const char *cs)
{
    uint16 c = (uint16)strtol(cs, NULL, 10);
    switch(c)
    {
        case 300: // display portrait by number
            scroll->display_string("Portrait? ");
            get_scroll_input();
            active_alt_code = c;
            break;

        case 400: // talk to anyone (FIXME: get portrait and inventory too)
            scroll->display_string("Npc number? ");
            get_scroll_input();
            active_alt_code = c;
            break;

        case 500: // control/watch anyone
            if(player->is_in_vehicle()
               || game->get_party()->is_in_combat_mode())
            {
                 if(player->is_in_vehicle())
                     display_not_aboard_vehicle(false);
                 else
                     scroll->display_string("\nNot while in combat mode!\n\n");
                 scroll->display_prompt();
                 active_alt_code = 0;
                 break;
            }
            scroll->display_string("Npc number? ");
            get_scroll_input();
            active_alt_code = c;
            break;

        case 456: // polymorph
            scroll->display_string("Object number? ");
            get_scroll_input();
            active_alt_code = c;
            break;

        case 213:
            alt_code_infostring();
            active_alt_code = 0;
            break;

/*        case 214:
            scroll->display_string("Location: \n",2);
            scroll->display_string(" ",0);
            get_scroll_input();
            active_alt_code = c;
            break;
*/
        case 214:
            if(player->is_in_vehicle())
            {
                if(game->get_game_type() ==NUVIE_GAME_U6)
                    scroll->display_string("\n<nat uail abord wip!>\n");
                else
                    display_not_aboard_vehicle();
                scroll->display_prompt();
                active_alt_code = 0;
            }
            else
            {
                if(game->get_game_type() ==NUVIE_GAME_U6)
                    scroll->display_string("\n<gotu eks>: ");
                else
                    scroll->display_string("\ngoto x: ");
                get_scroll_input();
                active_alt_code = c;
            }
            break;

        case 215:
            //clock->advance_to_next_hour();
        	game->get_script()->call_advance_time(60);
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
            game->time_changed();
            active_alt_code = 0;
            break;

        case 216:
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 314: // teleport player & party to selected location
            if(player->is_in_vehicle())
            {
                display_not_aboard_vehicle();
                active_alt_code = 0;
            }
            else
            {
                alt_code_teleport_menu(0);
                active_alt_code = c;
            }
            break;

        case 414: // teleport player & party to NPC location
            scroll->display_string("Npc number? ");
            get_scroll_input();
            active_alt_code = c;
            break;

        case 600: // map editor
        	view_manager->open_mapeditor_view();
        	active_alt_code = 0;
        	break;
    }
}

bool Event::alt_code_teleport(const char *location_string)
{
 char *next_num;
 uint16 x, y, z;

 if(!location_string || !strlen(location_string))
   return false;

 x = strtol(location_string,&next_num,16);
 y = strtol(next_num,&next_num,16);
 z = strtol(next_num,&next_num,16);

 if((x == 0 && y == 0) || z > 5)
   return false;
 player->move(x,y,z);

 // This is a bit of a hack but we would like to update the music when teleporting.
 game->get_party()->update_music();

 return true;
}

// changed to show time instead of date (SB-X)
void Event::alt_code_infostring()
{
 char buf[14]; // kkhhmmxxxyyyz
 uint8 karma;
 uint8 hour;
 uint8 minute;
 uint16 x, y;
 uint8 z;

 karma = player->get_karma();
 player->get_location(&x,&y,&z);

 hour = clock->get_hour();
 minute = clock->get_minute();

 sprintf(buf, "%02d%02d%02d%03X%03X%x", karma, hour,minute, x,y,z);

 scroll->display_string(buf);
 scroll->display_string("\n");
 new PeerEffect((x-x%8)-18,(y-y%8)-18,z); // wrap to chunk boundary, and center
                                          // in 11x11 MapWindow
}


/* Move player to NPC location. */
bool Event::alt_code_teleport_to_person(uint32 npc)
{
    ActorManager *actor_manager = game->get_actor_manager();
    MapCoord actor_location = actor_manager->get_actor(npc)->get_location();
    player->move(actor_location.x, actor_location.y, actor_location.z);
    if(!actor_manager->toss_actor(player->get_actor(), 2, 2))
        actor_manager->toss_actor(player->get_actor(), 4, 4);
    return(true);
}


/* Display teleport destinations, get input. */
void Event::alt_code_teleport_menu(uint32 selection)
{
    static uint8 category = 0;
    const char *teleport_dest = "";
    if(alt_code_input_num == 0) // select category
    {
        scroll->display_string("\nLazy Teleporters' Menu!\n");
        scroll->display_string(" 1) Cities\n");
        scroll->display_string(" 2) Major Areas\n");
        scroll->display_string(" 3) Shrines\n");
        scroll->display_string(" 4) Gargoyles\n");
        scroll->display_string(" 5) Dungeons\n");
        scroll->display_string(" 6) Other\n");
        scroll->display_string("Category? ");
        get_scroll_input("0123456");
    }
    else if(alt_code_input_num == 1) // selected category, select location
    {
        category = selection;
        scroll->display_string("\n");
        switch(selection)
        {
            case 1:
                scroll->display_string("Cities\n");
                scroll->display_string(" 1) Britain\n");
                scroll->display_string(" 2) Trinsic\n");
                scroll->display_string(" 3) Yew\n");
                scroll->display_string(" 4) Minoc\n");
                scroll->display_string(" 5) Moonglow\n");
                scroll->display_string(" 6) Jhelom\n");
                scroll->display_string(" 7) Skara Brae\n");
                scroll->display_string(" 8) New Magincia\n");
                scroll->display_string(" 9) Buc's Den\n");
                scroll->display_string("Location? ");
                get_scroll_input("0123456789");
                break;
            case 2:
                scroll->display_string("Major Areas\n");
                scroll->display_string(" 1) Cove\n");
                scroll->display_string(" 2) Paws\n");
                scroll->display_string(" 3) The Hold\n");
                scroll->display_string(" 4) The Abbey\n");
                scroll->display_string(" 5) Lycaeum\n");
                scroll->display_string(" 6) Library\n");
                scroll->display_string(" 7) Sutek\n");
                scroll->display_string(" 8) Stonegate\n");
                scroll->display_string(" 9) The Codex\n");
                scroll->display_string("Location? ");
                get_scroll_input("0123456789");
                break;
            case 3:
                scroll->display_string("Shrines\n");
                scroll->display_string(" 1) Honesty\n");
                scroll->display_string(" 2) Compassion\n");
                scroll->display_string(" 3) Valor\n");
                scroll->display_string(" 4) Justice\n");
                scroll->display_string(" 5) Sacrifice\n");
                scroll->display_string(" 6) Honor\n");
                scroll->display_string(" 7) Humility\n");
                scroll->display_string(" 8) Spirituality\n");
                scroll->display_string("Location? ");
                get_scroll_input("012345678");
                break;
            case 4:
                scroll->display_string("Gargoyles\n");
                scroll->display_string(" 1) Hall\n");
                scroll->display_string(" 2) Singularity\n");
                scroll->display_string(" 3) King's Temple\n");
                scroll->display_string(" 4) Tomb of Kings\n");
                scroll->display_string(" 5) Hythloth\n");
                scroll->display_string(" 6) Control\n");
                scroll->display_string(" 7) Passion\n");
                scroll->display_string(" 8) Diligence\n");
                scroll->display_string("Location? ");
                get_scroll_input("012345678");
                break;
            case 5:
                scroll->display_string("Dungeons\n");
                scroll->display_string(" 1) Wrong\n");
                scroll->display_string(" 2) Covetous\n");
                scroll->display_string(" 3) Destard\n");
                scroll->display_string(" 4) Shame\n");
                scroll->display_string(" 5) Deceit\n");
                scroll->display_string(" 6) Hythloth\n");
                scroll->display_string("Location? ");
                get_scroll_input("0123456");
                break;
            case 6:
                scroll->display_string("Other\n");
                scroll->display_string(" 1) Iolo's Hut\n");
                scroll->display_string(" 2) Lumberjack\n");
                scroll->display_string(" 3) Saw Mill\n");
                scroll->display_string(" 4) Thieves Guild\n");
                scroll->display_string(" 5) Wisps\n");
                scroll->display_string(" 6) Heftimus\n");
                scroll->display_string(" 7) Ant Mound\n");
                scroll->display_string(" 8) Buc's Cave\n");
                scroll->display_string(" 9) Pirate Cave\n");
                scroll->display_string("Location? ");
                get_scroll_input("0123456789");
                break;
        }
    }
    else if(alt_code_input_num == 2) // selected location, teleport
    {
        switch(category)
        {
            case 1:
                if(selection == 1) // Britain
                    teleport_dest = "133 1a3 0";
                else if(selection == 2) // Trinsic
                    teleport_dest = "19b 2e2 0";
                else if(selection == 3) // Yew
                    teleport_dest = "ec a7 0";
                else if(selection == 4) // Minoc
                    teleport_dest = "254 63 0";
                else if(selection == 5) // Moonglow
                    teleport_dest = "38a 203 0";
                else if(selection == 6) // Jhelom
                    teleport_dest = "a0 36b 0";
                else if(selection == 7) // Skara Brae
                    teleport_dest = "54 203 0";
                else if(selection == 8) // New Magincia
                    teleport_dest = "2e3 2ab 0";
                else if(selection == 9) // Buc's Den
                    teleport_dest = "246 274 0";
                break;
            case 2:
                if(selection == 1) // Cove
                    teleport_dest = "223 163 0";
                else if(selection == 2) // Paws
                    teleport_dest = "198 264 0";
                else if(selection == 3) // Serpent's Hold
                    teleport_dest = "22e 3bc 0";
                else if(selection == 4) // Empath Abbey
                    teleport_dest = "83 db 0";
                else if(selection == 5) // Lycaeum
                    teleport_dest = "37b 1a4 0";
                else if(selection == 6) // Library
                    teleport_dest = "37b 1b4 0";
                else if(selection == 7) // Sutek's Island
                    teleport_dest = "316 3d4 0";
                else if(selection == 8) // Stonegate
                    teleport_dest = "25f 11d 0";
                else if(selection == 9) // The Codex
                    teleport_dest = "39b 354 0";
                break;
            case 3:
                if(selection == 1) // Honesty
                    teleport_dest = "3a7 109 0";
                else if(selection == 2) // Compassion
                    teleport_dest = "1f7 168 0";
                else if(selection == 3) // Valor
                    teleport_dest = "9f 3b1 0";
                else if(selection == 4) // Justice
                    teleport_dest = "127 28 0";
                else if(selection == 5) // Sacrifice
                    teleport_dest = "33f a6 0";
                else if(selection == 6) // Honor
                    teleport_dest = "147 339 0";
                else if(selection == 7) // Humility
                    teleport_dest = "397 3a8 0";
                else if(selection == 8) // Spirituality
                    teleport_dest = "17 16 1";
                break;
            case 4:
                if(selection == 1) // Hall of Knowledge
                    teleport_dest = "7f af 5";
                else if(selection == 2) // Temple of Singularity
                    teleport_dest = "7f 37 5";
                else if(selection == 3) // Temple of Kings
                    teleport_dest = "7f 50 5";
                else if(selection == 4) // Tomb of Kings
                    teleport_dest = "7f 9 4";
                else if(selection == 5) // Hythloth exit
                    teleport_dest = "dc db 5";
                else if(selection == 6) // Shrine of Control
                    teleport_dest = "43 2c 5";
                else if(selection == 7) // Shrine of Passion
                    teleport_dest = "bc 2c 5";
                else if(selection == 8) // Shrine of Diligence
                    teleport_dest = "6c dc 5";
                break;
            case 5:
                if(selection == 1) // Wrong
                    teleport_dest = "1f4 53 0";
                else if(selection == 2) // Covetous
                    teleport_dest = "273 73 0";
                else if(selection == 3) // Destard
                    teleport_dest = "120 29d 0";
                else if(selection == 4) // Shame
                    teleport_dest = "eb 19b 0";
                else if(selection == 5) // Deceit
                    teleport_dest = "3c4 136 0";
                else if(selection == 6) // Hythloth
                    teleport_dest = "3b4 3a4 0";
                break;
            case 6:
                if(selection == 1) // Iolo's Hut
                    teleport_dest = "c3 e8 0";
                else if(selection == 2) // Lumberjack (Yew)
                    teleport_dest = "b2 94 0";
                else if(selection == 3) // Saw Mill (Minoc)
                    teleport_dest = "2a4 65 0";
                else if(selection == 4) // Thieves Guild
                    teleport_dest = "233 25e 0";
                else if(selection == 5) // Wisps
                    teleport_dest = "a5 115 0";
                else if(selection == 6) // Heftimus
                    teleport_dest = "84 35b 0";
                else if(selection == 7) // Ant Mound
                    teleport_dest = "365 bb 0";
                else if(selection == 8) // Buccaneer's Cave
                    teleport_dest = "234 253 0";
                else if(selection == 9) // Pirate Cave
                    teleport_dest = "2c3 342 0";
                break;
        }
        if(strlen(teleport_dest))
        {
            scroll->display_string("\n(");
            scroll->display_string(teleport_dest);
            scroll->display_string(")\n");
            alt_code_teleport(teleport_dest);
        }
    }
    ++alt_code_input_num;
}


void Event::wait()
{
 if(!ignore_timeleft)
   SDL_Delay(TimeLeft());
}

//Protected

inline Uint32 Event::TimeLeft()
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = clock->get_ticks();
    if ( next_time <= now ) {
        next_time = now+NUVIE_INTERVAL;
        return(0);
    }
    Uint32 delay = next_time-now;
    next_time += NUVIE_INTERVAL;
    return(delay);
}

void Event::quitDialog()
{
	GUI_Widget *quit_dialog;
	if(mode == MOVE_MODE && !showingQuitDialog)
	{
		map_window->set_walking(false);
		showingQuitDialog = true;

		quit_dialog = (GUI_Widget *) new GUI_YesNoDialog(gui, 75, 60, 170, 80, "Do you want to Quit", (GUI_CallBack *)this, (GUI_CallBack *)this);

		gui->AddWidget(quit_dialog);
		gui->lock_input(quit_dialog);
	}

 return;
}

void Event::saveDialog()
{
 if(!player->get_party()->main_actor_is_in_party())
 {
	scroll->display_string("\nNot when using the control npc cheat!\n\n");
	scroll->display_prompt();
	return;
 }
 SaveManager *save_manager = game->get_save_manager();
 if(mode == MOVE_MODE)
 {
	 map_window->set_walking(false);
	 save_manager->create_dialog();
 }
 return;
}

uint16 Event::callback(uint16 msg, CallBack *caller, void *data)
{
 GUI_Widget *widget;

 switch(msg) // Handle callback from quit dialog.
  {
   case YESNODIALOG_CB_YES :  showingQuitDialog = false;
                              game->get_gui()->unlock_input();
                              return GUI_QUIT;
   case YESNODIALOG_CB_NO :  widget = (GUI_Widget *)data;
                             widget->Delete();

                             showingQuitDialog = false;
                             game->get_gui()->unlock_input();
                             return GUI_YUM;
  }

 return GUI_PASS;
}


/* Switch to solo mode.
 */
void Event::solo_mode(uint32 party_member)
{
    Actor *actor = player->get_party()->get_actor(party_member);

    if(game->user_paused())
        return;

    if(!actor || player->is_in_vehicle())
        return;

    bool main_actor_was_not_in_party = !player->get_party()->main_actor_is_in_party();

    if(player->get_party()->is_in_combat_mode())
        scroll->display_string("Not in combat mode!\n\n");
    else if(player->set_solo_mode(actor))
    {
        scroll->display_string("Solo mode\n\n");
        player->set_mapwindow_centered(true);
        actor->set_worktype(0x02); // Player
        if(view_manager->get_current_view() == view_manager->get_inventory_view())
        {
            if(main_actor_was_not_in_party)
                view_manager->set_inventory_mode(); // reset inventory view
            view_manager->get_inventory_view()->set_party_member(party_member);
        }
        else if(view_manager->get_current_view() == view_manager->get_actor_view())
        {
            if(main_actor_was_not_in_party)
                view_manager->set_actor_mode(); // reset actor view
            view_manager->get_actor_view()->set_party_member(party_member);
        }
    }
    scroll->display_prompt();
}

/* Switch to party mode. */
void Event::party_mode()
{
    MapCoord leader_loc;
    if(!player->get_party()->main_actor_is_in_party())
        view_manager->set_party_mode();
    Actor *actor = player->get_party()->get_actor(0);
    assert(actor); // there must be a leader

    if(game->user_paused())
        return;

    if(player->is_in_vehicle())
        return;

    leader_loc = actor->get_location();

    if(player->get_party()->is_in_combat_mode())
        scroll->display_string("Not in combat mode!\n");
    else if(player->get_party()->is_at(leader_loc, 6))
    {
        if(player->set_party_mode(player->get_party()->get_actor(0)))
        {
            scroll->display_string("Party mode\n");
            player->set_mapwindow_centered(true);
        }
    }
    else
        scroll->display_string("Not everyone is here.\n");
    scroll->display_string("\n");
    scroll->display_prompt();
}

/* Switch to or from combat mode. */
bool Event::toggle_combat()
{
    Party *party = player->get_party();
    bool combat_mode = !party->is_in_combat_mode();

    if(!player->in_party_mode())
    {
        scroll->display_string("Not in solo mode.\n\n");
        scroll->display_prompt();
    }
    else if(party->is_in_vehicle())
    {
         display_not_aboard_vehicle();
    }
    else if(!player->get_party()->main_actor_is_in_party())
    {
         scroll->display_string("\nNot while using control cheat!\n\n");
         scroll->display_prompt();
    }
    else
        party->set_in_combat_mode(combat_mode);

    if(party->is_in_combat_mode() == combat_mode)
    {
        if(combat_mode)
            scroll->display_string("Begin combat!\n\n");
        else
        {
            scroll->display_string("Break off combat!\n\n");
            player->set_actor(party->get_actor(party->get_leader())); // return control to leader
            player->set_mapwindow_centered(true); // center mapwindow
        }
        scroll->display_prompt();
        game->get_command_bar()->set_combat_mode(combat_mode);
        return true;
    }

    return false;
}

/* Make actor wear an object they are holding. */
bool Event::ready(Obj *obj)
{
    Actor *actor = game->get_actor_manager()->get_actor(obj->x);
    bool readied = false;

    if(game->user_paused())
        return(false);

    scroll->display_string("Ready-");
    scroll->display_string(obj_manager->look_obj(obj, false));
    scroll->display_string("\n");

    // perform READY usecode
    if(usecode->has_readycode(obj) && (usecode->ready_obj(obj, actor) == false))
    {
        scroll->display_string("\n");
        scroll->display_prompt();
        return(obj->is_readied()); // handled by usecode
    }

    if(!(readied = actor->add_readied_object(obj)))
    {
    	if(actor->get_object_readiable_location(obj) == ACTOR_NOT_READIABLE)
    		scroll->display_string("\nCan't be readied!\n");
    	else
    		scroll->display_string("\nNo place to put!\n");
    }
    scroll->display_string("\n");
    scroll->display_prompt();
    return(readied);
}


/* Make actor hold an object they are wearing. */
bool Event::unready(Obj *obj)
{
    Actor *actor = game->get_actor_manager()->get_actor(obj->x);

    if(game->user_paused())
        return(false);

    scroll->display_string("Unready-");
    scroll->display_string(obj_manager->look_obj(obj, false));
    scroll->display_string("\n");

    // perform unREADY usecode
    if(usecode->has_readycode(obj) && (usecode->ready_obj(obj, actor) == false))
    {
        scroll->display_string("\n");
        scroll->display_prompt();
        return(!obj->is_readied()); // handled by usecode
    }

    actor->remove_readied_object(obj);

    scroll->display_string("\n");
    scroll->display_prompt();
    return(true);
}


bool Event::drop_start()
{
    if(game->user_paused())
        return false;
    drop_obj = NULL;
    drop_qty = 0;
    drop_x = drop_y = -1;

//    get_obj_from_inventory(some actor, "Drop-");
//    get_obj_from_inventory("Drop-");
    get_target("Drop-");
//    moveCursorToInventory(); done in newAction()
    return true;
}

/* Print object name and select it as object to be dropped. If qty is 0, the
 * amount to drop may be requested.
 */
bool Event::drop_select(Obj *obj, uint16 qty)
{
    if(game->user_paused())
        return false;

    drop_obj = obj;
    scroll->display_string(drop_obj ? obj_manager->look_obj(drop_obj) : "nothing");
    scroll->display_string("\n");

    if(drop_obj)
    {
        if(qty == 0 && obj_manager->is_stackable(drop_obj) && drop_obj->qty > 1)
        {
            scroll->display_string("How many? ");
//            newAction(DROPCOUNT_MODE);
			get_scroll_input(); // "How many?"
            return true;
        }
        drop_count(1);
    }
    else endAction(true);

    return true;
}

/* Select quantity of `drop_obj' to be dropped. (qty 0 = drop nothing) */
bool Event::drop_count(uint16 qty)
{
    if(game->user_paused())
        return(false);

    drop_qty = qty;
    scroll->display_string("\n");

    if(drop_qty != 0)
    {
        if(drop_x == -1)
            get_target("Location:");
        else // h4x0r3d by SB-X... eventually integrate MapWindow dragndrop better with this drop-action
        {
            scroll->display_string("Location:");
            perform_drop(); // use already selected target: drop_x,drop_y
        }
    }
    else
        endAction(true); // cancelled

    return true;
}


/* Make actor holding selected object drop it at cursor coordinates. Wait for
 * drop effect to complete before ending the action.
 */
bool Event::perform_drop()
{
    if(game->user_paused())
        return false;
    if(drop_x == -1 || drop_y == -1)
    {
    	if(input.loc == NULL)
    	{
    		scroll->display_string("Not possible\n");
    		endAction(true);
    		return false;
    	}

        if(drop_x == -1) drop_x = input.loc->x;
        if(drop_y == -1) drop_y = input.loc->y;
    }

    return(drop(drop_obj, drop_qty, uint16(drop_x), uint16(drop_y)));
}


/* Make actor holding object drop it at x,y. */
bool Event::drop(Obj *obj, uint16 qty, uint16 x, uint16 y)
{
    if(game->user_paused())
        return false;

    if(obj->get_engine_loc() == OBJ_LOC_MAP)
    {
        drop_okay_to_take = obj->is_ok_to_take(); // we need to preserve flag
        obj_manager->moveto_inventory(obj, player->get_actor()); // hack to stop ghosting from drop effect
    }

    Actor *actor = (obj->is_in_inventory()) // includes held containers
                   ? obj->get_actor_holding_obj()
                    : player->get_actor();
    MapCoord actor_loc = actor->get_location();
    MapCoord drop_loc(x, y, actor_loc.z);
    sint16 rel_x = x - actor_loc.x;
    sint16 rel_y = y - actor_loc.y;
    if(rel_x != 0 || rel_y != 0)
    {
        scroll->display_string(get_direction_name(rel_x, rel_y));
        scroll->display_string(".\n");
    }

    // check drop-to location (FIXME: This is redundant if using DragnDrop)
    if(!map_window->can_drop_obj(drop_loc.x, drop_loc.y, actor) || (rel_x == 0 && rel_y == 0))
    {
        scroll->display_string("Not possible\n");
        endAction(true); // because the DropEffect is never called to do this
        return false;
    }

    // all object management is contained in the effect (use requested quantity)
    if(!usecode->has_dropcode(obj)
       || usecode->drop_obj(obj, actor, drop_loc.x, drop_loc.y, qty ? qty : obj->qty))
    {
        obj->status |= OBJ_STATUS_OK_TO_TAKE;
        new DropEffect(obj, qty ? qty : obj->qty, actor, &drop_loc);
        endAction(false);
        set_mode(MOVE_MODE);
        return true;
    }
    // handled by usecode
    endAction(true); // because the DropEffect is never called to do this
    return false;
}

bool Event::rest()
{
    if(rest_time != 0) // already got time & started the campfire; time to Rest
    {
        assert(last_mode == REST_MODE); // we'll need to clear Rest mode after
                                        // exiting Wait mode
        player->get_party()->rest_sleep(rest_time, rest_guard-1);
        return true;
    }
    scroll->display_string("Rest");


    string err_str;
    if(!player->get_party()->can_rest(err_str))
    {
        scroll->display_string(err_str);
        scroll->display_string("\n");
        endAction(true);
        return false;
    }

    if(player->get_actor()->get_obj_n() == OBJ_U6_SHIP)
    {
        scroll->display_string("\n");
        player->repairShip();
        endAction(true);
    }
    else
    {
        scroll->display_string("\nHow many hours? ");
        get_scroll_input("0123456789");
    }
    return true;
}

/* Get hours to Rest, or number of party member who will guard. These must be
   entered in order. */
bool Event::rest_input(uint16 input)
{
    Party *party = player->get_party();
    scroll->set_input_mode(false);
    scroll->display_string("\n");
    if(rest_time == 0)
    {
        rest_time = input;
        if(rest_time == 0)
        {
            endAction(true);
            return false;
        }
        if(party->get_party_size() > 1)
        {
            scroll->display_string("Who will guard? ");
            get_scroll_input("0123456789");
        }
        else
        {
            party->rest_gather(); // nobody can guard; start now
        }
    }
    else
    {
        rest_guard = input;
        if(rest_guard > party->get_party_size())
            rest_guard = 0;
        if(rest_guard == 0)
            scroll->display_string("none\n");
        else
        {
            scroll->display_string(party->get_actor(rest_guard-1)->get_name());
            scroll->display_string("\n");
        }
        scroll->display_string("\n");
        party->rest_gather();
    }
    return true;
}

void Event::cast_spell_directly(uint8 spell_num)
{
	endAction(false);
	newAction(SPELL_MODE);
	input.type = EVENTINPUT_KEY;
	input.spell_num = spell_num;
	doAction();
}

/* Walk the player towards the mouse cursor. (just 1 space for now) */
void Event::walk_to_mouse_cursor(uint32 mx, uint32 my)
{
// FIXME: might add generic walk_to() action to Player
// player->walk_to(uint16 x, uint16 y, uint16 move_max, uint16 timeout_seconds);
    int wx, wy;
    sint16 rx, ry;

    if(game->user_paused() || !player->check_walk_delay())
        return;

    // Mouse->World->RelativeDirection
    map_window->mouseToWorldCoords((int)mx, (int)my, wx, wy);
    map_window->get_movement_direction((uint16)wx, (uint16)wy, rx, ry);
    // FIXME: With U6 mouse-move, Avatar tries to move left or right around obstacles.
    player->moveRelative((rx == 0) ? 0 : rx < 0 ? -1 : 1,
                         (ry == 0) ? 0 : ry < 0 ? -1 : 1);
    game->time_changed();
}


/* Talk to NPC, read a sign, or use an object at map coordinates.
 * FIXME: should be able to handle objects from inventory
 */
void Event::multiuse(uint16 wx, uint16 wy)
{
    ActorManager *actor_manager = game->get_actor_manager();
    Obj *obj = NULL;
    Actor *actor = NULL, *player_actor = player->get_actor();
    bool using_actor = false; //, talking = false;
    MapCoord target(player_actor->get_location()); // changes to target location

    if(game->user_paused())
        return;

    obj = obj_manager->get_obj(wx, wy, target.z);
    actor = actor_manager->get_actor(wx, wy, target.z);

    // use object or actor?
    if(actor && actor->is_visible())
    {
        obj = actor->make_obj();
        using_actor = true;
        target.x = actor->get_location().x;
        target.y = actor->get_location().y;
        DEBUG(0,LEVEL_DEBUGGING,"Use actor at %d,%d\n", target.x, target.y);
    }
    else if(obj)
    {
        target.x = obj->x;
        target.y = obj->y;
        DEBUG(0,LEVEL_DEBUGGING,"Use object at %d,%d\n", obj->x, obj->y);
    }

    if(using_actor) // use or talk to an actor
    {
        if(player->get_party()->is_in_combat_mode()
           && (actor->get_alignment() == ACTOR_ALIGNMENT_EVIL
           || actor->get_alignment() == ACTOR_ALIGNMENT_CHAOTIC))
        {
            newAction(ATTACK_MODE);
            if(get_mode() == ATTACK_MODE)
            {
                map_window->moveCursor(wx - map_window->get_cur_x(), wy - map_window->get_cur_y());
                select_target(uint16(wx), uint16(wy), target.z);
            }
            delete_obj(obj); // we were using an actor so free the temp Obj
            return;
        }
        bool can_use;
        if(game->get_game_type() == NUVIE_GAME_U6 && obj->obj_n == OBJ_U6_MOUSE)
            can_use = false;
        else
            can_use = usecode->has_usecode(obj);
        if(can_use)
        {
            newAction(USE_MODE);
            select_obj(obj);
        }
        else
        {
        	if(game->is_new_style() && actor == actor_manager->get_player())
        	{
        		//open inventory here.
        		view_manager->open_doll_view(NULL);
        	}
        	else
        	{
        		newAction(TALK_MODE);
        		talk(actor);
        	}
        }
        // we were using an actor so free the temp Obj
        delete_obj(obj);
    }
    else if(obj && usecode->is_sign(obj)) // look at a sign
    {
        set_mode(LOOK_MODE);
        look(obj);
        endAction(false); // FIXME: should be in look()
    }
    else if(obj) // use a real object
    {
        set_mode(USE_MODE);
        use(obj);
    }
}


/* Do the final action for the current mode, with a selected target. */
void Event::doAction()
{
    if(game->user_paused())
        return;

    if(mode == MOVE_MODE)
    {
        scroll->display_string("what?\n");
        endAction(true);
        return;
    }
    if(mode == INPUT_MODE) // set input to current cursor coord
    {
        if(input.get_text)
        {
            assert(scroll->has_input()); // doAction should only be called when input is ready
            assert(input.str == 0);
            input.str = new string(scroll->get_input());
            endAction();
            doAction();
        }
        else if(input.select_from_inventory) // some redirection here...
            view_manager->get_inventory_view()->select_objAtCursor();
        else
            select_target(map_window->get_cursorCoord().x,map_window->get_cursorCoord().y, map_window->get_cursorCoord().z);
        // the above function will switch back to the previous mode that
        // started getting input, and call doAction() again, which should
        // eventually result in an endAction()
        return;
    }
    else if(callback_target) // send input elsewhere
    {
        message(CB_DATA_READY, (char*)&input);
        callback_target = 0;
        endAction(true);
        return;
    }

    if(mode == LOOK_MODE)
    {
        if(input.type == EVENTINPUT_OBJECT)
        {   // look() returns false if prompt was already printed
            bool prompt_in_endAction = look(input.obj);
            endAction(prompt_in_endAction);
        }
        else if(input.type == EVENTINPUT_MAPCOORD && input.actor)
        {
            bool prompt = !look(input.actor);
            endAction(prompt);
        }
        else
        {
            look_cursor();
        }
    }
    else if(mode == TALK_MODE)
    {
        //		if(talk())
        //			scroll->set_talking(true);
        if(input.type == EVENTINPUT_OBJECT)
            talk(input.obj);
        else if(input.type == EVENTINPUT_MAPCOORD && input.actor)
            talk(input.actor);
        else
            talk_cursor();
        endAction();
    }
    else if(mode == USE_MODE)
    {
    	if(input.type == EVENTINPUT_OBJECT)
            use(input.obj);
    	else if(input.type == EVENTINPUT_MAPCOORD_DIR)
    	{
            if(input.actor && usecode->has_usecode(input.actor))
            	use(input.actor);
            else
            	use(input.loc->sx,input.loc->sy);
        }
        else
        {
            scroll->display_string("what?\n");
            endAction(true);
        }
        assert(mode != USE_MODE);
    }
    else if(mode == GET_MODE)
    {
    	if(input.type == EVENTINPUT_OBJECT)
            perform_get(input.obj);
        else if(input.type == EVENTINPUT_MAPCOORD_DIR)
            get(input.loc->sx,input.loc->sy);
        else
        {
            scroll->display_string("what?\n");
            endAction(true);
        }
        endAction();
    }
    else if(mode == ATTACK_MODE)
    {
    	attack();
    }
    else if(mode == PUSH_MODE)
    {
        assert(input.type == EVENTINPUT_MAPCOORD_DIR || input.type == EVENTINPUT_OBJECT || input.type == EVENTINPUT_MAPCOORD);
        if(input.type == EVENTINPUT_MAPCOORD_DIR)
        {
			if(!push_obj && !push_actor)
				pushFrom(input.loc->sx,input.loc->sy);
			else
				pushTo(input.loc->sx,input.loc->sy,PUSH_FROM_OBJECT);
        }
        else if(input.type == EVENTINPUT_MAPCOORD)
        {
        	pushTo(input.loc->x,input.loc->y);
        }
    	else
    	{
    		move_in_inventory = true;
    		if(!push_obj)
    			pushFrom(input.obj);
    		else
    		{
    			pushTo(input.obj, input.actor);
    		}
    	}
    }
    else if(mode == DROP_MODE) // called repeatedly
    {
    	if(!drop_obj)
    	{
        	if(input.select_from_inventory == false)
        		return endAction(true);

        	if(input.type == EVENTINPUT_MAPCOORD)
        	{
        		scroll->display_string("nothing\n");
        		return endAction(true);
        	}

            assert(input.type == EVENTINPUT_OBJECT);
            drop_select(input.obj);
        }
    	else if(!drop_qty)
    	{
    	    assert(input.str);
    		drop_count(strtol(input.str->c_str(), NULL, 10));
        }
    	else
    		perform_drop();
    }
    else if(mode == REST_MODE)
    {
        assert(input.str);
        rest_input(strtol(input.str->c_str(), NULL, 10));
    }
    else if(mode == CAST_MODE || mode == SPELL_MODE)
    {
    	if(input.type == EVENTINPUT_MAPCOORD)
    	{
    		if(magic->is_waiting_for_location())
    			magic->resume(MapCoord(input.loc->x, input.loc->y, input.loc->z));
    		else
    		{
    			magic->resume();
    			view_manager->get_inventory_view()->set_party_member(game->get_party()->get_leader());
    		}
    	}
    	else if(input.type == EVENTINPUT_MAPCOORD_DIR)
    	{
    		magic->resume(get_direction_code(input.loc->sx, input.loc->sy));
    	}
    	else if(input.type == EVENTINPUT_OBJECT)
    	{
    		magic->resume(input.obj);
    		view_manager->get_inventory_view()->set_party_member(game->get_party()->get_leader());
    	}
    	else if(input.type == EVENTINPUT_SPELL_NUM)
    	{
    		if(input.spell_num != -1)
    			magic->resume_with_spell_num(input.spell_num);
    		else
    			magic->resume();
    	}
    	else
    	{
    		if(mode == CAST_MODE)
    			magic->cast();
    		else
    			magic->cast_spell_directly(input.spell_num);
    	}

		for(;magic->is_waiting_to_talk();)
		{
			talk(magic->get_actor_from_script());
			magic->resume();
		}

		if(magic->is_waiting_for_location())
			get_target("");
		else if(magic->is_waiting_for_direction())
			get_direction("");
		else if(magic->is_waiting_for_inventory_obj())
		{
			get_inventory_obj(magic->get_actor_from_script());
		}
		else if(magic->is_waiting_for_spell())
		{
			get_spell_num(player->get_actor(), magic->get_spellbook_obj());
		}
		else
		{
			endAction(true);
		}
    }
    else
        cancelAction();
}

/* Cancel the action for the current mode, switch back to MOVE_MODE if possible. */
void Event::cancelAction()
{
    if(game->user_paused())
        return;

    if(mode == INPUT_MODE) // cancel action of previous mode
    {
        endAction();
        cancelAction();
        return;
    }

    if(mode == MOVE_MODE)
    {
        scroll->display_string("Pass!\n");
        player->pass();
    }
    else if(mode == CAST_MODE)
    {
        if(magic->is_waiting_to_resume())
        	magic->resume();
        else
        {

        	scroll->display_string("nothing\n");
      	  view_manager->close_spell_mode();
        }
    }
    else if(mode == USE_MODE)
    {
    	if(callback_target)
    	{
    		message(CB_INPUT_CANCELED, (char*)&input);
    		callback_target = NULL;
    		callback_user_data = NULL;
    	}
    }
    else
    {
        scroll->display_string("what?\n");
    }

    endAction(true);
}


/* Request new EventMode, for selecting a target.
 * Returns true the mode is changed. (basically if a new "select an
 * object/direction for this action" prompt is displayed)
 */
bool Event::newAction(EventMode new_mode)
{
	map_window->set_walking(false);

	if(game->user_paused())
		return(false);

// FIXME: make ATTACK_MODE use INPUT_MODE
if(mode == ATTACK_MODE && new_mode == ATTACK_MODE)
{
    doAction();
    return(mode==ATTACK_MODE);
}
    // since INPUT_MODE must be set to get input, it wouldn't make sense that
    // a mode would be requested again to complete the action
    assert(mode != new_mode);

    // called again (same key pressed twice); equivalent of pressing ENTER so call doAction() to set input
	if(mode == INPUT_MODE && new_mode == last_mode)
	{
        doAction();
		return(!(mode == MOVE_MODE));
	}
	else if(mode != MOVE_MODE && mode != EQUIP_MODE) // already in another mode; exit
	{
		cancelAction();
		return(false);
	}
	move_in_inventory = false;
	drop_okay_to_take = true;

	set_mode(new_mode);
	if (new_mode != COMBAT_MODE)
		game->set_mouse_pointer(1);
	switch(new_mode)
	{
	  	case CAST_MODE:
			/* TODO check if spellbook ready before changing mode */
			scroll->display_string("Cast-");
		  	if(!magic->start_new_spell())
			{
			  mode=MOVE_MODE;
			  scroll->display_prompt();
			}
			else
				key_redirect((CallBack*)magic, NULL);
		  	break;
	  	case SPELL_MODE:
	  		break;
		case LOOK_MODE: look_start(); break;
		case TALK_MODE: talk_start(); break;
		case USE_MODE:  use_start();  break;
		case GET_MODE:  get_start();  break;
		case ATTACK_MODE:
			if(game->get_game_type() == NUVIE_GAME_U6
					&& player->is_in_vehicle()
					&& player->get_actor()->get_obj_n() != OBJ_U6_SHIP)
			{
				scroll->display_string("Attack-");
				display_not_aboard_vehicle(false);
				endAction(true);
				return false;
			}

			player->attack_select_init();
			map_window->set_show_cursor(true);
			break;
		case PUSH_MODE: push_start(); break;
		case DROP_MODE: drop_start();
                        // drop to EQUIP_MODE (move cursor to inventory)
		case EQUIP_MODE: // if this was called from moveCursorToInventory, the
		                 // mode has now changed, so it wont be called again
			moveCursorToInventory();
			break;
//		case DROPCOUNT_MODE:
//			get_scroll_input(); /* "How many?" */
//			break;
        case REST_MODE:
            rest_time = rest_guard = 0;
            rest();
            break;
        case COMBAT_MODE:
   			toggle_combat();
   			mode = MOVE_MODE;
   			break;
		default:
			cancelAction(); // "what?"
			return(false);
	}
	return(true); // ready for object/direction
}


/* Revert to default MOVE_MODE. (walking)
 * This clears visible cursors, and resets all variables used by actions.
 */
void Event::endAction(bool prompt)
{
    if(prompt)
    {
        scroll->display_string("\n");
        scroll->display_prompt();
    }

    if(mode == PUSH_MODE)
    {
        push_obj = NULL;
        push_actor = NULL;
    }
    else if(mode == DROP_MODE)
    {
        drop_obj = NULL;
        drop_qty = 0;
    }
    else if(mode == REST_MODE)
    {
        rest_time = rest_guard = 0;
    }
    if(mode == ATTACK_MODE) // FIXME: make ATTACK_MODE use INPUT_MODE
    {
        map_window->set_show_cursor(false);
    }

    // Revert to the previous mode, instead of MOVE_MODE.
    /* Switching from INPUT_MODE, clear state indicating the type of input
       to return, but leave returned input. Clear returned input only when
       entering INPUT_MODE, or deleting Event. */
    if(/*game->user_paused() ||*/ mode == INPUT_MODE || mode == KEYINPUT_MODE)
    {
        mode = last_mode;
//        callback_target = 0;
        input.get_text = false;
//        input.select_from_inventory = false; // indicates cursor location
        input.get_direction = false;
        map_window->set_show_use_cursor(false);
        map_window->set_show_cursor(false);
        view_manager->get_inventory_view()->set_show_cursor(false);
//    game->set_mouse_pointer(0);
        return;
    }
    else
        set_mode(MOVE_MODE);

    map_window->updateBlacking();
}
// save current mode if switching to WAIT_MODE or INPUT_MODE
void Event::set_mode(EventMode new_mode)
{
	DEBUG(0,LEVEL_DEBUGGING, "new mode = %s,  mode = %s, last mode = %s\n", print_mode(new_mode), print_mode(mode), print_mode(last_mode) );
    if(new_mode == WAIT_MODE && (last_mode == EQUIP_MODE || last_mode == REST_MODE))
        last_mode = mode;
    else if((new_mode == INPUT_MODE || new_mode == KEYINPUT_MODE))
        last_mode = mode;
    else
        last_mode = MOVE_MODE;
    mode = new_mode;

    // re-init input state
    if(mode == INPUT_MODE || mode == KEYINPUT_MODE)
    {
        if(input.target_init) delete input.target_init;
        if(input.str) delete input.str;
        if(input.loc) delete input.loc;
        input.target_init = 0;
        input.str = 0;
        input.loc = 0;
        input.actor = 0; input.obj = 0;
    }
}


void Event::moveCursorToInventory()
{
    if(mode == MOVE_MODE)
        newAction(EQUIP_MODE);
    else
    {
        map_window->set_show_cursor(false); // hide both MapWindow cursors
        map_window->set_show_use_cursor(false);
        view_manager->get_inventory_view()->set_show_cursor(true);
        view_manager->get_inventory_view()->grab_focus(); // Inventory wants keyboard input
    }
    input.select_from_inventory = true;
}

// Note that the cursor is not recentered here.
void Event::moveCursorToMapWindow()
{
    view_manager->get_inventory_view()->set_show_cursor(false);
    view_manager->get_inventory_view()->release_focus();
    if(input.get_direction) // show the correct MapWindow cursor
        map_window->set_show_use_cursor(true);
    else
        map_window->set_show_cursor(true);
    input.select_from_inventory = false;

//    map_window->grab_focus(); FIXME add move() and keyhandler to MapWindow, and uncomment this
}

static const char eventModeStrings[][16] = {
"LOOK_MODE",
"USE_MODE",
"CAST_MODE",
"SPELL_MODE", //direct spell casting without spell select etc.
"GET_MODE",
"MOVE_MODE",
"DROP_MODE",
"TALK_MODE", /* finding an actor to talk to */
"ATTACK_MODE",
"REST_MODE",
"EQUIP_MODE",
"PUSH_MODE",
"COMBAT_MODE", /* only used to cancel previous actions */
"WAIT_MODE", /* waiting for something, optionally display prompt when finished */
"INPUT_MODE",
"KEYINPUT_MODE"
};

const char *Event::print_mode(EventMode mode)
{
	return eventModeStrings[mode];
}

bool Event::can_target_icon()
{
	if(mode == INPUT_MODE && (last_mode == TALK_MODE
	   || last_mode == CAST_MODE || last_mode == SPELL_MODE
	   || last_mode == LOOK_MODE || move_in_inventory
	   || last_mode == USE_MODE))
		return true;
	else
		return false;
}

void Event::display_not_aboard_vehicle(bool show_prompt)
{
	if(player->get_actor()->get_obj_n() == OBJ_U6_INFLATED_BALLOON)
		scroll->display_string("Not while aboard balloon!\n\n");
	else
		scroll->display_string("Not while aboard ship!\n\n");
	if(show_prompt)
		scroll->display_prompt();
}

bool Event::can_move_obj_between_actors(Obj *obj, Actor *src_actor, Actor *target_actor, bool display_name) // exchange inventory
{
	MapCoord from = src_actor->get_location();

	if(target_actor)
	{
		if(game->using_hackmove())
			return true;
		if(player->is_in_vehicle())
		{
			display_not_aboard_vehicle();
			return false;
		}

		if(display_name)
		{
			scroll->display_string(target_actor == src_actor ? "yourself" : target_actor->get_name());
			scroll->display_string(".\n\n");
		}

		if(target_actor == src_actor && obj->is_in_inventory())
			return true;

		LineTestResult lt;
		Map *map = game->get_game_map();
		MapCoord to = target_actor->get_location();

		if(map->lineTest(from.x, from.y, to.x, to.y, from.z, LT_HitUnpassable, lt) == false)
		{
			if(from.distance(to) < 5)
			{
				if(game->get_script()->call_actor_get_obj(target_actor, obj))
					return true;
			}
			else
				scroll->display_string("Out of range!\n");
		}
		else
			scroll->display_string("Blocked!\n");
	}
	else
		scroll->display_string("nobody.\n\n");

	return false;
}

void Event::display_move_text(Actor *target_actor, Obj *obj)
{
	scroll->display_string("Move-");
	scroll->display_string(obj_manager->look_obj(obj, OBJ_SHOW_PREFIX));
 	scroll->display_string(" To ");
	scroll->display_string(target_actor->get_name());
	scroll->display_string(".");
}
