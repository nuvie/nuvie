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
#include <cstring>
#include "U6Lzw.h"
#include "Converse.h"

using std::cerr;
using std::cin;
using std::endl;
// temporary variable to keep script from getting into infinite loop
static uint8 jump_count;

// TODO: work flawlessly with every u6 npc
//       uniform text output
//       show/remove portraits
//       get/set flags

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


/* Output the NPC name at the start of the script (to the output buffer) with
 * a newline, and return their id number.
 */
Uint32 Converse::print_name()
{
    Uint32 len = 0, id = 0;
    id = pop();
    for(; isprint(script_pt[len]); len++);
    output.append((const char *)script_pt, (unsigned int)len);
    output.append("\n\n");
    pop(len);
    return(id);
}


/* Update the script pointer to past the NPC identification.
 */
void Converse::seek_look()
{
    seek_byte(U6OP_SLOOK);
    if(script_pt == script)
        seek_byte(U6OP_SLOOKB);
}


/* Update the script pointer to past the NPC identification and look sections.
 */
void Converse::seek_converse()
{
    seek_look();
    seek_byte(U6OP_SCONVERSE);
}


/* Returns requested value from the argument list, which may actually be at a
 * variable location.
 */
uint32 Converse::get_val(uint8 arg_i, sint8 val_i)
{
	if(val_i >= 0)
        return(args[arg_i][val_i].valt != 0xb2
			   ? args[arg_i][val_i].val : get_var(args[arg_i][val_i].val));
	else // -1 = last value in argument
	{
        if(args[arg_i][args[arg_i].size()-1].valt != 0xb2)
            return(args[arg_i][args[arg_i].size()-1].val);
        else
            return(get_var(args[arg_i][args[arg_i].size()-1].val));
	}
}


/* Handle script text. Evaluate as output text, get as keywords, or skip.
 */
void Converse::collect_text(uint8 text_op = CONV_TEXTOP_PRINT)
{
// loop to print len
// when symbol is encountered, collect up to c, update p_strt
//  parse symbol
//  collect evaluated string
// if p_strt is before c, collect up to c
//    int c = 0;
//    unsigned char *p_strt = script_pt;
//    while(c < print_len)
//    {
//    }
    int c;
	output.resize(0);
    for(c = 0; !check_overflow(c) && is_print(peek(c)); c++);
    output.append((const char *)script_pt, (unsigned int)c);
    skip(c);

	switch(text_op)
	{
		case CONV_TEXTOP_PRINT:
			break;
		case CONV_TEXTOP_KEYWORD:
			break;
	}
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
            if(!strcmp(tok_s, cmp_s))
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


/* Execute a command code from the parsed script, with optional arguments
 * already collected.
 * Returns false if user input is requested, or the script has stopped, and true
 * otherwise.
 */
bool Converse::do_cmd()
{
    bool donext = true, ifcomp = false;
	Actor *cnpc = 0;
	uint8 flagnum = 0, flags = 0x00;
#if 0
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
        case U6OP_IF: // 1 arg, last val is test type
            switch(get_val(0, -1))
            {
                case 0x84: // val1 < val2
					scroll->display_string("\n-if ? < ?-\n");
                    if((val_count(0) > 2) && get_val(0, 0) < get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x85: // val1 != val2
					scroll->display_string("\n-if ? != ?-\n");
                    if((val_count(0) > 2) && get_val(0, 0) != get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0x86: // val1 == val2
					scroll->display_string("\n-if ? == ?-\n");
                    if((val_count(0) > 2) && get_val(0, 0) == get_val(0, 1))
                        ifcomp = true;
                    break;
                case 0xab: // is npc(val1) flag(val2) set?
					scroll->display_string("\n-if npc(flag)-\n");
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
                    scroll->display_string("\n!:ifinparty\n");
                    break;
                case 0x81: // ??
                case 0x82: // ??
                case 0x83: // ??
                default:
                    scroll->display_string("\nError: Unknown test\n");
                    break;
            }
            enter_scope(ifcomp ? CONV_SCOPE_IF : CONV_SCOPE_IFELSE);
			scroll->display_string(ifcomp ? "-TRUE" : "-FALSE" "-\n");
            break;
        case U6OP_ENDIF:
            break_scope();
            break;
        case U6OP_ELSE:
            break_scope();
            enter_scope(CONV_SCOPE_IF);
            break;
        case U6OP_ARGSTOP:
//            std::cerr << "Converse: END-OF-ARGUMENT" << std::endl;
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
            if(++jump_count == 5)
            {
                fprintf(stderr, "Converse: infinite loop detected!\n");
                scroll->display_string("\nError\n");
                stop();
                donext = false;
            }
            break;
        case U6OP_BYE:
            stop();
            donext = false;
            break;
		case U6OP_PORTRAIT:
			scroll->display_string("\n!:portrait\n");
			break;
        case U6OP_WAIT:
            scroll->display_string("*");
            break;
        case U6OP_ENDASK:
//            std::cerr << "Converse: END-OF-ASK section" << std::endl;
            keywords.resize(0);
            break_scope(2);
            break;
        case U6OP_KEYWORD:
		    keywords.assign(output);
			output.resize(0);
            break;
        case U6OP_SIDENT:
//            std::cerr << "Converse: IDENT section" << std::endl;
            print_name();
            break;
        case U6OP_SLOOK:
        case U6OP_SLOOKB:
//            std::cerr << "Converse: LOOK section" << std::endl;
            output.append("You see ");
            break;
        case U6OP_SCONVERSE:
//            std::cerr << "Converse: CONVERSE section" << std::endl;
            break;
        case U6OP_SASK:
//            std::cerr << "Converse: ASK section" << std::endl;
            scroll->display_string("\nyou say: ");
            scroll->set_input_mode(true);
            enter_scope(CONV_SCOPE_ASK);
            wait(); donext = false;
            break;
        case U6OP_SASKC:
            scroll->display_string("\n!:askc\n");
            break;
        case U6OP_SANSWER:
//            std::cerr << "Converse: ANSWER (check KEYWORDS)" << std::endl;
            if(check_keywords())
            {
                keywords.resize(0);
                // continue, no skip
                enter_scope(CONV_SCOPE_ANSWER);
                jump_count = 0;
            }
            break;
        case 0x00: // incorrectly parsed
            scroll->display_string("\nNull command\n");
            // try to salvage the next statement
            while(!check_overflow() && pop() != U6OP_ARGSTOP);
            donext = false;
            break;
        default: // unimplemented
            scroll->display_string("\nUnknown command\n");
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
        if(val == 0xd2)
        {
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi++].val = pop4();
//          std::cerr << "popped 4, next val" << std::endl;
        }
        else if(val == 0xd3)
        {
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi++].val = pop();
//			fprintf(stderr, "popped 1 as val: 0x%02x\n", args[ai][vi-1].val);
        }
        else if(val == 0xd4)
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
            collect_text();
			if(check_scope())
			{
//				fprintf(stderr, "Converse: print\n");
				print();
			}
            continue;
        }

        cmd = pop();
        if(cmd == U6OP_SIDENT)
        {
            print_name();
            continue;
        }

        if(cmd == U6OP_SLOOK || cmd == U6OP_SLOOKB)
            look_offset = (Uint32)(script_pt - 1 - script);
        if(cmd == U6OP_SASK || cmd == U6OP_SASKC)
            getinput_offset = (Uint32)(script_pt - 1 - script);
        // get args
        args.clear(); args.resize(1);
        if(cmd == U6OP_JUMP)
        {
            args[0].resize(1);
			args[0][0].valt = 0;
            args[0][0].val = pop4();
        }
		else if(cmd == U6OP_KEYWORD)
		{
			collect_text(CONV_TEXTOP_KEYWORD);
		}
        else
            collect_args();
        stepping = do_cmd();
    }
	if(check_overflow())
	{
		scroll->display_string("\n-EOF-\n");
		stop();
	}
    print();
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
	//	heap[CONV_VAR_WORKTYPE].val = npc->get_worktype();
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
        jump_count = 0; // FIXME: tmp var
        seek(0);
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
                break_scope(); // not part of ASK block
                seek(look_offset);
                step(2); // "look" marker & text
                seek(getinput_offset);
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
