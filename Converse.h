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
#include <stack>
#include "Actor.h"
#include "U6Lib_n.h"
#include "Configuration.h"
#include "MsgScroll.h"

enum Converse_interpreter {CONV_U6 = 0, CONV_MD, CONV_SE};


/* Control codes for U6; as far as I know work with the other games too... */
#define U6OP_IF 0xa1
#define U6OP_ENDIF 0xa2
#define U6OP_ELSEIF 0xa3
#define U6OP_SETF 0xa4
#define U6OP_CLEARF 0xa5
#define U6OP_ARGSTOP 0xa7
#define U6OP_JUMP 0xb0
#define U6OP_BYE 0xb6
#define U6OP_WAIT 0xcb
#define U6OP_UINT8 0xd3
#define U6OP_UINT16 0xd4
#define U6OP_UINT32 0xd2
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
    Uint8 valt; // value type
                // 0x00 = integer constant
                // 0xb2 = numbered variable
    Uint32 val;
} converse_arg;

class Converse
{
    Configuration *config;
    Converse_interpreter interpreter;
    Actor *npc;
    MsgScroll *scroll; // i/o object
    U6Lib_n *src; // points to opened converse.[ab] library
    Uint32 src_num; // 0=none 1="converse.a" 2="converse.b"

    unsigned char *script;
    Uint32 script_len;
    unsigned char *script_pt; // points to next command/string in script
    bool active; // running npc script? (paused or unpaused)
    bool is_waiting; // paused, waiting for user input?
    string output; // where text goes to be printed

    // statement parameters
    uint32 cmd; // previously read command code
    // fixed # of args with variable # values each
    vector <vector<converse_arg> > args;
    string input_s; // last input from player
    string keywords; // keyword list from last KEYWORD statement
#define CONV_TEXTOP_PRINT 0
#define CONV_TEXTOP_KEYWORD 1
#define CONV_TEXTOP_LOOK 2
    uint8 text_op; // one of CONV_TEXTOP_[012]
#define CONV_SCOPE_MAIN 0
#define CONV_SCOPE_ASK 1 // break at endask
#define CONV_SCOPE_ANSWER 2 // break at keywords or endask
#define CONV_SCOPE_IF 3 // at else change to endif, at endif break
#define CONV_SCOPE_IFELSE 4 // at else change to if, at endif break
#define CONV_SCOPE_IFEND 5 // at endif break
    std::stack <uint8> scope; // what "scope" is the script in?

    /* Check that loaded converse library (the source) contains `script_num'.
     * Load another source if it doesn't, and update `script_num' to item number.
     */
    void set_conv(Uint32 &script_num)
    {
        if(script_num <= 98 && src_num == 2)
            loadConv("converse.a");
        else if(script_num >= 99)
        {
            script_num -= 99;
            if(src_num == 1)
                loadConv("converse.b");
        }
    }

    bool do_cmd(); // do cmd with args and clear cmd and args
    void collect_args();
    bool do_text();

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
    /* Returns true if the script pointer is greater than the length of the
     * script.
     */
    bool check_overflow(Uint32 ptadd = 0)
    {
       return(((script_pt + ptadd) > (script + script_len)));
    }

    /* Returns true if byte is a value-size definition. */
    bool is_valsize(Uint8 valcpy)
    {
        return((valcpy == 0xd3 || valcpy == 0xd2 || valcpy == 0xd4));
    }
    /* Returns true if `check' can be part of a text string. */
    bool is_print(Uint8 check)
    {
        return(((check == 0x0a) || (check >= 0x20 && check <=0x7a)));
    }
    /* Returns true if the control code starts a statement (is the command). */
    bool is_cmd(Uint8 code)
    {
        if(code >= 0xa1 && !is_valsize(code))
            return(true);
        return(false);
    }
#if 0
    Uint32 get_val(Uint8 argi, Uint vali)
    {
//        if(args[argi][vali].valt == 0xb2)
//            get variable value
        return(args[argi][vali].val);
    }
#endif
    bool check_keywords();
    /* Returns true if the current scope allows the current command to execute.
     */
    bool check_scope()
    {
        if(scope.empty())
            return(true);
        if(scope.top() == CONV_SCOPE_ASK
           && (cmd != U6OP_KEYWORD && cmd != U6OP_SANSWER && cmd != U6OP_ENDASK))
        {
            return(false);
        }
//        if(scope.top() == 
        return(true);
    }
public:
    Converse(Configuration *cfg, Converse_interpreter engine_type, MsgScroll *ioobj);
    ~Converse();
    void loadConv(std::string convfilename="converse.a");
    /* Returns true if a script is active (paused or unpaused). */
    bool running() { return(active); }
    /* Returns true if the script is waiting for input. */
    bool waiting() { return(is_waiting); }
    /* Tell script to pause. */
    void wait() { is_waiting = true; }
    /* Tell script to resume. */
    void unwait() { is_waiting = false; }
    bool start(Actor *talkto); // makes step() available
    void stop();
    void step(Uint32 count = 0);
    /* Send output buffer to message scroller. */
    void print() { scroll->display_string((char *)output.c_str()); output.resize(0); }
    Uint32 print_name();
    /* Set input buffer (replacing what is already there.) */
    void input(const char *new_input)
    {
        input_s.assign(new_input);
    }
    void continue_script();
};


#endif /* CONVERSE_H */
