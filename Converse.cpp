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
 * data.
 */
Converse::Converse(Configuration *cfg, Converse_interpreter engine_type)
{
    config = cfg;
    interpreter = engine_type;
    src = 0;
    npc = 0;
    active = is_waiting = false;
    script_pt = script = 0;
    script_len = 0;
    output_cpy = 0;
    cmd = 0x00; // no command
    section = 0; // look section

    // argument count to each command:
    memset(&converse_cmd_args, 0, sizeof(Uint8) * 256);
    converse_cmd_args[0xa4] = 2;
    // "is multival" setting to each command (applies when args > 0):
    memset(&converse_cmd_multival, 1, sizeof(Uint8) * 256);
    converse_cmd_multival[0xa6] = 0;
    converse_cmd_multival[0xfc] = 0;
}


/* Output the NPC name at the start of the script (to the output buffer.)
 */
void Converse::print_name()
{
    Uint32 len = 0;
    unsigned char *old_pt = script_pt;
    script_pt = &script[2];
    for(; isprint(script_pt[len]); len++);
    output.append((const char *)script_pt, (unsigned int)len);
    output.append("\n");
    script_pt = old_pt;
}


/* Returns a pointer to the start of section `section_id'. Look section is 0,
 * Converse is 1.
 */
unsigned char *Converse::seek_section(Uint32 section_id)
{
    unsigned char *pt = script;
    if(section_id == 0)
    {
        for(; *pt != 0xf1; pt++);
        return(pt);
    }
    else if(section_id == 1)
    {
        pt = this->seek_section(0);
        for(; *pt != 0xf2 && *pt != 0xf3; pt++);
        return(pt);
    }
    return(pt);
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
    return(true);
}


/* Interpret a statement from an NPC script. Executes the command/instruction,
 * with optional values (which make up the arguments of a defined statement.)
 * This will pause or stop the script if required.
 * Returns true if stepping can continue, and false if the script is stopped,
 * or user input is requested.
 */
bool Converse::do_cmd()
{
    bool donext = true;
    switch(cmd)
    {
        case 0xa4: // set npc flag
            cerr << "Converse: SET" << endl;
            break;
        case 0xa5: // clear npc flag
            cerr << "Converse: CLEAR" << endl;
            break;
        case 0xb6: // bye
            cerr << "Converse: BYE" << endl;
            this->stop();
            donext = false;
            break;
        case 0xcb: // wait
            cerr << "Converse: WAIT" << endl;
            this->wait();
            donext = false;
            break;
        case 0x00:
            output.append("\nNull command\n");
            break;
        default:
            output.append("\nUnknown command\n");
    }
    cmd = 0x00;
    args.clear();
    return(donext);
}


#if 0
/* Read in the argument list for the command from `strt' which is the start of
 * a buffer of `len' bytes. If the command is variadic, each argument can
 * contain any number of values and value types, and each argument will be
 * terminated with 0xa7 (U6). If the command is not variadic, each argument can
 * contain 1 value and type, and does not have to be terminated with 0xa7.
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


/* Execute commands/print text from the active script. Stop when `count'
 * statements or entire strings of text have been interpreted, or if user input
 * is required to continue. A count of 0 means to go on until the latter, or
 * until the end of the conversation.
 */
void Converse::step(Uint32 count = 0)
{
    Uint32 c = 0, arg_c = 0, len = 0;
    bool stepping = true;
    if(!active)
        return;

    do
    {
        if(this->check_overflow(script_pt))
        {
            output.append("\nEOF\n");
            this->stop();
            break;
        }
        if(*script_pt == 0xf1)
        {
            section = 0;
            output.append("You see ");
            ++c; ++script_pt;
        }
        else if(*script_pt == 0xf2 || *script_pt == 0xf3)
        {
            section = 1;
            ++c; ++script_pt;
        }
        if(isprint(*script_pt)) // FIXME: change isprintable comparison
        {
            // grab text length and parse it
            for(len = 1; (&script_pt[len] < (script_pt + script_len))
                         && (isprint(script_pt[len - 1])); ++len);
            if(!do_text(len))
                output.append("\nPrint error\n");
            script_pt += len;
            ++c;
            break;
        }
        else if(cmd == 0x00)
        {
            cmd = *script_pt;
            ++script_pt;
            fprintf(stderr, "Converse: cmd is 0x%02x\n", cmd);
            arg_c = converse_cmd_args[cmd <= 255 ? cmd : 0x00];
            args.reserve(arg_c);
            fprintf(stderr, "Converse: cmd has %d arg(s)\n", arg_c);
            if(arg_c == 0)
            {
                stepping = do_cmd();
                c++;
            }
        }
        else
        {
            // get values until 0xa7
            if(converse_cmd_multival[cmd] == 1)
            {
                int val_i = 0;
                while(!this->check_overflow(script_pt) && (*script_pt != 0xa7))
                {
                    script_pt += this->readval(arg_c, val_i, script_pt);
                }
            }
            else // get 1 value
            {
                args[arg_c].val_c = 1;
                script_pt += this->readval(arg_c, 0, script_pt);
                ++arg_c;
            }
            if(args.size() == arg_c)
            {
                stepping = do_cmd();
                c++;
            }
        }
    } while(((count != 0) ? c < count : true) && stepping);
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
    args.clear();
    is_waiting = false;
    npc = 0;
    active = false;
    cerr << "Converse: stopped script" << endl;
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
        cerr << "Converse: loading";
        // decode
        if(!(undec_script[0] == 0 && undec_script[1] == 0
             && undec_script[2] == 0 && undec_script[3] == 0))
        {
            cerr << " encoded script";
            script = decoder.decompress_buffer(undec_script, undec_script_len,
                                               script_len);
        }
        else
        {
            cerr << " unencoded script";
            script = undec_script;
            script_len = undec_script_len;
        }
    }
    cerr << " for NPC " << (int)actor_num << endl;
    // start script (makes step() available)
    if(script)
    {
        cerr << "Converse: begin" << endl;
        active = true;
        script_pt = this->seek_section(0);
        return(true);
    }
    cerr << endl << "Converse: error loading actor " << (int)actor_num
                 << " script" << endl;
    return(false);
}
