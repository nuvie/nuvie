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

#include <SDL.h>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stack>

#include "ActorManager.h"
#include "Actor.h"
#include "U6Lib_n.h"
#include "Configuration.h"
#include "MsgScroll.h"
#include "GameClock.h"
#include "Player.h"
#include "U6def.h"
#include "ViewManager.h"

using std::stack;

enum Converse_interpreter {CONV_U6 = 0, CONV_MD, CONV_SE};

/* Control codes for U6; as far as I know work with the other games too... */
#define U6OP_IF 0xa1
#define U6OP_ENDIF 0xa2
#define U6OP_ELSE 0xa3
#define U6OP_SETF 0xa4 // set flag
#define U6OP_CLEARF 0xa5 // clear flag
#define U6OP_DECL 0xa6 // declare variable, to be initialized with ASSIGN
#define U6OP_ARGSTOP 0xa7 // optional argument terminator
#define U6OP_ASSIGN 0xa8 // assign value(s) with optional math to declared var.
#define U6OP_JUMP 0xb0 // go to location (absolute, from start of script)
#define U6OP_BYE 0xb6 // end conversation
#define U6OP_NEW 0xb9 // new object, place in an npc's inventory
#define U6OP_DELETE 0xba // delete an object
#define U6OP_INVENTORY 0xbe // show inventory
#define U6OP_PORTRAIT 0xbf // show portrait
#define U6OP_GIVE 0xc9 // transfer object from one npc inventory to another
#define U6OP_WAIT 0xcb // insert page-break/wait for input to continue
#define U6OP_WORKTYPE 0xcd // set worktype of npc
#define U6OP_UINT8 0xd3
#define U6OP_UINT16 0xd4
#define U6OP_UINT32 0xd2
#define U6OP_YASSIGN 0xd8 // set $Y to name of an npc
#define U6OP_ENDASK 0xee // end of response(s) to previous input
#define U6OP_KEYWORD 0xef // keyword(s) to trigger next response follows
#define U6OP_SIDENT 0xff // start of script; npc identification section
#define U6OP_SLOOK 0xf1 // start of look section
#define U6OP_SCONVERSE 0xf2 // start of main conversation section
#define U6OP_SPREFIX 0xf3 // unknown; can precede 0xf1 or 0xf2
#define U6OP_SANSWER 0xf6 // start of response
#define U6OP_ASK 0xf7 // get input string
#define U6OP_ASKC 0xf8 // get one character
#define U6OP_INPUT 0xfb // get number, store in variable as integer
#define U6OP_INPUTC 0xfc // get one character 0-9, store in variable as integer

#define CONV_SCOPE_MAIN 0
#define CONV_SCOPE_ASK 1 // break at keywords, answer, endask
#define CONV_SCOPE_ANSWER 2 // break at keywords or endask
#define CONV_SCOPE_IF 3 // at else change to endif, at endif break
#define CONV_SCOPE_IFELSE 4 // at else change to if, at endif break
#define CONV_SCOPE_ENDASK 5 // break at end-of-ask
#define CONV_SCOPE_SEEKLOOK 6 // do nothing until looksection, then break
#define CONV_SCOPE_SEEKIDENT 7 // after identsection change to SEEKLOOK
#define CONV_SCOPE_SEEKCONV 8 // do nothing until conversesection, then break

#define CONV_VAR_SEX 0x10 // sex of avatar: male=0 female=1
#define CONV_VAR_PARTYSIZE 0x17 // number of people (alive) in avatar's party
#define CONV_VAR_PARTYSIZET 0x18 // number of people (alive+dead) in party
#define CONV_VAR_WORKTYPE 0x20 // current activity of npc, from schedule
#define CONV_VAR__LAST_ 0x20

typedef struct
{
    Uint8 valt; // value type/modifier
                // 0x00 = value is an integer constant or something else
                // 0xb2 = value is number of a variable
                // 0xb3 = value is number of a character string
                // 0xd2,0xd3,0xd4 = value is an integer constant
    Uint32 val;
} converse_arg;
typedef unsigned char* convscript_p;
typedef struct
{
    convscript_p buf;
    uint32 buf_len;
} convscript;

class Converse
{
    Configuration *config;
    Converse_interpreter interpreter;
    GameClock *clock;
    ActorManager *actors;
    Actor *npc;
    Player *player;
    ViewManager *views;
    MsgScroll *scroll; // i/o object
    U6Lib_n *src; // points to opened converse.[ab] library
    Uint32 src_num; // 0=none 1="converse.a" 2="converse.b"

    convscript script;
    convscript_p script_pt; // points to next command/string in script
    Uint8 npc_num; // number of loaded npc script
    bool active; // running npc script? (paused or unpaused)
    bool is_waiting; // paused, waiting for user input?
    bool need_input; // wait is for text input, else for continuing after pause
    string output; // where text goes to be printed
    vector<converse_arg> heap; // random-access variable data
    vector<const char *> *strings; // string variables
    sint16 declared; // declared variable number, target of next assignment
    uint8 declared_t; // 0xb3 if declared variable is string, else it is integer
    const char *ystr; // value of $Y in text
    char *rstr; // result of expressions returning strings
    char my_name[16], // name of active NPC
         *my_desc, // description of active NPC
         aname[16]; // name of another NPC

    // statement parameters
    uint32 cmd; // previously read command code
    // fixed # of args with variable # values each
    vector<vector<converse_arg> > args;
    string input_s; // last input from player
    string keywords; // keyword list from last KEYWORD statement
    std::stack <uint8> scope; // what "scope" is the script in?
    Uint32 cmd_offset; // location of `cmd' in script
    Uint32 getinput_offset; // location of last input statement

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
    convscript read_script(Uint32 &n);
    void init_variables();
    void save_variables();

    bool do_cmd(); // do cmd with args and clear cmd and args
	// handling of the argument list
    void collect_args();
    void collect_text(bool eval);
    uint32 get_val(uint8 arg_i, sint8 val_i);
    /* Returns requested value from the argument list. The actual reference
     * number is returned in the case of a variable, not the stored value.
     */
    uint32 get_rval(uint8 arg_i, sint8 val_i)
    {
        if(val_i >= 0)
            return(args[arg_i][val_i].val);
        else // -1 = last value in argument
            return(args[arg_i][args[arg_i].size()-1].val);
    }
    /* Returns "type" of a value from the argument list.
     */
    uint32 get_valt(uint8 arg_i, sint8 val_i)
    {
        if(val_i >= 0)
            return(args[arg_i][val_i].valt);
        else // -1 = last value in argument
            return(args[arg_i][args[arg_i].size()-1].valt);
    }
    uint8 val_count(uint8 arg_i) { return(args[arg_i].size()); }
    /* Set contents of a certain value. */
    void set_val(uint8 arg_i, sint8 val_i, converse_arg &set)
    {
        args[arg_i][val_i].val = set.val;
        args[arg_i][val_i].valt = set.valt;
    }
    /* Returns collected text. */
    const char *get_text() { return(output.c_str()); }
    /* Return value stored at a normal variable. */
    uint32 get_var(uint8 varnum)
    {
        return(varnum <= CONV_VAR__LAST_ ? heap[varnum].val : 0x00);
    }
    /* Set the value of a normal variable. */
    void set_var(uint8 varnum, uint32 val, uint8 valt = 0x00)
    {
        if(varnum <= CONV_VAR__LAST_)
        {
            heap[varnum].valt = valt;
            heap[varnum].val = val;
        }
    }
    /* Return value stored at a string variable. */
    const char *get_svar(uint8 varnum)
    {
        if(!strings)
            return(NULL);
        if(varnum < strings->size())
            return((*strings)[varnum]);
        else
            return(NULL);
    }
    /* Set the value of a string variable. */
    void set_svar(uint8 varnum, const char *set)
    {
        if(!strings)
            strings = new vector<const char *>;
        if(varnum >= strings->size())
            strings->resize(varnum + 1);
        if((*strings)[varnum])
            free((char *)(*strings)[varnum]);
        (*strings)[varnum] = set ? strdup(set) : 0;
    }
    /* Declare(or clear) variable number and its type/modifier. */
    void let(sint16 n, uint8 t = 0) { declared = n; declared_t = t; }
    /* Set RStr (string result) value. It contains its own string. */
    void set_rstr(char *v) { if(rstr) free(rstr); rstr = v; }
    /* Set YStr (NPC Name) value. It points to a string contained elsewhere. */
    void set_ystr(const char *v) { ystr = v; }
    char *get_dbstr(uint32 loc, uint32 i);
    uint8 get_dbint(uint32 loc, uint32 i);
    /* Returns a randomly chosen number from `rnd_lo' to `rnd_hi'. */
    uint32 rnd(uint32 rnd_lo, uint32 rnd_hi)
    {
        if(rnd_lo == rnd_hi)
            return(rnd_lo);
        return((NUVIE_RAND() + rnd_lo) % (rnd_hi + 1));
    }
    uint32 u6op_if_test(uint32 cmpf, std::stack<uint32> *cmpv);
    uint32 u6op_assign_eval(uint32 opf, std::stack<uint32> *opv);
    void eval_arg(uint8 argn, bool test_first = false);

    /* Seeking methods - update script pointer. */
    void seek(Uint32 offset = 0) { script_pt = script.buf; script_pt += offset; }
    void seek_byte(Uint8 val)
    {
        for(script_pt = script.buf; !this->check_overflow(); script_pt++)
            if(pop() == val)
                return;
        script_pt = script.buf;
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
        val = *(script_pt++);
        val += *(script_pt++) << 8;
        return(val);
    }
    Uint32 pop4()
    {
        Uint32 val = 0;
        val = *(script_pt++);
        val += *(script_pt++) << 8;
        val += *(script_pt++) << 16;
        val += *(script_pt++) << 24;
        return(val);
    }
    Uint8 peek(Uint32 peekahead = 0) { return((Uint8)*(script_pt+peekahead)); }
    /* Returns true if the script pointer is greater than the length of the
     * script.
     */
    bool check_overflow(Uint32 ptadd = 0)
    {
       return(((script_pt + ptadd) > (script.buf + script.buf_len)));
    }

    /* Returns true if byte is a value-size definition. */
    bool is_valsize(Uint8 valcpy)
    {
        return((valcpy == 0xd3 || valcpy == 0xd2 || valcpy == 0xd4));
    }
    /* Returns true if byte is a value-type suffix. */
    bool is_valtype(Uint8 valcpy)
    {
        return((valcpy == 0xb2 || valcpy == 0xb3));
    }
    /* Returns true if `check' can be part of a text string. */
    bool is_print(Uint8 check)
    {
        return( ((check == 0x0a) || (check >= 0x20 && check <=0x7a)) );
    }
    /* Returns true if `check' is the number of a test or a special function
     * that is part of some statements. */
    bool is_test(Uint8 check)
    {
        return( ((check == 0x81) || (check == 0x82) || (check == 0x83)
                 || (check == 0x84) || (check == 0x85) || (check == 0x86)
                 || (check == 0x90) || (check == 0x91) || (check == 0x94)
                 || (check == 0x95) || (check == 0x9a) || (check == 0x9b)
                 || (check == 0xa0) || (check == 0xab) || (check == 0xb4)
                 || (check == 0xbb) || (check == 0xc6) || (check == 0xc7)
                 || (check == 0xd7) || (check == 0xdd) || (check == 0xe3)) );
    }
    /* Returns true if the control code starts a statement (is the command). */
    bool is_cmd(Uint8 code)
    {
        if(code >= 0xa1 && !is_valsize(code) && !is_test(code))
            return(true);
        return(false);
    }

    bool check_keywords();
    /* Returns true if the current scope permits the current command to execute.
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
        if(scope.top() == CONV_SCOPE_ENDASK
           && (cmd != U6OP_ENDASK))
        {
            return(false);
        }
        if(scope.top() == CONV_SCOPE_IFELSE
           && (cmd != U6OP_ELSE && cmd != U6OP_ENDIF))
        {
            return(false);
        }
        if(scope.top() == CONV_SCOPE_SEEKLOOK
           && (cmd != U6OP_SLOOK))
        {
            return(false);
        }
        if(scope.top() == CONV_SCOPE_SEEKIDENT
           && (cmd != U6OP_SIDENT))
        {
            return(false);
        }
        if(scope.top() == CONV_SCOPE_SEEKCONV
           && (cmd != U6OP_SCONVERSE))
        {
            return(false);
        }
//        if(scope.top() == 
        return(true);
    }
    void enter_scope(uint8 scopedef);
    void break_scope(uint8 levels = 1);
    uint8 current_scope()
    { return(!scope.empty() ? scope.top() : CONV_SCOPE_MAIN); }
public:
    Converse(Configuration *cfg, Converse_interpreter engine_type,
             MsgScroll *ioobj, ActorManager *actormgr, GameClock *c, Player *p,
             ViewManager *viewmgr);
    ~Converse();
    void loadConv(std::string convfilename="converse.a");
    /* Returns true if a script is active (paused or unpaused). */
    bool running() { return(active); }
    /* Returns true if the script is waiting for input. */
    bool waiting() { return(is_waiting); }
    /* Tell script to pause. */
    void wait() { is_waiting = true; }
    /* Tell script to resume. */
    void unwait() { need_input = is_waiting = false; }
    /* Pause to wait for input. */
    void wait_for_input() { need_input = true; wait(); }
    bool start(Actor *talkto); // makes step() available
    bool start(uint8 n)
    {
        return(start(actors->get_actor(n)));
    }
    void stop();
    void reset();
    void step(Uint32 count = 0, Uint8 bcmd = 0x00);
    void print(const char *printstr = 0);
    /* Set input buffer (replacing what is already there.) */
    void input(const char *new_input)
    {
        input_s.assign(new_input);
    }
    void continue_script();
    const char *npc_name(uint8 num);
};


#endif /* CONVERSE_H */
