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

#include <SDL.h>

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
#include "Event.h"

#include "UseCode.h"

#include "GUI.h"
#include "GUI_area.h"
#include "GUI_button.h"
#include "GUI_text.h"

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
}

Event::~Event()
{
 delete book;
 delete time_queue;
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
 return true;
}

bool Event::update()
{
 // timed
 uint32 now = SDL_GetTicks(); // FIXME: get from Game or GameClock
 while(!time_queue->empty() && time_queue->call_timer(now))
 {
    TimedEvent *tevent = time_queue->pop_timer(); // remove from timequeue
    if(!tevent->repeat)
        delete tevent; // if not repeated, safe to delete
 }
 // polled
 SDL_Event event;
 while(SDL_PollEvent(&event))
  {
   switch(gui->HandleEvent(&event))
     {
      case GUI_PASS : if(handleEvent(&event) == false)
                         return false;
                      break;

      case GUI_QUIT : return false;
   
      default : break;
     }
  }

 return true;
}

bool Event::handleSDL_KEYDOWN (const SDL_Event *event)
{
	if(scroll->handle_input(&event->key.keysym))
		return	true;

	// alt-code input
	if((event->key.keysym.sym >= SDLK_0 && event->key.keysym.sym <= SDLK_9)
		&& (SDL_GetModState() & KMOD_ALT))
	{
		alt_code_str[alt_code_len++] = (char)event->key.keysym.sym;
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
		//	separated these out from normal arrow keys because 
		//	they eventually need to account for alt-214, etc. cheats
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
		case SDLK_TAB   : 
			view_manager->set_inventory_mode(); //show inventory view
			break;
		case SDLK_q     : 
			if(!showingQuitDialog)
			{
				showingQuitDialog = true;
				quitDialog();
			}
			break;
		case SDLK_l     :
			mode = LOOK_MODE;
			scroll->display_string("Look-");
			map_window->centerCursor();
			map_window->set_show_cursor(true);
			break;
		case SDLK_t     :
			if(mode == TALK_MODE) //you can select an actor with 't' or enter.
			{
				if(talk())
					scroll->set_talking(true);
				mode = MOVE_MODE;
				map_window->set_show_cursor(false);
			}
			else
			{
				mode = TALK_MODE;
				scroll->display_string("Talk-");
				map_window->centerCursor();
				map_window->set_show_cursor(true);
			}
			break;
		case SDLK_u     :
			mode = USE_MODE;
			scroll->display_string("Use-");
			map_window->centerCursor();
			map_window->set_show_use_cursor(true);
			break;
		case SDLK_g     :
			mode = GET_MODE;
			scroll->display_string("Get-");
			map_window->centerCursor();
			map_window->set_show_use_cursor(true);
			break;
		case SDLK_RETURN  :
		case SDLK_KP_ENTER   :
			map_window->set_show_use_cursor(false);
			map_window->set_show_cursor(false);
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
				use(0,0);
			}
			else if(mode == GET_MODE)
			{
				mode = MOVE_MODE;
				get(0,0);
			}
			else if(mode == USESELECT_MODE || mode == FREESELECT_MODE)
			{
				mode = MOVE_MODE;
				select_obj();
			}
			else
			{
				scroll->display_string("what?\n\n");
				scroll->display_prompt();
			}
			break;
		case SDLK_ESCAPE:
			if(mode == MOVE_MODE)
			{
				scroll->display_string("Pass!\n\n");
				scroll->display_prompt();
				player->pass();
			}
			else
			{
				mode = MOVE_MODE;
				map_window->set_show_use_cursor(false);
				map_window->set_show_cursor(false);
				scroll->display_string("what?\n\n");
				scroll->display_prompt();
			}
                        map_window->updateBlacking();
			break;
		case SDLK_SPACE :
			scroll->display_string("Pass!\n\n");
			scroll->display_prompt();
			player->pass();
                        map_window->updateBlacking();
			break;
		default :
			if (event->key.keysym.sym != SDLK_LALT
				&& event->key.keysym.sym != SDLK_RALT)
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

	if(active_alt_code && scroll->get_input())
		alt_code_input(scroll->get_input());
	
	return true;
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
    // load and begin npc script
    if(converse->start(npc))
    {
        // print npc name if met-flag is set, or npc is in avatar's party
        name = converse->npc_name(id); // get name
        if(name &&
           ((npc->get_flags() & 1) || player->get_party()->contains_actor(npc)))
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
    scroll->display_string(map_window->lookAtCursor(false));
    scroll->display_string("\nFunny, no response.\n\n");
    scroll->display_prompt();
    return(false);
}


bool Event::move(sint16 rel_x, sint16 rel_y)
{
 if(mode == LOOK_MODE || mode == TALK_MODE || mode == FREESELECT_MODE)
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
   else
     {
      if(mode == GET_MODE)
        {
         get(rel_x,rel_y);
        }
      else
        {
         player->moveRelative(rel_x, rel_y);
        }
     }
  }

 return true;
}


bool Event::get(sint16 rel_x, sint16 rel_y)
{
 Obj *obj;
 uint16 x,y;
 uint8 level;
 float weight;
 
 player->get_location(&x,&y,&level);

 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 if(obj)
 {
  scroll->display_string(obj_manager->look_obj(obj));

  // objects with 0 weight aren't gettable. 
  weight = obj_manager->get_obj_weight(obj, OBJ_WEIGHT_EXCLUDE_CONTAINER_ITEMS);
  if(weight != 0)
    {
       Actor *pc = player->get_actor();
       if(pc->can_carry_weight(weight))
         {
          // object is someone else's
          if(!(obj->status & OBJ_STATUS_OK_TO_TAKE))
            {
             scroll->display_string("\n\nStealing!!!"); // or "Stop Thief!!!"
             player->subtract_karma();
             obj->status |= OBJ_STATUS_OK_TO_TAKE; // move to DROP
            }

          obj_manager->remove_obj(obj); //remove object from map.

          if(obj->qty > 0) // stackable object; will make copy and delete this
            {
             pc->inventory_new_object(obj->obj_n, obj->qty, obj->quality);
             obj_manager->delete_obj(obj); // original copy defunct
            }
          else
             pc->inventory_add_object(obj); // just stick this one in there
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

 map_window->set_show_use_cursor(false);
 map_window->updateBlacking();
 view_manager->update(); //redraw views to show new item.
 mode = MOVE_MODE;

 return true;
}

bool Event::use(sint16 rel_x, sint16 rel_y)
{
 Actor *actor = NULL;
 Obj *obj = NULL;
 Map *map = Game::get_game()->get_game_map();
 uint16 x,y;
 uint8 level;
 bool using_actor = false;

 player->get_location(&x,&y,&level);
 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 actor = map->get_actor((uint16)(x+rel_x), (uint16)(y+rel_y), level);
 if(!obj && actor && actor->is_visible())
   {
    obj = actor->make_obj();
    using_actor = true;
   }

 if(obj)
 {
  bool can_use = usecode->has_usecode(obj);
  if(!using_actor || can_use)
  {
   scroll->display_string(obj_manager->look_obj(obj));
   scroll->display_string("\n");
  }
  if(can_use)
   usecode->use_obj(obj, player->get_actor());
  else
  {
   if(using_actor)
      scroll->display_string("nothing\n");
   else
      scroll->display_string("\nNot usable\n");
   fprintf(stderr, "Object %d:%d\n", obj->obj_n, obj->frame_n);
  }
 }
 else
  scroll->display_string("nothing\n");

 if(mode == USE_MODE) // if selecting, select_obj will return to MOVE_MODE
 {
     scroll->display_string("\n");
     scroll->display_prompt();
     map_window->set_show_use_cursor(false);
     mode = MOVE_MODE;
 }
 map_window->updateBlacking();
 
 if(using_actor) //we were using an actor so free the temp Obj
  obj_manager->delete_obj(obj);

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
    map_window->set_show_use_cursor(false);
    map_window->set_show_cursor(false);
    mode = MOVE_MODE;
    
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


bool Event::look()
{
 bool can_search = true; // can object be searched? return from LOOK
 Obj *obj = map_window->get_objAtCursor();
 Actor *actor = map_window->get_actorAtCursor();
 sint16 p_id = -1; // party member number of actor
 char weight_string[26];
 float weight;

 if(actor)
 {
   view_manager->set_portrait_mode(actor->get_actor_num(),NULL);
   can_search = false;
 }
 scroll->display_string("Thou dost see ");
 if(actor)
   {
    Game::get_game()->get_actor_manager()->print_actor(actor); //DEBUG
    // show real actor name and portrait if in avatar's party
    if((p_id = player->get_party()->get_member_num(actor)) >= 0)
       scroll->display_string(player->get_party()->get_actor_name(p_id));
    else
       scroll->display_string(map_window->lookAtCursor());
   }
 else if(obj) // don't display weight or do usecode for obj under actor
  {
   obj_manager->print_obj(obj,false); //DEBUG
   scroll->display_string(map_window->lookAtCursor());

   weight = obj_manager->get_obj_weight(obj);
   if(weight != 0)
     {
      snprintf(weight_string,25,". It weighs %0.1f stones.",obj_manager->get_obj_weight(obj));
      scroll->display_string(weight_string);
     }
  // check for special description
  if(usecode->has_lookcode(obj))
     can_search = usecode->look_obj(obj, player->get_actor());
  }
 else // ground
   scroll->display_string(map_window->lookAtCursor());
 scroll->display_string("\n");
 // search
 MapCoord player_loc = player->get_actor()->get_location(),
          target_loc = map_window->get_cursorCoord();
 if(can_search && player_loc.distance(target_loc) <= 1)
   {
    scroll->display_string("\nSearching here, you find ");
    if(!obj || !usecode->search_obj(obj, player->get_actor()))
       scroll->display_string("nothing.\n");
    else
      {
       scroll->display_string(".\n");
       map_window->updateBlacking(); // secret doors
      }
   }
 scroll->display_string("\n");
 scroll->display_prompt();
 return true;
}


/* Send input to active alt-code.
 */
void Event::alt_code_input(const char *in)
{
    switch(active_alt_code)
    {
        case 400: // talk to NPC (FIXME: get portrait and inventory too)
            if(!converse->start((uint8)strtol(in, NULL, 10)))
            {
                scroll->display_string("\n");
                scroll->display_prompt();
            }
            active_alt_code = 0;
            break;

        case 214:
            alt_code_teleport(in); //teleport player & party? to location string
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 500: // control/watch anyone
            ActorManager *am = Game::get_game()->get_actor_manager();
            Actor *a = am->get_actor((uint8)strtol(in, NULL, 10));
            player->set_actor(a);
            map_window->centerMapOnActor(a);
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
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
        case 400: // talk to anyone (FIXME: get portrait and inventory too)
            scroll->display_string("Npc number? ");
            scroll->set_input_mode(true);
            active_alt_code = c;
            break;

        case 500: // control/watch anyone
            scroll->display_string("Npc number? ");
            scroll->set_input_mode(true);
            active_alt_code = c;
            break;

        case 213:
            alt_code_infostring();
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 214:
            scroll->display_string("Location: \n",2);
            scroll->display_string(" ",0);
            scroll->set_input_mode(true);
            active_alt_code = c;
            break;

        case 215:
            clock->advance_to_next_hour();
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
            map_window->updateBlacking();
            active_alt_code = 0;
            break;
    }
}

bool Event::alt_code_teleport(const char *location_string)
{
 char *next_num;
 uint16 x, y, z;

 x = strtol(location_string,&next_num,16);
 y = strtol(next_num,&next_num,16);
 z = strtol(next_num,&next_num,16);
 player->move(x,y,z);

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

void Event::wait()
{
 SDL_Delay(TimeLeft());
}

//Protected

inline Uint32 Event::TimeLeft()
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks(); // FIXME: get from Game or GameClock
    if ( next_time <= now ) {
        next_time = now+NUVIE_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

void Event::quitDialog()
{
 GUI_Widget *area_widget;
 GUI_Widget *widget;
 
 area_widget = (GUI_Widget *) new GUI_Area(75, 60, 170, 80, 212, 208, 131, 72, 69, 29, 2, AREA_ANGULAR);
 widget = (GUI_Widget *) new GUI_Button(area_widget, 100, 50, 40, 18, "Yes", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, quitDialogYesCallback, 0);
 area_widget->AddWidget(widget);

 widget = (GUI_Widget *) new GUI_Button(area_widget, 30, 50, 40, 18, "No", gui->get_font(), BUTTON_TEXTALIGN_CENTER, 0, quitDialogNoCallback, 0);
 area_widget->AddWidget(widget);
 
 widget = (GUI_Widget *) new GUI_Text(10, 25, 0, 0, 0, "Do you want to Quit", gui->get_font());
 area_widget->AddWidget(widget);
   
 gui->AddWidget(area_widget);

 return;
}

static GUI_status quitDialogYesCallback(void *data)
{
 GUI_Widget *widget;
 
 widget = (GUI_Widget *)data;
 
 //widget->Delete();
 
 showingQuitDialog = false;
 
 return GUI_QUIT;
}

static GUI_status quitDialogNoCallback(void *data)
{
 GUI_Widget *widget;
 
 widget = (GUI_Widget *)data;
 
 widget->Delete();

 showingQuitDialog = false;
 
 return GUI_YUM;
}
