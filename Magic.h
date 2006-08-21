#ifndef __Magic_h__
#define __Magic_h__

/*
 *  Magic.h
 *  Nuvie
 *
 *  Created by Pieter Luteijn on Mon Nov 15 2004,
 *  Copyright (c) 2004. All rights reserved.
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

#include "Event.h"

#define REAGENT_U6_MANDRAKE_ROOT 0x01
#define REAGENT_U6_NIGHTSHADE    0x02
#define REAGENT_U6_BLACK_PEARL   0x04
#define REAGENT_U6_BLOOD_MOSS    0x08
#define REAGENT_U6_SPIDER_SILK   0x10
#define REAGENT_U6_GARLIC        0x20
#define REAGENT_U6_GINSENG       0x40
#define REAGENT_U6_SULFUROUS_ASH 0x80

#define MAX_SCRIPT_LENGTH 65000
#define MAX_TOKEN_LENGTH 255 
#define MAX_STACK_DEPTH 255 

#define MAGIC_STATE_READY 	   0x00
#define MAGIC_STATE_SELECT_SPELL   0x01
#define MAGIC_STATE_PROCESS_SCRIPT 0x02
#define MAGIC_STATE_ACQUIRE_TARGET 0x03
#define MAGIC_STATE_ACQUIRE_INPUT  0x04

class Stack {
  private:
    char token[MAX_TOKEN_LENGTH+1];
    size_t SP;
    char *element[MAX_STACK_DEPTH];
  public:
    Stack(){SP=0;}
    void empty();
    void dump();
    void push(const char*);
    void pushptr(const void*);
    void* popptr();
    char* pop();
    void drop();
    void dup();
    void swap();
    void over();
    void rot();
     
    ~Stack(){this->empty();}
};


class Spell {
  public: /* saves using dumb get / set functions */
	char * name; // eg. "Heal" 
	char * invocation; // eg. "im" 
	char * script; // what functions the spell should call
	uint8 reagents; // reagents used
	
	Spell(const char *new_name="undefined name",const char *new_invocation="kawo", const char *new_script="0;return", uint8 new_reagents=255) {
	  name=strdup(new_name);
	  invocation=strdup(new_invocation);
	  script=strdup(new_script);
	  reagents=new_reagents;
	};
	~Spell(){
	  free(name);
	  free(invocation);
	  free(script);
	}
};

class Magic {
  private:
    Spell *spell[256]; // spell list;
    char cast_buffer_str[26]; // buffer for spell syllables typed.
    uint8 cast_buffer_len; // how many characters typed in the spell buffer.
    Stack *stack;
    Event *event;
    Actor *target_actor;
    Obj *target_object;
    uint8 state;
    size_t IP;
    /* TODO
     * add a register array, or a pointer to a list of variables?
     */
  public: 
    Magic();
    ~Magic();
    void init(Event *evt);
    void lc(char * str); 
    bool call(char *function);
    bool jump(char *label,size_t * OIP, char * script);

    bool read_line(NuvieIOFileRead *file, char *buf, size_t maxlen);

    bool process_script(char * a_script, bool from_start=true);
    bool next_token(char *token, size_t *IP, char *script);
    void read_spell_list();
    void clear_cast_buffer() { cast_buffer_str[0] = '\0'; cast_buffer_len = 0; }
    bool start_new_spell();
    uint8 book_equipped();
    bool cast();
    bool cast(Actor *Act);
    bool cast(Obj *Obj);
    bool handleSDL_KEYDOWN(const SDL_Event *sdl_event);

    // Support functions for spells (generated with fhdr.sh)
bool function_add();
bool function_add_hp();
bool function_add_mp();
bool function_colon();
bool function_define_spell();
bool function_delete_obj() ;
bool function_div();
bool function_display();
bool function_drop();
bool function_dup();
bool function_give_obj();
bool function_input();
bool function_inv_to_spell();
bool function_join();
bool function_load();
bool function_gt();
bool function_mod();
bool function_mul();
bool function_new_obj();
bool function_newline();
bool function_null();
bool function_over();
bool function_random();
bool function_save_spell();
bool function_space();
bool function_strcmp();
bool function_string2npc();
bool function_get_actor_attrib();
bool function_sub();
bool function_swap();
bool function_template();
bool function_underscore();
bool function_eclipse();
bool function_quake();
};


#endif /* __Magic_h__ */
