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
#include "U6LList.h"
#include "Event.h"

#include "UseCode.h"

#include "GUI.h"
#include "GUI_YesNoDialog.h"

//dialog callbacks

static GUI_status quitDialogYesCallback(void *data);
static GUI_status quitDialogNoCallback(void *data);

static bool showingQuitDialog = false; // Yuck! FIX find a better way to do this.

Event::Event(Configuration *cfg)
{
 config = cfg;
 clear_alt_code();
 active_alt_code = 0;
 alt_code_input_num = 0;

 use_obj = NULL;
 selected_actor = NULL;

 book = NULL;
 time_queue = game_time_queue = NULL;
}

Event::~Event()
{
 delete book;
 delete time_queue;
 delete game_time_queue;
}

bool Event::init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p,
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

 book = new Book(config);
 if(book->init() == false)
   return false;
 time_queue = new TimeQueue;
 game_time_queue = new TimeQueue;
 return true;
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
    Game::get_game()->set_mouse_pointer(0);

 return true;
}

bool Event::handleSDL_KEYDOWN (const SDL_Event *event)
{
	SDLMod mods = SDL_GetModState();
	// alt-code input
	if((mods & KMOD_ALT)
	   && ( (event->key.keysym.sym >= SDLK_KP0 && event->key.keysym.sym <= SDLK_KP9)
               || (event->key.keysym.sym >= SDLK_0 && event->key.keysym.sym <= SDLK_9) ) )
	{
		if(event->key.keysym.sym >= SDLK_KP0 && event->key.keysym.sym <= SDLK_KP9)
			alt_code_str[alt_code_len++] = (char)(event->key.keysym.sym-208);
		else
			alt_code_str[alt_code_len++] = (char)(event->key.keysym.sym);
		alt_code_str[alt_code_len] = '\0';
		if(alt_code_len == 3)
		{
			alt_code(alt_code_str);
			clear_alt_code();
		}
		return	true;
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
		case SDLK_TAB :
                        map_window->set_show_cursor(false);
                        if(mode == MOVE_MODE)
                            newAction(EQUIP_MODE);
                        else
                            view_manager->get_inventory_view()->set_show_cursor(true);
                        break;
		case SDLK_q     : 
			if(mode == MOVE_MODE && !showingQuitDialog)
			{
				showingQuitDialog = true;
				quitDialog();
			}
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
			newAction(GET_MODE);
			break;
		case SDLK_m     :
			newAction(PUSHSELECT_MODE);
			break;
                case SDLK_F1:
                case SDLK_F2:
                case SDLK_F3:
                case SDLK_F4:
                case SDLK_F5:
                case SDLK_F6:
                case SDLK_F7:
                case SDLK_F8:
                        if(view_manager->get_inventory_view()
                           ->set_party_member(event->key.keysym.sym - 282))
                            view_manager->set_inventory_mode();
                        break;
                case SDLK_F10:
                            view_manager->set_party_mode();
                        break;
                case SDLK_1:
                case SDLK_2:
                case SDLK_3:
                case SDLK_4:
                case SDLK_5:
                case SDLK_6:
                case SDLK_7:
                case SDLK_8:
			solo_mode(event->key.keysym.sym - 48 - 1);
			break;
                case SDLK_0:
                case SDLK_9:
			party_mode();
			break;
		case SDLK_RETURN  :
		case SDLK_KP_ENTER   :
			doAction();
			break;
		case SDLK_ESCAPE:
		case SDLK_SPACE :
			mode = MOVE_MODE;
			cancelAction();
			break;
		default :
			if (event->key.keysym.sym != SDLK_LALT
				&& event->key.keysym.sym != SDLK_RALT)
				if(mode != MOVE_MODE)
					cancelAction();
				else
				{
					scroll->display_string("what?\n\n");
					scroll->display_prompt();
				}
			break;
	}	//	switch (event->key.keysym.sym)
	return	true;
}

bool Event::handleEvent(const SDL_Event *event)
{
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
			if(!showingQuitDialog)
			{
				showingQuitDialog = true;
				quitDialog();
			}
			break;

		default:
			break;
	}	

	if(active_alt_code && scroll->has_input())
		alt_code_input(scroll->get_input().c_str());

	return true;
}


/* Switch focus to MsgScroll and start getting user input.
 */
void Event::get_scroll_input(const char *allowed, bool can_escape)
{
   if(scroll)
       scroll->set_input_mode(true, allowed, can_escape);
}


/* Switch focus to PortraitView, display a portrait, and wait for user input.
 */
void Event::display_portrait(Actor *actor, const char *name)
{
    view_manager->set_portrait_mode(actor, (char *)name);
    view_manager->get_portrait_view()->set_waiting(true);
}


/* Allow use-cursor move to grab a target for `use_obj'.
 */
void Event::useselect_mode(Obj *src, const char *prompt)
{
    use_obj = src;
    mode = USESELECT_MODE;
    if(prompt)
        scroll->display_string(prompt);
    map_window->centerCursor();
    map_window->set_show_cursor(false);
    map_window->set_show_use_cursor(true);
}


/* Allow free-cursor move to grab a target for `use_obj'.
 */
void Event::freeselect_mode(Obj *src, const char *prompt)
{
    use_obj = src;
    mode = FREESELECT_MODE;
    if(prompt)
        scroll->display_string(prompt);
    map_window->centerCursor();
    map_window->set_show_use_cursor(false);
    map_window->set_show_cursor(true);
}


/* Get ID of Actor at cursor and check to see if him/her/it is willing to talk
 * to the player character.
 * Returns true if conversation starts, false if they don't talk, or if there
 * is no Actor at the requested location.
 */
bool Event::talk()
{
    Actor *npc = map_window->get_actorAtCursor(),
          *pc = player->get_actor();
    uint8 id = 0;
    const char *name = NULL;
    if(!npc)
    {
        scroll->display_string("nothing!\n\n");
        scroll->display_prompt();
        return(false);
    }
    id = npc->get_actor_num();
    // actor is controlled by player
    if(id == pc->get_actor_num())
    {
        // print name or look-string if actor has no name
        name = converse->npc_name(id); // get name
        if(!name)
            name = map_window->lookAtCursor();
        scroll->display_string(name);
        scroll->display_string("\nTalking to yourself?\n\n");
        scroll->display_prompt();
        return(false);
    }
    // FIXME: this check and the "no response" messages should be in Converse
    if(!player->in_party_mode())
    {
        scroll->display_string(map_window->lookAtCursor(false));
        scroll->display_string("\nNot in solo mode.\n");
    }
    // load and begin npc script
    else if(converse->start(npc))
    {
        // print npc name if met-flag is set, or npc is in avatar's party
//        scroll->display_string(npc->get_name());
        name = converse->npc_name(id); // get name
        if(name &&
           (npc->is_met() || player->get_party()->contains_actor(npc)))
            scroll->display_string(name);
        else
            scroll->display_string(map_window->lookAtCursor(false));
        scroll->display_string("\n");
        // turn towards eachother
        pc->face_actor(npc);
        npc->face_actor(pc);
        return(true);
    }
    // some actor that has no script
    else
    {
        scroll->display_string(map_window->lookAtCursor(false));
        scroll->display_string("\nFunny, no response.\n");
    }
    scroll->display_string("\n");
    scroll->display_prompt();
    return(false);
}


bool Event::move(sint16 rel_x, sint16 rel_y)
{
 if(mode == LOOK_MODE || mode == TALK_MODE || mode == FREESELECT_MODE
    || mode == EQUIP_MODE || mode == DROP_MODE || mode == DROPTARGET_MODE)
    map_window->moveCursorRelative(rel_x,rel_y);
 else
  {
   if(mode == USE_MODE)
     {
      use(rel_x,rel_y);
     }
   else if(mode == USESELECT_MODE)
     {
      select_obj(rel_x,rel_y);
     }
   else if(mode == PUSH_MODE)
     {
      pushTo(rel_x,rel_y);
     }
   else if(mode == PUSHSELECT_MODE)
     {
      pushFrom(rel_x,rel_y);
     }
   else if(mode == GET_MODE)
     {
      get(rel_x,rel_y);
     }
   else
     {
        static uint32 walk_delay = 0, // unlike mouse-move, start with no delay
                      last_time = SDL_GetTicks();
        uint32 this_time = SDL_GetTicks();
        uint32 time_passed = this_time - last_time;
        if(sint32(walk_delay - time_passed) < 0)
            walk_delay = 0;
        else
            walk_delay -= time_passed;
        last_time = this_time;
        if(!walk_delay)
        {
            player->moveRelative(rel_x, rel_y);
            walk_delay = player->get_walk_delay();
        }
     }
  }
 return true;
}


/* Get object into an actor. (no mode change)
 */
bool Event::get(Obj *obj, Obj *container_obj, Actor *actor)
{
    bool got_object = false;
    float weight;
    if(!actor)
        actor = player->get_actor();

    if(obj)
    {
        scroll->display_string(obj_manager->look_obj(obj));

        // objects with 0 weight aren't gettable. 
        weight = obj_manager->get_obj_weight(obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS);
        if(weight != 0)
        {
            if(actor->can_carry_weight(weight))
            {
                // object is someone else's
                if(!(obj->status & OBJ_STATUS_OK_TO_TAKE))
                {
                    scroll->display_string("\n\nStealing!!!"); // or "Stop Thief!!!"
                    player->subtract_karma();
                    obj->status |= OBJ_STATUS_OK_TO_TAKE; // FIXME move to DROP
                }
                obj_manager->remove_obj(obj); //remove object from map.

                actor->inventory_add_object(obj, container_obj, true);
#if 0 /* inventory_add_object() will stack now */
                // add to container (assume the actor is holding container)
                if(container_obj)
                    actor->inventory_add_object(obj, container_obj);
                // stackable object; will make copy and delete this
                else if(obj->qty > 0)
                {
                    // only new_object() can stack (for now)
                    actor->inventory_new_object(obj->obj_n, obj->qty, obj->quality);
                    obj_manager->delete_obj(obj); // original copy defunct
                }
                else // just stick this one in there
                    actor->inventory_add_object(obj);
#endif
                got_object = true;
            }
            else
                scroll->display_string("\n\nThe total is too heavy.");
        }
        else
            scroll->display_string("\n\nNot possible.");
    }
    else
        scroll->display_string("nothing");

    scroll->display_string("\n\n");
    scroll->display_prompt();
    map_window->updateBlacking();
    return(got_object);
}


/* Get object at selected position, and end action.
 */
bool Event::get(sint16 rel_x, sint16 rel_y)
{
 Obj *obj;
 uint16 x,y;
 uint8 level;
 
 player->get_location(&x,&y,&level);

 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 bool got_object = get(obj, NULL, player->get_actor());

 view_manager->update(); //redraw views to show new item.
 endAction();

 return got_object;
}


bool Event::use(Obj *obj)
{
    MapCoord target(obj->x, obj->y, obj->z);

    scroll->display_string(obj_manager->look_obj(obj));
    scroll->display_string("\n");

    if(!(obj->status & OBJ_STATUS_IN_INVENTORY) && player->get_actor()->get_location().distance(target) > 1)
    {
        scroll->display_string("\nOut of range!\n");
        fprintf(stderr, "distance to object: %d\n", player->get_actor()->get_location().distance(target));
    }
    else if(usecode->has_usecode(obj))
        usecode->use_obj(obj, player->get_actor());
    else
    {
        scroll->display_string("\nNot usable\n");
        fprintf(stderr, "Object %d:%d\n", obj->obj_n, obj->frame_n);
    }
    map_window->updateBlacking();
    // if selecting (now in a select mode), select_obj will return to MOVE_MODE
    // else (mode is still USE_MODE) return to MOVE_MODE now
    if(mode == USE_MODE)
    {
        scroll->display_string("\n");
        scroll->display_prompt();
        endAction();
    }
    return(true);
}


bool Event::use(Actor *actor)
{
    MapCoord target = actor->get_location();
    Obj *obj = actor->make_obj();

    if(usecode->has_usecode(obj))
    {
        scroll->display_string(obj_manager->look_obj(obj));
        scroll->display_string("\n");
        if(player->get_actor()->get_location().distance(target) > 1)
        {
            scroll->display_string("\nToo far away!\n");
            fprintf(stderr, "distance to object: %d\n", player->get_actor()->get_location().distance(target));
        }
        else
            usecode->use_obj(obj, player->get_actor());
    }
    else
    {
        scroll->display_string("nothing\n");
        fprintf(stderr, "Object %d:%d\n", obj->obj_n, obj->frame_n);
    }
    obj_manager->delete_obj(obj); // we were using an actor so free the temp Obj
    map_window->updateBlacking();
    // if selecting (now in a select mode), select_obj will return to MOVE_MODE
    // else (mode is still USE_MODE) return to MOVE_MODE now
    if(mode == USE_MODE)
    {
        scroll->display_string("\n");
        scroll->display_prompt();
        endAction();
    }
    return(true);
}


bool Event::use(sint16 rel_x, sint16 rel_y)
{
 Actor *actor = NULL;
 Obj *obj = NULL;
 Map *map = Game::get_game()->get_game_map();
 uint16 x,y;
 uint8 level;

 player->get_location(&x,&y,&level);
 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 actor = map->get_actor((uint16)(x+rel_x), (uint16)(y+rel_y), level);

 if(obj)
    return(use(obj));
 else if(actor && actor->is_visible())
    return(use(actor));

 scroll->display_string("nothing\n");

 // if selecting (now in a select mode), select_obj will return to MOVE_MODE
 // else (mode is still USE_MODE) return to MOVE_MODE now
 if(mode == USE_MODE)
 {
     scroll->display_string("\n");
     scroll->display_prompt();
     endAction();
 }
 return true;
}


/* Call usecode for `use_obj' with object and actor under the cursor, or the
 * passed item(s) as the itemref.
 */
bool Event::select_obj(Obj *obj, Actor *actor)
{
    static MapCoord loc(0,0,0);
    if(!obj)
        obj = map_window->get_objAtCursor();
    if(!actor)
        actor = map_window->get_actorAtCursor();
    usecode->set_itemref(obj);
    usecode->set_itemref(NULL, actor);
    loc=map_window->get_cursorCoord();
    usecode->set_itemref(&loc);
    //if(use_obj && (obj || actor))  // removed so it works with location only.
    //hopefully won't break anything, needs reimplementation anyway.
    if(use_obj)
     usecode->use_obj(use_obj, player->get_actor());
    use_obj = NULL;
    // return to MOVE_MODE
    scroll->display_string("\n");
    scroll->display_prompt();
    endAction();
    map_window->updateBlacking();
    return(true);
}


/* Select object and actor relative to player.
 */
bool Event::select_obj(sint16 rel_x, sint16 rel_y)
{
//    uint16 x, y;
//    uint8 level;
//    player->get_location(&x, &y, &level);
    map_window->moveCursorRelative(rel_x, rel_y);
    return(select_obj(map_window->get_objAtCursor(), map_window->get_actorAtCursor()));
//    return(select_obj(obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level),
//                      Game::get_game()->get_actor_manager()->get_actor((uint16)(x+rel_x), (uint16)(y+rel_y), level)));
}


/* Returns true if object can be searched. (false if prompt shouldn't be shown)
 */
bool Event::look(Obj *obj)
{
    char weight_string[32];
    float weight;
    bool special_desc = false;

    obj_manager->print_obj(obj, false); // DEBUG
    scroll->display_string("Thou dost see ");
    scroll->display_string(obj_manager->look_obj(obj, true));

    // check for special description
    if(usecode->has_lookcode(obj))
    {
        special_desc = usecode->look_obj(obj, player->get_actor());
//        scroll->display_string("\n");
    }
    if(special_desc)
    {
        scroll->display_string("\n");
        scroll->display_prompt();
        return(false);
    }
    else
    {
        weight = obj_manager->get_obj_weight(obj);
        if(weight != 0)
        {
            if(obj->qty > 1 && obj_manager->is_stackable(obj)) //use the plural sentance.
                snprintf(weight_string,31,". They weigh %0.1f stones.",obj_manager->get_obj_weight(obj));
            else
                snprintf(weight_string,31,". It weighs %0.1f stones.",obj_manager->get_obj_weight(obj));
            scroll->display_string(weight_string);
        }
        scroll->display_string("\n\n");
        return(true);
    }
}


/* Returns true if there was a portrait for actor.
 */
bool Event::look(Actor *actor)
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    sint16 p_id = -1; // party member number of actor
    bool had_portrait = true;
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
    if(!(obj->status & OBJ_STATUS_IN_INVENTORY) && player_loc.distance(target_loc) <= 1)
    {
        scroll->display_string("Searching here, you find ");
        if(!obj || !usecode->search_obj(obj, player->get_actor()))
            scroll->display_string("nothing.\n");
        else
        {
            scroll->display_string(".\n");
            map_window->updateBlacking(); // secret doors
        }
        return(true);
    }
    return(false);
}


bool Event::look()
{
 bool display_prompt = true;
 Obj *obj = map_window->get_objAtCursor();
 Actor *actor = map_window->get_actorAtCursor();

 if(actor)
   display_prompt = !look(actor);
 else if(obj) // don't display weight or do usecode for obj under actor
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

 if(display_prompt)
  {
   scroll->display_string("\n");
   scroll->display_prompt();
  }
 return true;
}


/* Move selected object in direction.
 */
bool Event::pushTo(sint16 rel_x, sint16 rel_y)
{
    Tile *obj_tile;
    bool can_move = false;
    Map *map = Game::get_game()->get_game_map();
    LineTestResult lt;
    if(rel_x == 0 && rel_y == -1) // FIXME: move direction names somewhere else
        scroll->display_string("North.");
    else if(rel_x == 1 && rel_y == -1)
        scroll->display_string("Northeast.");
    else if(rel_x == 1 && rel_y == 0)
        scroll->display_string("East.");
    else if(rel_x == 1 && rel_y == 1)
        scroll->display_string("Southeast.");
    else if(rel_x == 0 && rel_y == 1)
        scroll->display_string("South.");
    else if(rel_x == -1 && rel_y == 1)
        scroll->display_string("Southwest.");
    else if(rel_x == -1 && rel_y == 0)
        scroll->display_string("West.");
    else if(rel_x == -1 && rel_y == -1)
        scroll->display_string("Northwest.");
    else
        scroll->display_string("nowhere.");
    scroll->display_string("\n\n");

    // FIXME: the random chance here is just made up, I don't know what
    //        kind of check U6 did ("Failed.\n\n")
    if(selected_actor)
    {
        MapCoord from(selected_actor->get_location());
        if(map->lineTest(from.x+rel_x, from.y+rel_y, from.x+rel_x, from.y+rel_y,
                         from.z, LT_HitActors | LT_HitUnpassable, lt))
            scroll->display_string("Blocked.\n\n");
        else if(!selected_actor->moveRelative(rel_x, rel_y))
        {
            if(selected_actor->can_be_moved() && NUVIE_RAND() % 2) // already checked if target is passable
                selected_actor->move(from.x+rel_x, from.y+rel_y, from.z, ACTOR_FORCE_MOVE);
            else
                scroll->display_string("Failed.\n\n");
        }
    }
    else
    {
        if(map->lineTest(use_obj->x+rel_x, use_obj->y+rel_y, use_obj->x+rel_x, use_obj->y+rel_y,
                         use_obj->z, LT_HitActors | LT_HitUnpassable, lt))
            {
             if(lt.hitObj)
              {
               // We can place an object on a bench or table. Or on any other object if
               // the object is passable and not on a boundary.
               
               obj_tile = obj_manager->get_obj_tile(lt.hitObj->obj_n, lt.hitObj->frame_n);
               if(obj_tile->flags3 & TILEFLAG_CAN_PLACE_ONTOP || 
                  (obj_tile->passable && !map->is_boundary(lt.hit_x, lt.hit_y, lt.hit_level)) )
                 can_move = true;
              }
            }
         else
           can_move = true;   

        /* do normal move if no usecode or return from usecode was true */
 
        if(can_move)
          {
           if(!usecode->has_movecode(use_obj) || usecode->move_obj(use_obj,rel_x,rel_y))
                can_move = obj_manager->move(use_obj,use_obj->x+rel_x,use_obj->y+rel_y,use_obj->z);
          }
        
        if(!can_move)
           scroll->display_string("Blocked.\n\n");
    }
    scroll->display_prompt();
    endAction();
    return(true);
}


/* Select object to move.
 */
bool Event::pushFrom(sint16 rel_x, sint16 rel_y)
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    MapCoord from = player->get_actor()->get_location();
    map_window->set_show_use_cursor(false);
    if(rel_x || rel_y)
    {
        use_obj = obj_manager->get_obj((uint16)(from.x+rel_x), (uint16)(from.y+rel_y), from.z);
        selected_actor = actor_manager->get_actor((uint16)(from.x+rel_x), (uint16)(from.y+rel_y), from.z);
    }
    if(use_obj
       && (obj_manager->get_obj_weight(use_obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS) == 0))
        use_obj = NULL;

    if(selected_actor)
    {
        scroll->display_string(selected_actor->get_name());
        scroll->display_string("\nTo ");
        mode = PUSH_MODE;
    }
    else if(use_obj)
    {
        scroll->display_string(obj_manager->look_obj(use_obj));
        scroll->display_string("\nTo ");
        mode = PUSH_MODE;
    }
    else
    {
        scroll->display_string("nothing.\n\n");
        scroll->display_prompt();
        endAction();
    }
    return(true);
}


/* Send input to active alt-code.
 */
void Event::alt_code_input(const char *in)
{
    ActorManager *am = Game::get_game()->get_actor_manager();
    Actor *a = am->get_actor((uint8)strtol(in, NULL, 10));
    static string teleport_string = "";
    static Obj obj;
    switch(active_alt_code)
    {
        case 300: // show NPC portrait (FIXME: should be show portrait number)
            if(a)
                display_portrait(a);
            scroll->display_string("\n");
            active_alt_code = 0;
            break;

        case 400: // talk to NPC (FIXME: get portrait and inventory too)
            if(!converse->start((uint8)strtol(in, NULL, 10)))
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
                scroll->display_string("\n<uai>: ");
                get_scroll_input();
            }
            else if(alt_code_input_num == 2)
            {
                scroll->display_string("\n<zi>: ");
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
            alt_code_teleport_to_person((uint32)strtol(in, NULL, 10));
            scroll->display_string("\n\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 500: // control/watch anyone
            player->set_actor(a);
            map_window->centerMapOnActor(a);
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 456: // polymorph
            if(alt_code_input_num == 0)
            {
                obj.obj_n = strtol(in, NULL, 10);
                obj.frame_n = 0;
                scroll->display_string("\nNpc number? ");
                get_scroll_input();
                ++alt_code_input_num;
            }
            else
            {
                obj.x = a->get_location().x;
                obj.y = a->get_location().y;
                obj.z = a->get_location().z;
                a->init_from_obj(&obj);
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
            scroll->display_string("\n");
            scroll->display_prompt();
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
            if(player->get_actor()->get_actor_num() == 0)
            {
                scroll->display_string("\n<nat uail abord wip!>\n");
                scroll->display_prompt();
                active_alt_code = 0;
            }
            else
            {
                scroll->display_string("\n<gotu eks>: ");
                get_scroll_input();
                active_alt_code = c;
            }
            break;

        case 215:
            clock->advance_to_next_hour();
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
            map_window->updateBlacking();
            active_alt_code = 0;
            break;

        case 216:
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 314: // teleport player & party to selected location
            if(player->get_actor()->get_actor_num() == 0)
            {
                scroll->display_string("\nNot while aboard ship!\n");
                scroll->display_prompt();
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

 if(x == 0 && y == 0)
   return false;

 player->move(x,y,z);
 
 // This is a bit of a hack but we would like to update the music when teleporting.
 Game::get_game()->get_party()->update_music();
 
 return true;
}

void Event::alt_code_infostring()
{
 char buf[18]; // kkmmddyyyyxxxyyyz
 uint8 karma;
 uint8 day;
 uint8 month;
 uint8 year;
 uint16 x, y;
 uint8 z;

 karma = player->get_karma();
 player->get_location(&x,&y,&z);

 day = clock->get_day();
 month = clock->get_month();
 year = clock->get_year();

 sprintf(buf, "%02d%02d%02d%04d%03x%03x%x", karma, month, day, year, x,y,z);

 scroll->display_string(buf);

 return;
}


/* Move player to NPC location.
 */
bool Event::alt_code_teleport_to_person(uint32 npc)
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    MapCoord actor_location = actor_manager->get_actor(npc)->get_location();
    player->move(actor_location.x, actor_location.y, actor_location.z);
    if(!actor_manager->toss_actor(player->get_actor(), 2, 2))
        actor_manager->toss_actor(player->get_actor(), 4, 4);
    return(true);
}


/* Display teleport destinations, get input.
 */
void Event::alt_code_teleport_menu(uint32 selection)
{
    static uint8 category = 0;
    char *teleport_dest = "";
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
    return(next_time-now);
}

void Event::quitDialog()
{
 GUI_Widget *quit_dialog;

 quit_dialog = (GUI_Widget *) new GUI_YesNoDialog(gui, 75, 60, 170, 80, "Do you want to Quit", quitDialogYesCallback, quitDialogNoCallback);

   
 gui->AddWidget(quit_dialog);
 gui->lock_input(quit_dialog);
 return;
}

static GUI_status quitDialogYesCallback(void *data)
{ 
 showingQuitDialog = false;
 Game::get_game()->get_gui()->unlock_input();
 return GUI_QUIT;
}

static GUI_status quitDialogNoCallback(void *data)
{
 GUI_Widget *widget;
 
 widget = (GUI_Widget *)data;
 
 widget->Delete();

 showingQuitDialog = false;
 Game::get_game()->get_gui()->unlock_input();
 return GUI_YUM;
}


/* Switch to solo mode.
 */
void Event::solo_mode(uint32 party_member)
{
    Actor *actor = player->get_party()->get_actor(party_member);

    if(player->get_actor()->get_actor_num() == 0) // vehicle
        return;

    if(actor && player->set_solo_mode(actor))
    {
        scroll->display_string("\nSolo mode\n\n");
        std::string prompt = player->get_party()->get_actor_name(party_member);
        prompt += ":\n>";
        scroll->set_prompt((char *)prompt.c_str());
        scroll->display_prompt();
        map_window->centerMapOnActor(actor);
        if(view_manager->get_inventory_view()->set_party_member(party_member))
            view_manager->set_inventory_mode(); // reset inventoryview
    }
}


/* Switch to party mode.
 */
void Event::party_mode()
{
    MapCoord leader_loc;
    Actor *actor = player->get_party()->get_actor(0);
    assert(actor); // there must be a leader

    if(player->get_actor()->get_actor_num() == 0) // vehicle
        return;

    leader_loc = actor->get_location();
    if(player->get_party()->is_at(leader_loc, 8))
    {
        if(player->set_party_mode(player->get_party()->get_actor(0)))
        {
            scroll->display_string("\nParty mode\n");
            std::string prompt = player->get_party()->get_actor_name(0);
            prompt += ":\n>";
            scroll->set_prompt((char *)prompt.c_str());
            map_window->centerMapOnActor(actor);
            if(view_manager->get_inventory_view()->set_party_member(0))
                view_manager->set_inventory_mode(); // reset inventoryview
        }
    }
    else
        scroll->display_string("Not everyone is here.\n");
    scroll->display_string("\n");
    scroll->display_prompt();
}


/* Make actor wear an object they are holding.
 */
bool Event::ready(Obj *obj)
{
    Actor *actor = Game::get_game()->get_actor_manager()->get_actor(obj->x);
    bool readied = false;
    scroll->display_string("Ready-");
    scroll->display_string(obj_manager->look_obj(obj, false));
    scroll->display_string("\n");

    // FIXME: perform READY usecode

    if(!(readied = actor->add_readied_object(obj)))
        scroll->display_string("\nCan't be readied!\n");

    scroll->display_string("\n");
    scroll->display_prompt();
    return(readied);
}


/* Make actor hold an object they are wearing.
 */
bool Event::unready(Obj *obj)
{
    Actor *actor = Game::get_game()->get_actor_manager()->get_actor(obj->x);
    scroll->display_string("Unready-");
    scroll->display_string(obj_manager->look_obj(obj, false));
    scroll->display_string("\n");

    // FIXNE: perform READY usecode

    actor->remove_readied_object(obj);

    scroll->display_string("\n");
    scroll->display_prompt();
    return(true);
}


/* Print object name and select it as object to be dropped.
 */
bool Event::drop_select(Obj *obj)
{
    return(true);
}


/* Make actor holding selected object drop it at x,y.
 */
bool Event::drop_to(uint16 x, uint16 y)
{
    return(true);
}


/* Make actor holding object drop it at x,y.
 */
bool Event::drop(Obj *obj, uint16 x, uint16 y)
{
    return(true);
}


/* Do the finishing action for the current mode, based on cursor coordinates,
 * or based on the passed player-relative coordinates.
 */
void Event::doAction(sint16 rel_x, sint16 rel_y)
{
	map_window->set_show_use_cursor(false);
	map_window->set_show_cursor(false);
	Game::get_game()->set_mouse_pointer(0);
	if(mode == LOOK_MODE)
	{
		mode = MOVE_MODE;
		look();
	}
	else if(mode == TALK_MODE)
	{
		mode = MOVE_MODE;
		if(talk())
			scroll->set_talking(true);
	}
	else if(mode == USE_MODE)
	{
		//mode = MOVE_MODE;
		use(rel_x,rel_y);
	}
	else if(mode == GET_MODE)
	{
		mode = MOVE_MODE;
		get(rel_x,rel_y);
	}
	else if(mode == PUSHSELECT_MODE)
	{
		pushFrom(rel_x,rel_y);
	}
	else if(mode == PUSH_MODE)
	{
		pushTo(rel_x,rel_y);
	}
	else if(mode == USESELECT_MODE || mode == FREESELECT_MODE)
	{
		mode = MOVE_MODE;
		select_obj();
	}
	else if(mode == MOVE_MODE)
	{
		scroll->display_string("what?\n\n");
		scroll->display_prompt();
	}
	else
		cancelAction();
}


/* Cancel the action for the current mode, switch back to MOVE_MODE if possible.
 */
void Event::cancelAction()
{
	if(mode == MOVE_MODE)
	{
		scroll->display_string("Pass!\n\n");
		scroll->display_prompt();
		player->pass();
	}
	else
	{
		scroll->display_string("what?\n\n");
		scroll->display_prompt();
	}
        endAction();
}


/* Request new EventMode, for selecting a target. This will cancel any pending
 * actions, or perform the action for the current mode.
 */
void Event::newAction(EventMode new_mode)
{
	if(mode == new_mode)
	{
		doAction();
                return;
	}
	else if(mode != MOVE_MODE)
	{
		cancelAction();
                return;
	}

	Game::get_game()->set_mouse_pointer(1);
	mode = new_mode;
	switch(new_mode)
	{
		case LOOK_MODE:
			scroll->display_string("Look-");
			map_window->centerCursor();
			map_window->set_show_cursor(true);
			break;
		case TALK_MODE:
			scroll->display_string("Talk-");
			map_window->centerCursor();
			map_window->set_show_cursor(true);
			break;
		case USE_MODE:
			scroll->display_string("Use-");
			map_window->centerCursor();
			map_window->set_show_use_cursor(true);
			break;
		case GET_MODE:
			scroll->display_string("Get-");
			map_window->centerCursor();
			map_window->set_show_use_cursor(true);
			break;
		case PUSHSELECT_MODE:
			scroll->display_string("Move-");
			map_window->centerCursor();
			map_window->set_show_use_cursor(true);
			break;
		case DROP_MODE:
			scroll->display_string("Drop-");
		case EQUIP_MODE:
			view_manager->get_inventory_view()->set_show_cursor(true);
			break;
		default:
			cancelAction();
	}
}


/* Revert to default MOVE_MODE. (walking)
 */
void Event::endAction()
{
    mode = MOVE_MODE;
    map_window->set_show_use_cursor(false);
    map_window->set_show_cursor(false);
    view_manager->get_inventory_view()->set_show_cursor(false);
    use_obj = NULL;
    selected_actor = NULL;
    map_window->updateBlacking();
    Game::get_game()->set_mouse_pointer(0);
}
