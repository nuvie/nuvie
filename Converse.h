#ifndef CONVERSE_H
#define CONVERSE_H
/* Copyright (C) 2003 Joseph Applegate
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

#include <iostream>
#include <cstring>
#include "Actor.h"
#include "U6Lib_n.h"
#include "Configuration.h"

enum Converse_interpreter {CONV_U6 = 0, CONV_MD, CONV_SE};


/* Control codes for U6, as far as I know work with the others too... */
#define U6OP_SETF 0xa4
#define U6OP_CLEARF 0xa5
#define U6OP_JUMP 0xb0
#define U6OP_BYE 0xb6
#define U6OP_WAIT 0xcb
#define U6OP_ENDASK 0xee
#define U6OP_KEYWORD 0xef
#define U6OP_SIDENT 0xff
#define U6OP_SLOOK 0xf1
#define U6OP_SLOOKB 0xf3
#define U6OP_SCONVERSE 0xf2
#define U6OP_SANSWER 0xf6
#define U6OP_SASK 0xf7

typedef struct
{
    Uint32 val_c; // number of values
    vector<Uint32> val_t; // type/size of each value (from value list)
                          // 0xd4 = 32bit integer
                          // 0xd3 = 8bit integer
    vector<Uint32> val;
} converse_arg;

class Converse
{
    Configuration *config;
    Converse_interpreter interpreter;
    U6Lib_n *src; // points to opened converse.[ab] library
    Actor *npc;
    unsigned char *script;
    Uint32 script_len;
    unsigned char *script_pt; // points to next command/string in script
    bool active; // running npc script? (paused or unpaused)
    bool is_waiting; // paused, waiting for user input?
    Uint32 prompt; // 0 = not waiting
                   // 1 = just waiting for any input to continue
                   // 2 = need input character or string, ENTER, or ESC
    char *allowed_input; // NULL if any input is allowed
    string output; // where text goes until something grabs it
    char *output_cpy; // "safe" buffer of text until checked again
    Uint32 cmd, skip_to_cmd;
    string input_s; // last input from player
    string keywords; // keyword list from last KEYWORD statement

    bool do_cmd(); // do cmd with args and clear cmd and args
    bool do_text(Uint32 text_len);

    /* Seeking methods - update script pointer. */
    void seek(Uint32 offset = 0) { script_pt = script; script_pt += offset; }
    void seek_look();
    void seek_converse();
    void seek_byte(Uint8 val)
    {
        for(script_pt = script; !this->check_overflow(); script_pt++)
            if(pop() == val)
                return;
        script_pt = script;
    }
    void skip(Uint32 bytes = 1) { pop(bytes); }

    /* Read methods - Return a number of bytes and update script pointer. */
    Uint8 pop(Uint32 bytes = 1)
    {
        Uint8 val = 0;
        while(bytes--)
        {
            val = *script_pt;
            ++script_pt;
        }
        return(val);
    }
    Uint16 pop2()
    {
        Uint16 val = 0;
        memcpy(&val, script_pt, 2);
        script_pt += 2;
        return(val);
    }
    Uint32 pop4()
    {
        Uint32 val = 0;
        memcpy(&val, script_pt, 4);
        script_pt += 4;
        return(val);
    }
    Uint8 peek(Uint32 peekahead = 0) { return((Uint8)*(script_pt+peekahead)); }

    /* Returns true if byte is a value-size definition. */
    bool is_valsize(Uint8 valcpy)
    {
        return((valcpy == 0xd3 || valcpy == 0xd2 || valcpy == 0xd4));
    }
    /* Returns true if the script pointer is greater than the length of the
     * script.
     */
    bool check_overflow(Uint32 ptadd = 0)
    {
       return(((script_pt + ptadd) > (script + script_len)));
    }
public:
    Converse(Configuration *cfg, Converse_interpreter engine_type);
    ~Converse()
    {
        // FIXME: free and delete heap .. oops.. one doesn't exist yet :P
        if(active)
            this->stop();
    }
    void loadConv();
    /* Returns true if a script is active (paused or unpaused). */
    bool running() { return(active); }
    /* Returns 0 if the script is not waiting for input, or
        1 if any key will continue the script,
        2 if some character/string input is needed.
    */
    Uint32 waiting()
    {
        if(!is_waiting)
            return(0);
        return(prompt);
    }
    /* Tell script to pause. */
    void wait() { is_waiting = true; prompt = 1; }
    /* Tell script to resume. */
    void unwait() { is_waiting = false; prompt = 0; }
    bool start(Actor *talkto); // makes step() available
    void stop();
    void step(Uint32 count = 0);
    /* Returns text to print or NULL if no text is available. **The string
     * pointer returned becomes invalid when get_output() is called again.**
     */
    char *get_output()
    {
        output_cpy = (char *)realloc(output_cpy, output.size());
        if(output_cpy)
            strcpy(output_cpy, (char *)output.c_str());
        output.resize(0);
        fprintf(stderr, "Converse: output=\"%s\"\n", output_cpy);
        return(output_cpy);
    }
    /* Return true if there is output text ready to be printed. */
    bool has_output() { return(!output.empty()); }
    Uint32 print_name();
    /* Set input buffer (replacing what is already there.) */
    void input(const char *new_input)
    {
        input_s.assign(new_input);
    }
};


#endif /* CONVERSE_H */
