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
#include "U6Lzw.h"
#include "Converse.h"

// argument counts (256args * 256values), if args are collected first
static Uint8 converse_cmd_args[256];


/* Load converse.a as src.
 */
void Converse::loadConv()
{
    string conv_lib_str;
    config->pathFromValue("config/ultima6/gamedir", "converse.a", conv_lib_str);
    src = new U6Lib_n;
    src->open(conv_lib_str, 4);
}


/* Set the interpreter type, and point to an opened converse library. Initialize
 * data. FIXME: call stop() to do most of the work
 */
Converse::Converse(Configuration *cfg, Converse_interpreter engine_type)
{
    config = cfg;
    interpreter = engine_type;
    src = 0;
    npc = 0;
    active = is_waiting = false;
    prompt = 0;
    allowed_input = NULL;
    script_pt = script = 0;
    script_len = 0;
    output_cpy = 0;
    cmd = 0x00; // no command

    // argument count to each command:
    memset(&converse_cmd_args, 0, sizeof(Uint8) * 256);
    converse_cmd_args[U6OP_SETF] = 2;
    converse_cmd_args[U6OP_CLEARF] = 2;
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
    output.append("\n");
    pop(len);
    fprintf(stderr, "Converse: got name of npc 0x%02x\n", id);
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


/* Try to print `print_len' bytes of text from the currect script location.
 * Returns true if printing (to output string) was successful or false if the
 * script runs out or a non-printable character is encountered.
 */
bool Converse::do_text(Uint32 print_len)
{
// loop to print len
// when symbol is encountered, print up to c, update p_strt
//  parse symbol
//  print evaluated string
// if p_strt is before c, print up to c
//    int c = 0;
//    unsigned char *p_strt = script_pt;
//    while(c < print_len)
//    {
//    }
    output.append((const char *)script_pt, (unsigned int)print_len);
    skip(print_len);
    return(true);
}


/* Execute a command code from the parsed script, with optional arguments
 * already collected. Some commands have arguments that will be collected here.
 * Returns false if user input is requested, or the script has stopped, and true
 * otherwise.
 */
bool Converse::do_cmd()
{
    bool donext = true;
    switch(cmd)
    {
        case U6OP_SETF: // set (npc, flag)
            std::cerr << "Converse: SET" << std::endl;
            break;
        case U6OP_CLEARF: // clear (npc, flag)
            std::cerr << "Converse: CLEAR" << std::endl;
            break;
        case U6OP_JUMP: // goto (4byte offset)
            std::cerr << "Converse: JUMP" << std::endl;
            seek(pop4());
            break;
        case U6OP_BYE: // bye
            std::cerr << "Converse: BYE" << std::endl;
            stop();
            donext = false;
            break;
        case U6OP_WAIT: // wait
            std::cerr << "Converse: WAIT" << std::endl;
            wait();
            prompt = 1;
            donext = false;
            break;
        case U6OP_ENDASK: // endask
            std::cerr << "Converse: end of ASK section" << std::endl;
            break;
        case U6OP_KEYWORD: // keywords (keyword list)
            std::cerr << "Converse: get KEYWORDS" << std::endl;
            keywords = "";
            while(!check_overflow() && peek() > 0 && peek() < 0xa0)
                keywords.push_back((char)pop());
            std::cerr << "Converse: got \"" << keywords << "\"" << std::endl;
            break;
        case U6OP_SIDENT:// npc (1byte npc, name)
            std::cerr << "Converse: start IDENT section" << std::endl;
            print_name();
            break;
        case U6OP_SLOOK:
        case U6OP_SLOOKB: // description start
            std::cerr << "Converse: start LOOK section" << std::endl;
            output.append("You see ");
            break;
        case U6OP_SCONVERSE: // conversation start
            std::cerr << "Converse: start CONVERSE section" << std::endl;
            break;
        case U6OP_SASK: // ask section start
            std::cerr << "Converse: start ASK section" << std::endl;
            wait();
            prompt = 2; allowed_input = NULL; // any
            donext = false;
            break;
        case U6OP_SANSWER: // answer section start
            std::cerr << "Converse: check KEYWORDS" << std::endl;
//            if(check_keywords())
//                keywords = "";
//            else(
            break;
        case 0x00:
            output.append("\nNull command\n");
            break;
        default:
            output.append("\nUnknown command\n");
    }
    cmd = 0x00;
//    args.clear();
    return(donext);
}


#if 0
/* Read in the argument list for the command. Each 0xa7 starts the next, and
 * ends the final argument.
 * If the command has 1 argument it does not have to be terminated with 0xa7.
 */
void Converse::collect_args(Uint32 cmd, Uint32 argc, unsigned char *strt,
                            unsigned int len)
{
    int a = 0, b = 0;
    args.reserve(argc);
    for(a = 0; a < argc; a++)
    {
        args[argc].val_c = 0;
        if(converse_cmd_variadic[cmd])
        {
            args[a].val_t = (Uint32 *)malloc(sizeof(Uint32) * args[argc].val_c);
            args[a].val = (Uint32 *)malloc(sizeof(Uint32) * args[argc].val_c);
//        while(b < len)
//        {
//        }
        }
    }

    do // args
                {
                    args[arg_c].val_c = 0;
                    args[arg_c].val_t = args[arg_c].val = NULL;
                    collect_args(
                    while(*script_pt != 0xa7) // arg values
                    {
                        args[arg_c].val_t = realloc(args[arg_c].val_t, sizeof(Uint32)
                                                            *args[arg_c].val_c);
                        args[arg_c].val = realloc(args[arg_c].val, sizeof(Uint32)
                                                            *args[arg_c].val_c);
                        // get type
                        if(args[arg_c].val_t[args[arg_c].val_c] == 0
                           && is_valsize(*script_pt))
                        // get value
                        else if(args[arg_c].val_t[args[arg_c].val_c] == 0
                                && is_varnum(*script_pt))
                        else // probably next cmd
                        {
                            ++args[arg_c].val_c;
                            ++script_pt;
                            break;
                        }
                    }
                    args[arg_c].val_t
                    fprintf(stderr, "Converse: arg arg_c\n");
                } while(++a < arg_c);
}
#endif


/* Interpret `count' statements from the NPC script. These are control codes
 * (command with optional arguments, each with variable number of values), or
 * text. A count of 0 means to keep going if possible.
 * Returns true if stepping can continue, and false if the script is stopped,
 * or user input is requested.
 */
void Converse::step(Uint32 count)
{
    Uint32 c = 0, a = 0, arg_c = 0, len = 0;
    bool stepping = true;
    if(!active)
        return;

    do
    {
        cmd = peek();
        if(cmd == 0 || cmd > 0xa0) // FIXME: change istextorcontrol comparison
        {
            skip();
            fprintf(stderr, "Converse: cmd is 0x%02x\n", cmd);
            arg_c = converse_cmd_args[cmd <= 255 ? cmd : 0x00];
//            args.reserve(arg_c);
            fprintf(stderr, "Converse: cmd has %d arg(s)\n", arg_c);
            if(arg_c == 0)
            {
                stepping = do_cmd();
            }
            else
            {
                if(arg_c == 1)
                    skip(2);
                else
                    for(a = 0; a < arg_c; a++)
                        skip(3);
                arg_c = 0;
                output.append("\nUnimplemented\n");
            }
            ++c; continue;
        }
        std::cerr << "Converse: print" << std::endl;
        // grab text length and parse it
        for(len = 1; peek(len) > 0 && peek(len) < 0xa0 && !check_overflow(len); ++len);
        if(!do_text(len))
            output.append("\nPrint error\n");
        ++c;
    } while(((count != 0) ? c < count : true) && stepping && !check_overflow());
    if(check_overflow())
    {
        output.append("\nEOF\n");
        stop();
    }
}


/* Stop execution of the current script and unload it.
 */
void Converse::stop()
{
    if(!active)
        return;
    if(script_len)
    {
        script_len = 0;
        delete script;
    }
    script_pt = 0;
    if(output_cpy)
    {
        free(output_cpy);
        output_cpy = NULL;
    }
    cmd = 0x00;
//    args.clear();
    is_waiting = false;
    prompt = 0;
    allowed_input = 0;
    input_s.resize(0); // input_s.erase();
    npc = 0;
    active = false;
    std::cerr << "Converse: stopped script" << std::endl;
}


/* Returns false if a conversation cannot be started with NPC `talkto'. This
 * represents an internal error, and doesn't have anything to do with the NPC
 * not wanting/being able to talk to the Avatar.
 */
bool Converse::start(Actor *talkto)
{
    unsigned char *undec_script = 0; // item as it appears in library
    Uint32 undec_script_len = 0, actor_num = 0;
    U6Lzw decoder;
    if(!talkto || !src)
        return(false);

    // make sure previous script is unloaded first
    if(active)
        this->stop();
    actor_num = talkto->get_actor_num();
    undec_script_len = src->get_item_size(actor_num);
    if(undec_script_len > 4)
    {
        undec_script = src->get_item(actor_num);
        std::cerr << "Converse: loading";
        // decode
        if(!(undec_script[0] == 0 && undec_script[1] == 0
             && undec_script[2] == 0 && undec_script[3] == 0))
        {
            std::cerr << " encoded script";
            script = decoder.decompress_buffer(undec_script, undec_script_len,
                                               script_len);
        }
        else
        {
            std::cerr << " unencoded script";
            script = undec_script;
            script_len = undec_script_len;
        }
    }
    std::cerr << " for NPC " << (int)actor_num << std::endl;
    // start script (makes step() available)
    if(script)
    {
        std::cerr << "Converse: begin" << std::endl;
        active = true;
        seek(0);
        return(true);
    }
    std::cerr << std::endl << "Converse: error loading actor " << (int)actor_num
                 << " script" << std::endl;
    return(false);
}
