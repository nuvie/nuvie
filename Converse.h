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
    string output; // where text goes until something grabs it
    char *output_cpy; // "safe" buffer of text until get_text() calls again
    Uint32 cmd;
    vector <converse_arg> args;
    Uint32 section; // 0 = look, 1 = converse
    bool do_cmd(); // do cmd with args and clear cmd and args
    bool do_text(Uint32 text_len);
    unsigned char *seek_section(Uint32 section_id);
    /* Get a Converse command-argument value from `loc' and place it at the
     * requested argument and value index.
     * Returns the number of bytes copied.
     */
    Uint32 readval(Uint32 arg_i, Uint32 val_i, unsigned char *loc)
    {
        // values are written as: 00 b2 or d3 00 or d2 00000000
        if(is_valsize(*script_pt))
        {
            args[arg_i].val_t[val_i] = *script_pt;
            switch(args[arg_i].val_t[val_i])
            {
                case 0xd3:
                    memcpy(&args[arg_i].val[val_i], script_pt + 1, 1);
                    return(2);
                case 0xd2:
                    memcpy(&args[arg_i].val[val_i], script_pt + 1, 4);
                    return(5);
            }
            args[arg_i].val_t[val_i] = *script_pt;
        }
        else
        {
            args[arg_i].val[val_i] = *script_pt;
            args[arg_i].val_t[val_i] = *(script_pt + 1);
        }
        return(2);
    }
    /* Returns true if byte is a value-size definition. */
    bool is_valsize(Uint8 valcpy)
    {
        return((valcpy == 0xd3 || valcpy == 0xd2 || valcpy == 0xd4));
    }
    /* Returns true if the address `pt' is greater than the length of the
     * script.
     */
    bool check_overflow(unsigned char *pt)
    {
       return((pt > (script + script_len)));
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
    /* Returns true if script is waiting for input. */
    bool waiting() { return(is_waiting); }
    /* Tell script to pause. */
    void wait() { is_waiting = true; }
    /* Tell script to resume. */
    void unwait() { is_waiting = false; }
    bool start(Actor *talkto); // makes step() available
    void stop();
    void step(Uint32 count = 0);
    /* Returns text to print or NULL if no text is available. **The string
     * pointer returned becomes invalid when get_text() is called again.**
     */
    char *get_output()
    {
        output_cpy = (char *)realloc(output_cpy, output.size());
        if(output_cpy)
            strcpy(output_cpy, (char *)output.c_str());
        output.resize(0);
        return(output_cpy);
    }
    /* Return true if there is output text ready to be printed. */
    bool has_output() { return(!output.empty()); }
    void print_name();
};

// argument counts (256args * 256values)
static Uint8 converse_cmd_args[256];
// for each command, 1 if can accept multiple values in 0xa7 terminated args
static Uint8 converse_cmd_multival[256];

#endif /* CONVERSE_H */
