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
#include <cctype>
#include <cstdio>
#include "U6Lzw.h"
#include "Converse.h"

#define CONVERSE_DEBUG
#ifdef CONVERSE_DEBUG
# define test_msg(S) print(S)
#else
# define test_msg(S) scroll->display_string("")
#endif

using std::cerr;
using std::cin;
using std::endl;

// TODO: work flawlessly with every u6 npc
//       uniform (better) text output
//       show/remove portraits
//       fix overflow at end of Chuckles(10) script

/* Load `convfilename' as src.
 */
void Converse::loadConv(std::string convfilename)
{
    string conv_lib_str;
    config->pathFromValue("config/ultima6/gamedir", convfilename, conv_lib_str);
    delete src;
    src = new U6Lib_n;
    src->open(conv_lib_str, 4);
    src_num = (convfilename == "converse.a")
              ? 1 : (convfilename == "converse.b") ? 2 : 0;
    std::cerr << "Converse: load \"" << convfilename << "\"" << std::endl;
}


Converse::~Converse()
{
    delete src;
    if(active)
        this->stop();
}


/* Set the interpreter type, and point to an opened converse library, and an
 * object to send output to and get input from.
 */
Converse::Converse(Configuration *cfg, Converse_interpreter engine_type,
                   MsgScroll *ioobj, ActorManager *actormgr)
{
    config = cfg;
    scroll = ioobj;
	actors = actormgr;
    interpreter = engine_type;
    src = 0; src_num = 0;
    npc = 0;
    // argument count to each command:
//    memset(&converse_cmd_args, 0, sizeof(Uint8) * 256);
//    converse_cmd_args[U6OP_SETF] = 2;
//    converse_cmd_args[U6OP_CLEARF] = 2;

    script = 0;
    stop();
}


/* Send output buffer or some other text to message scroller.
 */
void Converse::print(const char *printstr)
{
#ifdef CONVERSE_DEBUG /* and optionally send it to stderr */
    fprintf(stderr,"Converse: print \"%s\"\n",printstr?printstr:output.c_str());
#endif
    if(!printstr)
    {
        scroll->display_string((char *)output.c_str());
        output.resize(0);
    }
    else
        scroll->display_string((char *)printstr);
}


/* Output the NPC name at the start of the script (to the output buffer) with
 * a newline, and return their id number.
 */
Uint32 Converse::print_name()
{
    Uint32 id = get_val(0, 0);
    output.append("\n\n");
    print();
    return(id);
}


/* Returns requested value from the argument list, which may actually be at a
 * variable location.
 */
uint32 Converse::get_val(uint8 arg_i, sint8 val_i)
{
    if(args[arg_i].empty())
        return(0);
    uint8 last_val_i = args[arg_i].size() - 1;

    if(val_i >= 0)
        return(args[arg_i][val_i].valt != 0xb2
	       ? args[arg_i][val_i].val : get_var(args[arg_i][val_i].val));
    else // -1 = last value in argument
    {
        if(args[arg_i][last_val_i].valt != 0xb2)
            return(args[arg_i][last_val_i].val);
        else
            return(get_var(args[arg_i][last_val_i].val));
    }
}


/* Handle script text. Evaluate as output text, get as keywords, or skip.
 * FIXME: collect text up to symbol or EOS all at once instead of 1charatatime
 */
void Converse::collect_text(bool eval = false)
{
    unsigned int c;
    char symbol[3] = { '\0', '\0', '\0' }, intval[16];
    const char *p_strt = (const char *)script_pt;

    output.resize(0);
    if(!eval)
    {
        for(c = 0; !check_overflow(c) && is_print(peek(c)); c++);
        output.append((const char *)script_pt, (unsigned int)c);
    }
    else
        for(c = 0; !check_overflow(c) && is_print(peek(c));)
        {
            if(peek(c) == '$' || peek(c) == '#')
            {
                p_strt = (const char *)&script_pt[c];
                // copy symbol
                strncpy(symbol, p_strt, 2);
                // evaluate
                if(!strcmp(symbol, "$G")) // gender title
                    output.append("Doctor");
                else if(!strcmp(symbol, "$N")) // npc name
                    output.append("MyName");
                else if(!strcmp(symbol, "$P")) // player name
                    output.append("Nuvie");
                else if(!strcmp(symbol, "$T")) // time of day
                    output.append("TimeOfDay");
                else if(!strcmp(symbol, "$Z")) // previous input
                    output.append(input_s);
                // FIXME: only converts 1 character variable numbers
                else if(symbol[0] == '#') // value of a variable
                    if(isdigit(symbol[1])
                       && strtol(&symbol[1], NULL, 10) <= CONV_VAR__LAST_)
                    {
                        snprintf(intval, 16, "%d",
                                 heap[strtol(&symbol[1], NULL, 10)].val);
                        output.append(intval);
                    }
                else
                    output.append(symbol);
                c += 2;
            }
            else
            {
                output.append(1,(char)peek(c));
                ++c;
            }
        }
    skip(c);
}


/* Returns true if the keywords list contains the input string, or contains an
 * asterisk (matching any input).
 */
bool Converse::check_keywords()
{
    const char *strt_s = NULL;
    char *tok_s = NULL, *cmp_s = NULL;
    if(keywords == "*")
        return(true);
    // check each comma-separated keyword
    strt_s = keywords.c_str();
    for(uint32 c = 0; c < strlen(strt_s); c++)
    {
        // check at start of string and each keyword
        if(c == 0 || strt_s[c] == ',')
        {
            // copy from keyword start to end of string/keyword
            uint32 l;
            tok_s = strdup(&strt_s[(c == 0) ? c : c + 1]);
            for(l = 0; l < strlen(tok_s) && tok_s[l] != ','; l++);
            tok_s[l] = '\0';
            cmp_s = strdup(input_s.c_str());
            // trim input to keyword size
            if(l < strlen(cmp_s))
                cmp_s[l] = '\0';
            // compare
            if(!strcasecmp(tok_s, cmp_s))
            {
                free(cmp_s);
                free(tok_s);
                return(true);
            }
            free(cmp_s);
            free(tok_s);
        }
    }
    return(false);
}


#ifndef CONVERSE_DEBUG
/* Add new processing scope level `scopedef'.
 */
void Converse::enter_scope(uint8 scopedef)
{
    scope.push(scopedef);
}


/* Remove `levels' previously added scope level(s).
 */
void Converse::break_scope(uint8 levels)
{
    while(levels--)
        if(!scope.empty())
            scope.pop();
        else break;
}

#else

void Converse::enter_scope(uint8 scopedef)
{
    scope.push(scopedef);
    fprintf(stderr, "Converse: ...enter %d...\n", scopedef);
}


void Converse::break_scope(uint8 levels)
{
    while(levels--)
        if(!scope.empty())
        {
            fprintf(stderr, "Converse: ...leave %d...\n", scope.top());
            scope.pop();
        }
        else break;
}
#endif /* CONVERSE_DEBUG */


/* Execute a command code from the parsed script, with optional arguments
 * already collected.
 * Returns false if user input is requested, or the script has stopped, and true
 * otherwise.
 */
bool Converse::do_cmd()
{
    bool donext = true, ifcomp = false;
    Actor *cnpc = 0;
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: cmd=0x%02x\n", cmd);
if(!args.empty() && !args[0].empty())
{
	fprintf(stderr, "Converse: args:");
    for(unsigned int a = 0; a < args.size() && !args[a].empty(); a++)
    {
        fprintf(stderr, " (");
        for(unsigned int v = 0; v < args[a].size(); v++)
            fprintf(stderr, "(0x%02x)0x%02x", args[a][v].valt, args[a][v].val);
        fprintf(stderr, ")");
    }
    fprintf(stderr, "\n");
}
#endif
    if(!check_scope())
        return(donext);
    switch(cmd)
    {
        case U6OP_IF: // 1 arg, with multiple vals last val is test type
            test_msg("-if-");
            // is val "true"?
            if(val_count(0) == 1)
            {
                if(get_val(0, 0))
                    ifcomp = true;
            }
            else
            switch(get_val(0, -1))
            {
                case 0x81: // val1 > val2
                    if((val_count(0) > 2) && get_val(0, 0) > get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x83: // val1 < val2
                    if((val_count(0) > 2) && get_val(0, 0) < get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x84: // ?? val1 <= val2
                    test_msg("-0x84:<=?-");
                    if((val_count(0) > 2) && get_val(0, 0) <= get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x85: // val1 != val2
                    if((val_count(0) > 2) && get_val(0, 0) != get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x86: // val1 == val2
                    if((val_count(0) > 2) && get_val(0, 0) == get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0xab: // is npc(val1) flag(val2) set?
                    if(val_count(0) > 2 && get_val(0, 1) <= 7)
                    {
                        if(get_val(0, 0) == 0xeb)
                            cnpc = npc;
                        else
                            cnpc = actors->get_actor(get_val(0, 0));
                        if(cnpc->get_flags() & (1 << get_val(0, 1)))
                            ifcomp = true;
                    }
                    break;
                case 0xc6: // is val1 # of npc in party?
                    print("-!ifinparty- ");
                    break;
                case 0x82: // ?? >=
                case 0x95: // (?) && (?) ??
                default:
                    print("\nError: Unknown test\n");
                    break;
            }
            enter_scope(ifcomp ? CONV_SCOPE_IF : CONV_SCOPE_IFELSE);
            test_msg((char *)(ifcomp ? "-TRUE-" : "-FALSE-"));
            break;
        case U6OP_ENDIF:
            break_scope();
            break;
        case U6OP_ELSE:
            if(current_scope() == CONV_SCOPE_IFELSE)
            {
                break_scope();
                enter_scope(CONV_SCOPE_IF);
            }
            else if(current_scope() == CONV_SCOPE_IF)
            {
                break_scope();
                enter_scope(CONV_SCOPE_IFELSE);
            }
            break;
        case U6OP_DECL:
            declared = get_rval(0, 0);
            if(declared < 0 || declared > CONV_VAR__LAST_)
            {
                print("\nError: Illegal var. num\n");
                break;
            }
            test_msg("-let X-");
            break;
        case U6OP_ASSIGN: // 1 arg with assignment values/operations
            if(declared < 0 || declared > CONV_VAR__LAST_)
            {
                print("\nError: Unknown variable\n");
                break;
            }
            // simple assignment
            if(val_count(0) == 1)
            {
                test_msg("- = X-");
                heap[declared].val = get_val(0, 0);
            }
            // assignment of expression with two values and an operation
            else if(val_count(0) == 3)
            {
                switch(get_val(0, -1))
                {
                    case 0x90: // val1 + val2
                        test_msg("- = X + Y-");
                        heap[declared].val = get_val(0, 0) + get_val(0, 1);
                        break;
                    case 0x91: // val1 - val2
                        test_msg("- = X - Y-");
                        heap[declared].val = get_val(0, 0) - get_val(0, 1);
                        break;
                    case 0xa0: // random number between val1 and val2 inclusive
                        test_msg("- = rnd(X to Y)-");
                        heap[declared].val = rnd(get_val(0, 0),
                                                 get_val(0, 1));
                        break;
                    case 0xab: // flag val2 of npc val1
                        test_msg("- = npc(flag)-");
                        cnpc = actors->get_actor(get_val(0, 0));
                        if(cnpc)
                            heap[declared].val = (cnpc->get_flags()
                                                         & (1 << get_val(0, 1)))
                                                 ? 1 : 0;
                        else
                        {
                            print("\nError: No such NPC\n");
                            heap[declared].val = 0;
                        }
                        break;
                    default:
                        print("\nError: Unknown assignment\n");
                        break;
                }
            }
            else
                print("\nAssignment error\n");
            declared = -1;
            break;
        case U6OP_ARGSTOP:
            test_msg("-EOA-");
            break;
        case U6OP_SETF: // 0,0=npc 1,0=flagnum
            if(get_val(0, 0) == 0xeb) // "this npc"
                cnpc = npc;
            else
                cnpc = actors->get_actor(get_val(0, 0));
            cnpc->set_flag(get_val(1, 0));
            break;
        case U6OP_CLEARF: // 0,0=npc 1,0=flagnum
            if(get_val(0, 0) == 0xeb) // "this npc"
                cnpc = npc;
            else
                cnpc = actors->get_actor(get_val(0, 0));
            cnpc->clear_flag(get_val(1, 0));
            break;
        case U6OP_JUMP:
            seek(get_val(0, 0));
            break_scope(scope.size());
            break;
        case U6OP_BYE:
            stop();
            donext = false;
            break;
        case U6OP_NEW: // 4 args, npc, objnum, qual, quant
            print("-!new obj.-");
            break;
        case U6OP_DELETE: // 4 args, npc, objnum, qual, quant
            print("-!delete obj.-");
            break;
        case U6OP_GIVE: // 4 args, objnum, qual, fromnpc, tonpc
            print("-!give obj.-");
            break;
        case U6OP_WORKTYPE: // 2 args, npc number, new worktype
            print("-!worktype-");
            break;
        case U6OP_PORTRAIT:
            print("-!portrait-");
            break;
        case U6OP_INVENTORY:
            print("-!inventory-");
        case U6OP_WAIT:
            scroll->display_string("*");
            break;
        case U6OP_ENDASK:
//            std::cerr << "Converse: END-OF-ASK section" << std::endl;
            keywords.resize(0);
            break_scope(2);
            break;
        case U6OP_KEYWORD: // end of last answer, start of keyword list
            if(current_scope() == CONV_SCOPE_ANSWER)
            {
                // already answered
                break_scope();
                enter_scope(CONV_SCOPE_ENDASK);
            }
            else
            {
                // not answered yet
                if(current_scope() != CONV_SCOPE_ASK)
                    enter_scope(CONV_SCOPE_ASK);
                keywords.assign(output);
                output.resize(0);
            }
            break;
        case U6OP_SIDENT:
            break_scope(); // SEEKIDENT
            print_name();
            enter_scope(CONV_SCOPE_SEEKLOOK);
            break;
        case U6OP_SLOOK:
        case U6OP_SLOOKB:
            break_scope(); // SEEKLOOK
            print("You see ");
            break;
        case U6OP_SCONVERSE:
//            std::cerr << "Converse: CONVERSE section" << std::endl;
            break;
        case U6OP_ASK:
            scroll->display_string("\nyou say: ");
            scroll->set_input_mode(true);
            wait(); donext = false;
            break;
        case U6OP_ASKC:
            scroll->set_input_mode(true, output.c_str(), false);
            wait(); donext = false;
            break;
        case U6OP_SANSWER:
            if(check_keywords())
            {
                keywords.resize(0);
                // continue, no skip
                enter_scope(CONV_SCOPE_ANSWER);
            }
            break;
        case U6OP_INPUT: // 1 val, variable to store input at
            declared = get_rval(0, 0);
            scroll->set_input_mode(true);
            wait(); donext = false;
            break;
        case U6OP_INPUTC: // 1 val, variable to store input at
            declared = get_rval(0, 0);
            scroll->set_input_mode(true, "0123456789");
            wait(); donext = false;
            break;
        case 0x00: // incorrectly parsed
            print("\nNull command\n");
            // try to salvage the next statement
            while(!check_overflow() && pop() != U6OP_ARGSTOP);
            donext = false;
            break;
        default: // unimplemented
            print("\nUnknown command\n");
            donext = false;
    }
    cmd = 0x00;
    return(donext);
}


/* Each control statement has a specific number of arguments & values. This
 * collects them into the argument vector for `cmd'.
 */
void Converse::collect_args()
{
    Uint32 val = 0, ai = 0, vi = 0;
    while(!check_overflow())
    {
//      std::cerr << "check val" << std::endl;
        val = peek();
        if(val == U6OP_ARGSTOP)
        {
//          std::cerr << "val is 0xa7, skip, next arg" << std::endl;
            skip(); ++ai; vi = 0;
            args.resize(ai + 1); args[ai].resize(0);
            continue;
        }
        if(is_print(val) || is_cmd(val))
        {
//          std::cerr << "val is printable or cmd, done" << std::endl;
            break;
        }
//            std::cerr << "skip, get val" << std::endl;
        skip();
        if(val == U6OP_UINT32)
        {
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi++].val = pop4();
//          std::cerr << "popped 4, next val" << std::endl;
        }
        else if(val == U6OP_UINT8)
        {
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi++].val = pop();
//			fprintf(stderr, "popped 1 as val: 0x%02x\n", args[ai][vi-1].val);
        }
        else if(val == U6OP_UINT16)
        {
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi++].val = pop2();
 //         std::cerr << "popped 2, next val" << std::endl;
        }
        else
        {
//          std::cerr << "val is val" << std::endl;
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi].val = val;
            if(peek() == 0xb2)
            {
                args[ai][vi].valt = pop();
//              std::cerr << "0xb2 follows, popped" << std::endl;
            }
            ++vi;
//          std::cerr << "next val" << std::endl;
        }
    }
}


/* Interpret `count' statements from the NPC script. These are control codes
 * (command with optional arguments, each with variable number of values), or
 * text. A count of 0 means to keep going if possible.
 */
void Converse::step(Uint32 count)
{
    Uint32 c = 0;
    bool stepping = true;

    while(stepping && !check_overflow() && ((count > 0) ? (c++ < count) : true))
    {
        if(is_print(peek()))
        {
            if(check_scope())
            {
                collect_text(true);
                print();
            }
            else
                collect_text(false); // save some processing time
            continue;
        }

        cmd = pop();
        if(cmd == U6OP_SLOOK || cmd == U6OP_SLOOKB)
            look_offset = (Uint32)(script_pt - 1 - script);
        if(cmd == U6OP_ASK || cmd == U6OP_ASKC)
            getinput_offset = (Uint32)(script_pt - 1 - script);
        // get args
        args.clear(); args.resize(1);
        if(cmd == U6OP_JUMP)
        {
            args[0].resize(1);
            args[0][0].valt = 0;
            args[0][0].val = pop4();
        }
        else if(cmd == U6OP_SIDENT)
        {
            args[0].resize(1);
            args[0][0].valt = 0;
            args[0][0].val = pop(); // NPC id number
            collect_text(false); // name
        }
        else if(cmd == U6OP_KEYWORD)
	{
	    collect_text(); // get keyword list
	}
        else if(cmd == U6OP_ASKC)
        {
            collect_text(); // get characters allowed as input
        }
        else
            collect_args();
        stepping = do_cmd();
    }
    if(check_overflow())
    {
        print("\n-EOF-\n");
        stop();
    }
}


/* Stop execution of the current script and unload it. Initialize data.
 */
void Converse::stop()
{
    npc = 0;
    free(script);
    script_pt = script = 0;
    script_len = 0;
    heap.clear();
    declared = -1;

    input_s.resize(0); // input_s.erase();
    keywords.resize(0);
    args.clear();
    while(!scope.empty())
        scope.pop();

    active = is_waiting = false;
    std::cerr << "Converse: script is stopped" << std::endl;
}


/* Set the initial variables that an npc script can access.
 */
void Converse::init_variables()
{
    heap.resize(CONV_VAR__LAST_ + 1); // FIXME: the U6 max. var num is unknown
    for(int v = 0; v <= CONV_VAR__LAST_; v++)
        heap[v].valt = heap[v].val = 0x00;
    heap[CONV_VAR_SEX].val = 0; // everyone is male for now
//    heap[CONV_VAR_WORKTYPE].val = npc->get_worktype();
    heap[CONV_VAR_WORKTYPE].val = 0x20;
}


/* Returns false if a conversation cannot be started with NPC `talkto'. This
 * represents an internal error, and doesn't have anything to do with the NPC
 * not wanting/being able to talk to the Avatar.
 */
bool Converse::start(Actor *talkto)
{
    unsigned char *undec_script = 0; // item as it appears in library
    Uint32 undec_script_len = 0, actor_num = 0, script_num = 0;
    U6Lzw decoder;
    if(!talkto || !src)
        return(false);

    // make sure previous script is unloaded first
    if(active)
        this->stop();
    script_num = actor_num = talkto->get_actor_num();
    npc = talkto;
    init_variables();
    set_conv(script_num);
    undec_script_len = src->get_item_size(script_num);
    if(undec_script_len > 4)
    {
        std::cerr << "Converse: reading";
        undec_script = src->get_item(script_num);
        // decode
        if(!(undec_script[0] == 0 && undec_script[1] == 0
             && undec_script[2] == 0 && undec_script[3] == 0))
        {
            std::cerr << " encoded script";
            script = decoder.decompress_buffer(undec_script, undec_script_len,
                                               script_len);
            free(undec_script);
        }
        else
        {
            std::cerr << " unencoded script";
            script = undec_script;
            script_len = undec_script_len;
        }
        std::cerr << " for NPC " << (int)actor_num << std::endl;
    }
    // start script (makes step() available)
    if(script)
    {
        std::cerr << "Converse: begin" << std::endl;
        active = true;
        seek(0);
        enter_scope(CONV_SCOPE_SEEKIDENT);
        return(true);
    }
    std::cerr << std::endl << "Converse: error loading script " << (int)script_num
              << std::endl;
    return(false);
}


/* Check to see if script is not waiting. If it is, continue the active script.
 * If the script is waiting, check the i/o object for input, taking the input if
 * available.
 */
void Converse::continue_script()
{
    if(running())
    {
        if(!waiting())
            step();
        else if(scroll->get_input())
        {
            scroll->display_string("\n\n");
            input_s.assign(scroll->get_input());
            if(input_s.empty())
                input_s.assign("bye");
            else if(input_s == "look")
            {
                seek(look_offset);
                step(2); // "look" marker & text, another FIXME: look section can have many statements
                seek(getinput_offset);
            }
            // assign value to declared input variable
            if(declared >= 0 && declared <= CONV_VAR__LAST_)
            {
                heap[declared].val = strtol(input_s.c_str(), NULL, 10);
                declared = -1;
            }
            unwait();
        }
        // script has stopped itself:
        if(!running())
        {
            scroll->set_talking(false);
            scroll->display_string("\n");
//            remove portrait [& name [& inventory display]]
            scroll->display_prompt();
        }
    }
}
