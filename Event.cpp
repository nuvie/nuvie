/*
 *  Event.cpp
 *  Nuive
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


#include "U6def.h"
#include "Configuration.h"

#include "MapWindow.h"
#include "Player.h"
#include "Book.h"
#include "Event.h"

#include "U6UseCode.h"

uint32 nuvieGameCounter;

Event::Event(Configuration *cfg)
{
 config = cfg;
 clear_alt_code();
 active_alt_code = 0;
 alt_code_input_num = 0;
}

Event::~Event()
{
 delete book;
}

bool Event::init(ObjManager *om, MapWindow *mw, MsgScroll *ms, Player *p,
                 GameClock *gc, Converse *c, ViewManager *vm, UseCode *uc)
{
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

 return true;
}

bool Event::update()
{
  while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYUP:
                                        if(event.key.keysym.sym == SDLK_RALT
                                           || event.key.keysym.sym == SDLK_LALT)
                                        {
                                            clear_alt_code();
                                        }
                                        break;
				case SDL_KEYDOWN:
           if(scroll->handle_input(&event.key.keysym))
             break; // break switch
           // alt-code input
           if((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)
              && (SDL_GetModState() & KMOD_ALT))
           {
               alt_code_str[alt_code_len++] = (char)event.key.keysym.sym;
               alt_code_str[alt_code_len] = '\0';
               if(alt_code_len == 3)
               {
                   alt_code(alt_code_str);
                   clear_alt_code();
               }
               break;
           }
           switch(event.key.keysym.sym)
            {
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
             case SDLK_q     :
                               return false;
                               break;
             case SDLK_l     :
                               mode = LOOK_MODE;
                               scroll->display_string("Look-");
                               map_window->centerCursor();
                               map_window->set_show_cursor(true);
                               break;
             case SDLK_p     :
                               scroll->display_string("\nYou say:");
                               scroll->set_input_mode(true);
                               break;
             case SDLK_t     :
                               mode = TALK_MODE;
                               scroll->display_string("Talk-");
                               map_window->centerCursor();
                               map_window->set_show_cursor(true);
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
                               if(mode == LOOK_MODE)
                                 {
                                  mode = MOVE_MODE;
                                  look();
                                  map_window->set_show_cursor(false);
                                 }
                               else if(mode == TALK_MODE)
                                 {
                                  if(talk())
                                    scroll->set_talking(true);
                                  mode = MOVE_MODE;
                                  map_window->set_show_cursor(false);
                                 }
                               else if(mode == USE_MODE)
                                 {
                                  mode = MOVE_MODE;
                                  use(0,0);
                                  map_window->set_show_use_cursor(false);
                                 }
                               else if(mode == GET_MODE)
                                 {
                                  mode = MOVE_MODE;
                                  get(0,0);
                                  map_window->set_show_cursor(false);
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
                                  clock->inc_move_counter_by_a_minute();
                                 }
                               else
                                 {
                                  mode = MOVE_MODE;
                                  map_window->set_show_use_cursor(false);
                                  map_window->set_show_cursor(false);
                                  scroll->display_string("what?\n\n");
                                  scroll->display_prompt();
                                 }
                               break;
             case SDLK_SPACE :
                               scroll->display_string("Pass!\n\n");
                               scroll->display_prompt();
                               break;
             default :
                               if(event.key.keysym.sym != SDLK_LALT
                                  && event.key.keysym.sym != SDLK_RALT)
                               {
                                scroll->display_string("what?\n\n");
                                scroll->display_prompt();
                               }
                               break;
            }
          break;

        case SDL_QUIT :
                       return false; //time to quit.
                       break;
        default:
        break;
			}
		}
    if(active_alt_code && scroll->get_input())
        alt_code_input(scroll->get_input());
 return true;
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
 if(mode == LOOK_MODE || mode == TALK_MODE)
    map_window->moveCursorRelative(rel_x,rel_y);
 else
  {
   if(mode == USE_MODE)
     {
      use(rel_x,rel_y);
     }
   else
     {
      if(mode == GET_MODE)
        {
         get(rel_x,rel_y);
        }
      else
         player->moveRelative(rel_x,rel_y);
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

  weight = obj_manager->get_obj_weight(obj);
  weight /= 10;
  
  if(weight != 0)
    {
       Actor *pc = player->get_actor();
       float total = (weight*(obj->qty?obj->qty:1)) + pc->get_inventory_weight();
       float max = pc->inventory_get_max_weight();
       if(total <= pc->inventory_get_max_weight())
         {
           pc->inventory_add_object(obj->obj_n, obj->qty, obj->quality);
           obj_manager->remove_obj(obj);
         }
      else
         scroll->display_string("\n\nThe total is too heavy.");
    }
  else
    scroll->display_string("\n\nNot possible.");
    
 }
 else
  scroll->display_string("nothing");

 scroll->display_string("\n");
 scroll->display_prompt();

 map_window->set_show_use_cursor(false);
 map_window->updateBlacking();
 view_manager->update_display();
 mode = MOVE_MODE;

 return true;
}

bool Event::use(sint16 rel_x, sint16 rel_y)
{
 Obj *obj;
 uint16 x,y;
 uint8 level;

 player->get_location(&x,&y,&level);


 obj = obj_manager->get_obj((uint16)(x+rel_x), (uint16)(y+rel_y), level);

 if(obj)
 {
  scroll->display_string(obj_manager->look_obj(obj));
  scroll->display_string("\n");

  usecode->use_obj(obj);
 }

 scroll->display_string("\n");
 scroll->display_prompt();

 map_window->set_show_use_cursor(false);
 map_window->updateBlacking();
 mode = MOVE_MODE;

 return true;
}


bool Event::look()
{
 Obj *obj;
 Actor *actor = map_window->get_actorAtCursor();
 sint16 p_id = -1; // party member number of actor
 char *data;
 char weight_string[26];
 float weight;

 if(actor)
   view_manager->set_portrait_mode(actor->get_actor_num(),NULL);

 scroll->display_string("Thou dost see ");
 // show real actor name and portrait if in avatar's party
 if(actor && ((p_id = player->get_party()->get_member_num(actor)) >= 0))
     scroll->display_string(player->get_party()->get_actor_name(p_id));
 else
     scroll->display_string(map_window->lookAtCursor());
 obj = map_window->get_objAtCursor();
 if(obj)
  {
   weight = obj_manager->get_obj_weight(obj);
   if(weight != 0)
     {
      snprintf(weight_string,25,". It weighs %0.1f stones.",obj_manager->get_obj_weight(obj) / 10.0);
      scroll->display_string(weight_string);
     }

   if(obj->obj_n == OBJ_U6_SIGN || obj->obj_n == OBJ_U6_BOOK ||
      obj->obj_n == OBJ_U6_SCROLL || obj->obj_n == OBJ_U6_PICTURE ||
      obj->obj_n == OBJ_U6_TOMBSTONE || obj->obj_n == OBJ_U6_CROSS)
      {
       if(obj->quality != 0)
         {
          scroll->display_string(":\n\n");
          data = book->get_book_data(obj->quality-1);
          scroll->display_string(data);
          free(data);
         }
      }
  }

 scroll->display_string("\n\n");
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

        case 213:
            alt_code_infostring();
            scroll->display_string("\n");
            scroll->display_prompt();
            active_alt_code = 0;
            break;

        case 214:
            scroll->display_string("Location: ");
            scroll->set_input_mode(true);
            active_alt_code = c;
            break;

        case 215:
            clock->advance_to_next_hour();
            scroll->display_string(clock->get_time_string());
            scroll->display_string("\n");
            scroll->display_prompt();
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

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+NUVIE_INTERVAL;
        return(0);
    }
    return(next_time-now);
}
