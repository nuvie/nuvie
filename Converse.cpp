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

using std::cerr;
using std::cin;
using std::endl;

//#define CONVERSE_DEBUG
#ifdef CONVERSE_DEBUG
# define test_msg(S) print(S)
#else
# define test_msg(S) scroll->display_string("")
#endif

// TODO: uniform (better) text output
//       fix overflow at end of Chuckles(10) script
//       the "scope" system for skipping blocks of script is annoying, change it
//       everything you know about u6-scripting is wrong! need to rewrite this
//        with new knowledge

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
#ifdef CONVERSE_DEBUG
    std::cerr << "Converse: load \"" << convfilename << "\"" << std::endl;
#endif
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
                   MsgScroll *ioobj, ActorManager *actormgr, GameClock *c,
                   Player *p, ViewManager *viewmgr, ObjManager *objmgr)
{
    // get global objects
    config = cfg;
    scroll = ioobj;
    actors = actormgr;
    clock = c;
    player = p;
    views = viewmgr;
    objects = objmgr;
    // initialization
    interpreter = engine_type;
    src = 0; src_num = 0;
    npc = 0;
    script.buf = 0;
    strings = 0;
    rstr = 0;
    my_desc = 0;
    reset();
}


/* Check that loaded converse library (the source) contains `script_num'.
 * Load another source if it doesn't, and update `script_num' to item number.
 */
void Converse::set_conv(Uint32 &script_num)
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


/* Copy the NPC num's name from their conversation script.
 * Returns the name as a non-modifiable string of 16 characters maximum.
 */
const char *Converse::npc_name(uint8 num)
{
    Uint32 c;
    convscript temp_script;
    unsigned char *s_pt;
    if(num == 0 || num == 1)
        return(player->get_name());
    // load script if not for current NPC
    if(num != npc_num)
    {
        Uint32 temp_num = num;
        temp_script = read_script(temp_num);
        s_pt = temp_script.buf;
    }
    else
        s_pt = script.buf;
    // read name up to LOOK section, convert "_" to "."
    for(c = 0; s_pt[c+2] != 0xf1 && s_pt[c+2] != 0xf3 && c <= 15; c++)
        aname[c] = s_pt[c+2] != '_' ? s_pt[c+2] : '.';
    aname[c <= 15 ? c : 15] = '\0';
    // unload script if not for current NPC
    if(s_pt != script.buf)
        free(s_pt);
    else
        strcpy(my_name, aname);
    return(aname);
}


/* Send output buffer or some other text to message scroller.
 */
void Converse::print(const char *printstr)
{
    if(!printstr)
        printstr = output.c_str();
#ifdef CONVERSE_DEBUG /* and optionally send it to stderr */
    uint32 dplen = strlen(printstr), slen = strlen(printstr), d = 0;
    char *dprint = (char *)malloc(dplen);
    for(int c = 0; c < slen; c++)
    {
        if(printstr[c] == '\n')
        {
            dplen += 1;
            dprint = (char *)realloc(dprint, dplen);
            dprint[d++] = '\\';
            dprint[d++] = 'n';
        }
        else
            dprint[d++] = printstr[c];
    }
    dprint[d] = '\0';
    fprintf(stderr,"\nConverse: PRINT \"%s\"\n\n", dprint);
    free(dprint);
#endif
    scroll->display_string((char *)printstr);
    if(printstr == output.c_str())
        output.resize(0);
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


/* Returns string data from block at `loc', element index `i'.
 * FIXME: doesn't handle script overflow in an element
 * Returns NULL if the location or the requested item are out of script range.
 */
char *Converse::get_dbstr(uint32 loc, uint32 i)
{
    convscript_p db = &(script.buf[loc]); /* start of element */
    char *val = NULL; /* buffer */
    uint32 len = 0, e = 0; /* length of buffer, element index */
    do
    {
        // get element length (at least 1)
        // skip required first byte (it can be 0)
        for(len = 1; db[len] != 0x00; len++);
        // copy and return
        if(e++ == i)
        {
            val = (char *)malloc(len + 1);
            memcpy(val, db, len);
            val[len] = '\0';
            return(val);
        }
        db += (len + 1); // skip to start of next
    } while(db < (script.buf + script.buf_len));
    return(NULL);
}


/* Returns integer data from block at `loc', element index `i'.
 * FIXME: doesn't handle script overflow in an element
 * Returns 0 if the location or the requested item are out of script range.
 */
uint8 Converse::get_dbint(uint32 loc, uint32 i)
{
    convscript_p db = &(script.buf[loc]); /* start of element */
    uint32 len = 0, e = 0; /* length of buffer, element index */
    do
    {
        // get element length (at least 1)
        // skip required first byte (it can be 0)
        for(len = 1; db[len] != 0x00; len++);
        // copy and return
        if(e++ == i)
            return((uint8)(db[0]));
        db += (len + 1); // skip to start of next
    } while(db < (script.buf + script.buf_len));
    return(0);
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
                    output.append(player->get_gender_title());
                else if(!strcmp(symbol, "$N")) // npc name
                    output.append(my_name);
                else if(!strcmp(symbol, "$P")) // player name
                    output.append(player->get_name());
                else if(!strcmp(symbol, "$T")) // time of day
                    output.append(clock->get_time_of_day_string());
                else if(!strcmp(symbol, "$Y")) // Y-string
                    output.append(ystr ? ystr : "");
                else if(!strcmp(symbol, "$Z")) // previous input
                    output.append(input_s);
                // FIXME: only converts 1 character variable numbers
                else if(symbol[0] == '$' // value of a string variable
                        && isdigit(symbol[1]))
                {
                    if(get_svar(strtol(&symbol[1], NULL, 10)))
                        output.append(get_svar(strtol(&symbol[1], NULL, 10)));
                    else
                        output.append("");
                }
                else if(symbol[0] == '#' // value of a variable
                        && isdigit(symbol[1]))
                {
                    snprintf(intval, 16, "%d",
                             get_var(strtol(&symbol[1], NULL, 10)));
                    output.append(intval);
                }
                else
                    output.append(symbol);
                c += 2;
            }
            else
            {
                output.append(1, (unsigned char)peek(c));
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


/* Show portrait for npc `n'. The name will be shown for actors in the player
 * party, or those the player/avatar has met. The look-string will be shown for
 * anyone else.
 */
void Converse::show_portrait(uint8 n)
{
    Actor *actor = (n == npc_num) ? npc : actors->get_actor(n);
    const char *nameret = 0;
    if(!actor)
        return;
    if((actor->get_flags() & 1) || player->get_party()->contains_actor(actor))
        nameret = npc_name(n);
    else
        nameret = actors->actor_lookstr(actor);
    views->set_portrait_mode(n, (char *)nameret);
}


/* Tests/Compares values from a stack, using a comparison function code. The
 * number of values needed for the comparison will be popped from the stack. An
 * evaluation code can also be used.
 * Returns the result of the comparison. True returns 1 and false returns 0.
 */
uint32 Converse::u6op_if_test(uint32 cmpf, stack<uint32> *cmpv)
{
    uint32 ifcomp = 0;
    Actor *cnpc = 0;
    uint32 v[2]; // popped vals
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: cmp: func=0x%02x", cmpf);
    stack<uint32> p_stk; // have to copy values here to print stack
    while(!cmpv->empty())
    {
        p_stk.push(cmpv->top());
        cmpv->pop();
    }
    int vi = 1;
    while(!p_stk.empty())
    {
        cmpv->push(p_stk.top());
        p_stk.pop();
        fprintf(stderr, " v%d=0x%02x", vi++, cmpv->top());
    }
    fprintf(stderr, "\n");
#endif
    switch(cmpf)
    {
        case 0x81: // val1 > val2
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] > v[1])
                ifcomp = 1;
            break;
        case 0x82: // ?? >
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] > v[1])
                ifcomp = 1;
            break;
        case 0x83: // val1 < val2
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] < v[1])
                ifcomp = 1;
            break;
        case 0x84: // val1 < val2
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] < v[1])
                ifcomp = 1;
            break;
        case 0x85: // val1 != val2
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] != v[1])
                ifcomp = 1;
            break;
        case 0x86: // val1 == val2
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] == v[1])
                ifcomp = 1;
            break;
        case 0x94: // (val1) OR (val2)
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] || v[1])
                ifcomp = 1;
            break;
        case 0x95: // (val1) AND (val2)
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[0] && v[1])
                ifcomp = 1;
            break;
        case 0xab: // is npc(val1) flag(val2) set?
            v[1] = cmpv->top(); cmpv->pop();
            v[0] = cmpv->top(); cmpv->pop();
            if(v[1] <= 7)
            {
                if(v[0] == 0xeb)
                    cnpc = npc;
                else
                    cnpc = actors->get_actor(v[0]);
                if(cnpc->get_flags() & (1 << v[1]))
                    ifcomp = 1;
            }
            break;
        case 0xc6: // is val1 # of npc in party?
            v[0] = cmpv->top(); cmpv->pop();
            if(player->get_party()->contains_actor(v[0]))
                ifcomp = 1;
            break;
        case 0xd7: // is val1 # of an npc nearby self?
            v[0] = cmpv->top(); cmpv->pop();
            cnpc = actors->get_actor(v[0]);
            if(cnpc)
                ifcomp = npc->is_nearby(cnpc) ? 1 : 0;
            break;
        case 0xda: // is npc val1 wounded?
            v[0] = cmpv->top(); cmpv->pop();
            cnpc = actors->get_actor(get_npc_num(v[0]));
            if(cnpc)
                if(cnpc->get_hp() < cnpc->get_maxhp())
                    ifcomp = 1;
            break;
        case 0xdc: // is npc val1 poisoned?
            v[0] = cmpv->top(); cmpv->pop();
            cnpc = actors->get_actor(get_npc_num(v[0]));
            // FIXME
            ifcomp = 0;
            break;
        default: // try eval op, or just return 0
            ifcomp = u6op_assign_eval(cmpf, cmpv);
            break;
    }
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: cmp: result=0x%02x\n", ifcomp);
#endif
    return(ifcomp);
}


/* Combine values from a stack, using a math function code.
 * The number of values needed for the operation will be popped from the stack.
 * Returns the operation result.
 */
uint32 Converse::u6op_assign_eval(uint32 opf, stack<uint32> *opv)
{
    Actor *cnpc = 0;
    uint32 res = 0, v[2]; // result, popped vals
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: eval: func=0x%02x", opf);
    stack<uint32> p_stk; // have to copy values here to print stack
    while(!opv->empty())
    {
        p_stk.push(opv->top());
        opv->pop();
    }
    int vi = 1;
    while(!p_stk.empty())
    {
        opv->push(p_stk.top());
        p_stk.pop();
        fprintf(stderr, " v%d=0x%02x", vi++, opv->top());
    }
    fprintf(stderr, "\n");
#endif
    // evaluate
    switch(opf)
    {
        case 0x90: // val1 + val2
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            res = v[0] + v[1];
            break;
        case 0x91: // val1 - val2
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            res = v[0] - v[1];
            break;
        case 0x92: // val1 * val2
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            res = v[0] * v[1];
            break;
        case 0x9a: // weight that npc val1 is free to carry
            v[0] = opv->top(); opv->pop();
            cnpc = actors->get_actor(v[0]);
            if(cnpc)
               res = (cnpc->get_strength() * 2 * 10) - (uint32)(cnpc->get_inventory_weight() * 10);
//printf("9A: weight that npc %d can carry: %d\n", v[0], res);
//printf("    (max weight: %d, total weight: %d)\n", cnpc->get_strength() * 2 * 10, (uint32)(cnpc->get_inventory_weight() * 10));
            break;
        case 0x9b: // weight of object val1, times val2(quantity)
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            res = objects->get_obj_weight(v[0]) * v[1];
//printf("9B: weight of object %d * %d: %d\n", v[0], v[1], res);
            break;
        case 0xa0: // random number between val1 and val2 inclusive
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            res = rnd(v[0], v[1]);
            break;
        case 0xab: // flag val2 of npc val1
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            cnpc = actors->get_actor(v[0]);
            if(cnpc)
                res = (cnpc->get_flags() & (1 << v[1]))
                       ? 1 : 0;
            break;
        case 0xb4: // indexed data val2 at script location val1
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            if(declared >= 0 && declared_t == 0xb3)
                set_rstr(get_dbstr(v[0], v[1]));
            else
                res = get_dbint(v[0], v[1]);
            break;
        case 0xbb: // quantity of object val2 on npc val1
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            cnpc = actors->get_actor(v[0]);
            if(cnpc)
                res = cnpc->inventory_count_object(v[1], 0);
            break;
        case 0xc7: // unknown, combines two vals, related to in-inventory check
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            // ??
            if(player->get_actor()->inventory_get_object(v[0], v[1]))
                res = 0xFFFF;
            else
                res = 0x8001;
            break;
        case 0xdd: // id of party member val1(0=leader), val2=??
            v[1] = opv->top(); opv->pop();
            v[0] = opv->top(); opv->pop();
            cnpc = player->get_party()->get_actor(v[0]);
            res = cnpc ? cnpc->get_actor_num() : 0;
            break;
        default:
            print("\nError: Unknown operation\n");
            break;
    }
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: eval: result=0x%02x\n", res);
    if(rstr)
        fprintf(stderr, "Converse: eval: sresult=\"%s\"\n", rstr);
#endif
    return(res);
}


/* Stack all values from an argument in the argument list and run an assign
 * -evaluation on them. The original values are replaced with the result.
 */
void Converse::eval_arg(uint8 argn, bool test_first)
{
    uint8 v = 0; // value index
    uint32 op_v = 0; // value
    converse_arg replace;
    stack<uint32> *op_stk = new stack<uint32>;
    // add values left to right to op-stack
    do
    {
        op_v = get_val(argn, v);
        // do op-func. with stack values and push result
        if(is_test((Uint8)op_v) && !get_valt(argn, v))
            op_stk->push(test_first ? u6op_if_test(op_v, op_stk)
                                    : u6op_assign_eval(op_v, op_stk));
        else
            op_stk->push(op_v);
    } while(++v < val_count(argn));
    // result of last op is new value
    replace.val = op_stk->top();
    replace.valt = U6OP_UINT32;
    // remove all values but first, and replace it
    args[argn].resize(1);
    set_val(argn, 0, replace);
    delete op_stk;
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
//    Party *group = 0;
    uint32 res = 0; // some operation result
#ifdef CONVERSE_DEBUG
fprintf(stderr, "Converse: %04x: 0x%02x\n", cmd_offset, cmd);
if(!args.empty() && !args[0].empty())
{
    fprintf(stderr, "Converse: args:");
    for(unsigned int a = 0; a < args.size() && !args[a].empty(); a++)
    {
        fprintf(stderr, " ");
        for(unsigned int v = 0; v < args[a].size(); v++)
            fprintf(stderr, "(0x%02x)0x%02x", args[a][v].valt, args[a][v].val);
    }
    fprintf(stderr, "\n");
}
#endif
    if(!check_scope())
        return(donext);
    switch(cmd)
    {
        case U6OP_IF: // 1 arg, with multiple vals last val is test type
            eval_arg(0, true);
            ifcomp = get_val(0, 0) ? true : false;
            enter_scope(ifcomp ? CONV_SCOPE_IF : CONV_SCOPE_IFELSE);
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
            let(get_rval(0, 0), get_valt(0, 0));
            if(declared > CONV_VAR__LAST_)
            {
                print("\nError: Illegal var. num\n");
                break;
            }
            break;
        case U6OP_ASSIGN: // 1 arg with assignment values/operations
        case U6OP_YASSIGN:
            eval_arg(0);
            res = get_val(0, 0);
            // assign result to declared var or str
            if(cmd == U6OP_ASSIGN)
            {
                if(declared >= 0 && declared <= CONV_VAR__LAST_)
                {
                    if(declared_t == 0xb3)
                        set_svar(declared, rstr);
                    else
                        set_var(declared, res);
                }
                else
                    print("\nError: Unknown variable\n");
            }
            else // assign name of npc `result' to ystr
            {
                // get name from party information
                if(player->get_party()->contains_actor(res))
                {
                    res = player->get_party()->get_member_num(res);
                    set_ystr(player->get_party()->get_actor_name(res));
                }
                else
                    set_ystr(npc_name(res)); // read from script
            }
            let(-1);
            set_rstr(NULL);
            break;
        case U6OP_ARGSTOP: // should be removed by parser
            test_msg("\n-EOA-\n");
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
#ifdef CONVERSE_DEBUG
            fprintf(stderr, "Converse: JUMP 0x%x (script: 0x%08x->0x%08x)\n",
                    get_val(0, 0), script_pt, script.buf + get_val(0, 0));
#endif
            seek(get_val(0, 0));
            break_scope(scope.size());
            break;
        case U6OP_BYE:
            stop();
            donext = false;
            break;
        case U6OP_NEW: // 4 args, npc, objnum, qual, quant
//bool Actor::inventory_add_object(uint16 obj_n, uint8 qty, uint8 quality);
            eval_arg(0);
            eval_arg(1);
            eval_arg(2);
            eval_arg(3);
            cnpc = actors->get_actor(get_npc_num(get_val(0, 0)));
            if(cnpc)
                cnpc->inventory_add_object(get_val(1, 0), get_val(3, 0),
                                           get_val(2, 0));
            break;
        case U6OP_DELETE: // 4 args, npc, objnum, qual, quant
//bool Actor::inventory_del_object(uint16 obj_n, uint8 qty, uint8 quality);
            eval_arg(0);
            eval_arg(1);
            eval_arg(2);
            eval_arg(3);
            cnpc = actors->get_actor(get_npc_num(get_val(0, 0)));
            if(cnpc)
                cnpc->inventory_del_object(get_val(1, 0), get_val(3, 0),
                                           get_val(2, 0));
            break;
        case U6OP_GIVE: // 4 args, objnum, qual, fromnpc, tonpc
            eval_arg(0);
            eval_arg(1);
            eval_arg(2);
            eval_arg(3);
            cnpc = actors->get_actor(get_npc_num(get_val(2, 0)));
            if(!cnpc)
                break;
            cnpc->inventory_del_object(get_val(0, 0), 1, get_val(1, 0));
            cnpc = actors->get_actor(get_npc_num(get_val(3, 0)));
            if(cnpc)
                cnpc->inventory_add_object(get_val(0, 0), 1, get_val(1, 0));
            break;
        case U6OP_PORTRAIT: // 1 arg, npc number
            show_portrait(get_npc_num(get_val(0, 0)));
            break;
        case U6OP_HEAL: // 1 arg, npc number
            eval_arg(0);
            cnpc = actors->get_actor(get_npc_num(get_val(0, 0)));
            if(cnpc)
                cnpc->set_hp(cnpc->get_maxhp());
            break;
        case U6OP_CURE: // 1 arg, npc number
            eval_arg(0);
            print("-!cure-"); // UNPOISON actor
            break;
        case U6OP_WORKTYPE: // 2 args, npc number, new worktype
            eval_arg(0);
            eval_arg(1);
            cnpc = actors->get_actor(get_npc_num(get_val(0, 0)));
            if(cnpc)
                cnpc->set_worktype(get_val(1, 0));
            break;
        case U6OP_INVENTORY:
            print("-!inventory-");
            break;
        case U6OP_WAIT: // set page-break on scroller and wait
            scroll->display_string("*");
            wait();
            donext = false;
            break;
        case U6OP_ENDASK:
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
        case U6OP_SIDENT: // arg 1 is id number, name follows
            break_scope(); // SEEKIDENT
            if(get_val(0, 0) != npc_num)
                fprintf(stderr,
                        "Converse: warning: npc number in script (%d) does not"
                        " match real number (%d)\n", get_val(0, 0), npc_num);
            strcpy(my_name, get_text()); // collected
            print(my_name);
            print("\n\n");
            donext = false;
            break;
        case U6OP_SLOOK: // description follows
            break_scope(); // SEEKLOOK
            my_desc = strdup(get_text()); // collected
            print("You see ");
            print(my_desc);
            print("\n");
            donext = false;
            break;
        case U6OP_SCONVERSE:
            break_scope(); // SEEKCONV
            donext = false;
            break;
        case U6OP_SPREFIX: // Unknown
            donext = false;
            break;
        case U6OP_ASK:
            scroll->display_string("\nyou say:");
            scroll->set_input_mode(true);
            wait_for_input();
            donext = false;
            break;
        case U6OP_ASKC:
            scroll->set_input_mode(true, get_text(), false);
            wait_for_input();
            donext = false;
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
            let(get_rval(0, 0), 0xb2);
            scroll->set_input_mode(true);
            wait_for_input();
            donext = false;
            break;
        case U6OP_INPUTC: // 1 val, variable to store input at
            let(get_rval(0, 0), 0xb2);
            scroll->set_input_mode(true, "0123456789");
            wait_for_input();
            donext = false;
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
        if((is_print(val) || is_cmd(val))
           && (check_overflow(1) || !is_valtype(peek(1)))) // next is not 0xb2,0xb3)
        {
//          std::cerr << "val is printable or cmd, done" << std::endl;
            break;
        }
//            std::cerr << "skip, get val" << std::endl;
        skip();
        if(val == U6OP_UINT32)
        {
            args[ai].resize(vi + 1);
            args[ai][vi].valt = U6OP_UINT32;
            args[ai][vi++].val = pop4();
//          std::cerr << "popped 4, next val" << std::endl;
        }
        else if(val == U6OP_UINT8)
        {
            args[ai].resize(vi + 1);
            args[ai][vi].valt = U6OP_UINT8;
            args[ai][vi++].val = pop();
//			fprintf(stderr, "popped 1 as val: 0x%02x\n", args[ai][vi-1].val);
        }
        else if(val == U6OP_UINT16)
        {
            args[ai].resize(vi + 1);
            args[ai][vi].valt = U6OP_UINT16;
            args[ai][vi++].val = pop2();
 //         std::cerr << "popped 2, next val" << std::endl;
        }
        else
        {
//          std::cerr << "val is val" << std::endl;
            args[ai].resize(vi + 1);
			args[ai][vi].valt = 0;
            args[ai][vi].val = val;
            if(is_valtype(peek())) // 0xb2,0xb3
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
 * text. A count of 0 means to keep going if possible. Stepping will stop if
 * control command `bcmd' is encountered (if it's not 0x00.)
 */
void Converse::step(Uint32 count, Uint8 bcmd)
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

        if(bcmd != 0x00 && peek() == bcmd) // break on `bcmd'
            break;
        cmd = pop();
        cmd_offset = (Uint32)(script_pt - 1 - script.buf);
        if(cmd == U6OP_ASK || cmd == U6OP_ASKC)
            getinput_offset = cmd_offset;
        // get args
        args.clear(); args.resize(1);
        if(cmd == U6OP_JUMP)
        {
            args[0].resize(1);
            args[0][0].valt = 0;
            args[0][0].val = pop4();
        }
        else if(cmd == U6OP_SIDENT) // get name
        {
            args[0].resize(1);
            args[0][0].valt = 0;
            args[0][0].val = pop(); // NPC id number
            collect_text(false); // name
        }
        else if(cmd == U6OP_KEYWORD  // get keyword list
                || cmd == U6OP_ASKC  // ...or characters allowed as input
                || cmd == U6OP_SLOOK) // ...or description
	{
	    collect_text(false);
	}
        else
            collect_args();
        stepping = do_cmd();
    }
    if(check_overflow())
    {
        print("\n-EOF-\n");
#ifdef CONVERSE_DEBUG
        fprintf(stderr, "Converse: EOF: script=%08x, script_len=%d, script_pt=%08x\n",
                script.buf, script.buf_len, script_pt);
#endif
        stop();
    }
}


/* Reset or initialize data that changes from npc to npc.
 */
void Converse::reset()
{
    npc = 0;
    free(script.buf);
    script_pt = script.buf = 0;
    script.buf_len = 0;
    npc_num = 0;
    free(my_desc);
    my_desc = 0;
    if(!heap.empty())
        save_variables();
    heap.clear();
    if(strings)
    {
        int s = 0;
        while(s < strings->size())
            set_svar(s++, NULL);
        delete strings; strings = 0;
    }
    let(-1);
    set_rstr(NULL);
    set_ystr(NULL);

    input_s.resize(0); // input_s.erase();
    keywords.resize(0);
    args.clear();
    while(!scope.empty())
        scope.pop();

    active = is_waiting = need_input = false;
}


/* Stop execution of the current script.
 */
void Converse::stop()
{
    reset();
    fprintf(stderr, "End conversation\n");
}


/* Set the initial variables that an npc script can access.
 */
void Converse::init_variables()
{
    // FIXME: the U6 max. var num is unknown, in this implementation can be no
    // greater than one less than the first command code.
    heap.resize(CONV_VAR__LAST_ + 1);
    for(int v = 0; v <= CONV_VAR__LAST_; v++)
        heap[v].valt = heap[v].val = 0x00;
    heap[CONV_VAR_SEX].val = player->get_gender();
    heap[CONV_VAR_PARTYSIZE].val = player->get_party()->get_party_size();
    heap[CONV_VAR_PARTYSIZET].val = heap[CONV_VAR_PARTYSIZE].val;
    heap[CONV_VAR_WORKTYPE].val = npc->get_worktype();
}


/* Copy the script variable values to the appropriate (original) locations.
 */
void Converse::save_variables()
{
    if(player)
        player->set_gender(heap[CONV_VAR_SEX].val);
    if(npc)
    {
        npc->set_worktype(heap[CONV_VAR_WORKTYPE].val);
    }
}


/* Read (decode if necessary) an NPC conversation from the correct file. The
 * script number is updated to the actual number in the source read from.
 * Returns a new script structure.
 */
convscript Converse::read_script(Uint32 &n)
{
    unsigned char *undec_script = 0; // item as it appears in library
    unsigned char *dec_script = 0; // decoded
    convscript read_script; // final item
    Uint32 undec_len = 0, dec_len = 0;
    U6Lzw decoder;

    set_conv(n); // gets updated n
    undec_len = src->get_item_size(n);
    if(undec_len > 4)
    {
        fprintf(stderr, "Reading");
        undec_script = src->get_item(n);
        // decode
        if(!(undec_script[0] == 0 && undec_script[1] == 0
             && undec_script[2] == 0 && undec_script[3] == 0))
        {
            fprintf(stderr, " encoded npc script");
            dec_script =
                    decoder.decompress_buffer(undec_script, undec_len, dec_len);
            free(undec_script);
        }
        else
        {
            fprintf(stderr, " unencoded npc script");
            dec_script = undec_script;
            dec_len = undec_len;
        }
        fprintf(stderr, " (%s:%d)\n",
                src_num == 1 ? "converse.a" : "converse.b", (unsigned int)n);
    }
    read_script.buf = dec_script;
    read_script.buf_len = dec_len;
    return(read_script);
}


/* Returns false if a conversation cannot be started with NPC `talkto'. This
 * represents an internal error, and doesn't have anything to do with the NPC
 * not wanting/being able to talk to the Avatar.
 */
bool Converse::start(Actor *talkto)
{
  //  unsigned char *undec_script = 0; // item as it appears in library
    Uint32 actor_num = 0, script_num = 0;
    if(!talkto || !src)
        return(false);

    // make sure previous script is unloaded first
    if(active)
        this->stop();
    script_num = actor_num = talkto->get_actor_num();
    npc = talkto;
    init_variables();
    script = read_script(script_num);
    // start script (makes step() available)
    if(script.buf)
    {
        active = true;
        npc_num = actor_num;
        seek(0);
        enter_scope(CONV_SCOPE_SEEKLOOK);
        fprintf(stderr, "Begin conversation with \"%s\" (npc %d)\n",
                npc_name(actor_num), actor_num);
#ifdef CONVERSE_DEBUG
        fprintf(stderr, "Converse: script=%08x, script_len=%d\n", script.buf, script.buf_len);
#endif
        show_portrait(npc_num);
        return(true);
    }
    fprintf(stderr, "Error loading npc %d, script %s:%d\n", actor_num,
            src_num == 1 ? "converse.a" : "converse.b", script_num);
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
        else if(need_input && scroll->get_input())
        {
            scroll->display_string("\n\n");
            input(scroll->get_input());
            if(input_s.empty())
                input("bye");
            else if(input_s == "look")
            {
                print("You see ");
                print(my_desc);
                seek(getinput_offset);
            }
#ifdef CONVERSE_DEBUG
            fprintf(stderr, "\nConverse: INPUT \"%s\"\n\n", input_s.c_str());
#endif
            // assign value to declared input variable
            set_var(declared, strtol(input_s.c_str(), NULL, 10));
            let(-1);
            unwait();
        }
        else if(!need_input && !scroll->get_page_break())
        {
            // if page unbroken, unpause script
            unwait();
        }
        // script has stopped itself:
        if(!running())
        {
            scroll->set_talking(false);
            scroll->display_string("\n");
            scroll->display_prompt();
            views->set_inventory_mode(0);
        }
    }
}
