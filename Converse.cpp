/* Created by Joseph Applegate
 * Copyright (C) 2003 The Nuvie Team
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
#include <cstdlib>
#include <cctype>
#include <cstring>

#include "nuvieDefs.h"
#include "Game.h"
#include "Configuration.h"
#include "U6Lzw.h"
#include "Player.h"
#include "Party.h"
#include "ViewManager.h"
#include "ActorManager.h"
#include "SoundManager.h"
#include "Event.h"
#include "ConverseInterpret.h"
#include "Converse.h"

//#define CONVERSE_DEBUG


Converse::Converse()
{
    config = NULL;
    clock = NULL;
    actors = NULL;
    objects = NULL;
    player = NULL;
    views = NULL;
    scroll = NULL;

    conv_i = NULL;
    script = NULL;
    npc = NULL;
    npc_num = 0;
    name = desc = NULL;
    src = NULL;
    src_num = 0;

    allowed_input = NULL;

    active = false;
    variables = NULL;
    party_all_the_time = false;
}


/* Initialize global classes from the game.
 */
void
Converse::init(Configuration *cfg, nuvie_game_t t, MsgScroll *s,ActorManager *a,
               GameClock *c, Player *p, ViewManager *v, ObjManager *o)
{
    config = cfg;
    scroll = s;
    actors = a;
    clock = c;
    player = p;
    views = v;
    objects = o;
    gametype = t;

    cfg->value("config/party_all_the_time", party_all_the_time);
}


Converse::~Converse()
{
    if(running())
    {
        reset();
        fprintf(stderr, "End conversation\n");
    }
    unload_conv();
}


/* Free up allocated memory, reset values for new conversation. (call only when
 * ending a conversation or quitting)
 */
void Converse::reset()
{
    delete conv_i; conv_i = NULL;
    set_input(""); // delete
    set_output(""); // clear output
    if(name)
    {
        free(name);
        name = NULL;
    }
    if(desc)
    {
        free(desc);
        desc = NULL;
    }
    if(allowed_input)
    {
        free(allowed_input);
        allowed_input = NULL;
    }
    delete_variables();
}


/* Load `convfilename' as src.
 */
void Converse::load_conv(const std::string &convfilename)
{
    string conv_lib_str;
    config->pathFromValue("config/ultima6/gamedir", convfilename, conv_lib_str);
    unload_conv();
    src_num = 0;
    if(gametype == NUVIE_GAME_U6)
    {
        src = new U6Lib_n;
        src->open(conv_lib_str, 4);
        src_num = (convfilename == "converse.a") ? 1 : (convfilename == "converse.b") ? 2 : 0;
    }
/*  else if(gametype == NUVIE_GAME_MD)
    {
        src = new U6Lib_n(?);
        src->open(conv_lib_str, ?);
    }
    else if(gametype == NUVIE_GAME_SE)
    {
    } */
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: load \"%s\"\n", convfilename.c_str());
#endif
}


/* Check that loaded converse library (if any) has script for npc `a'. Load
 * another file if it doesn't.
 * Returns the real item number in the source.
 */
uint32 Converse::load_conv(uint8 a)
{
    if(a <= 98)
    {
        if(src_num != 1)
            load_conv("converse.a");
        return(a);
    }
    else // a >= 99
    {
        if(src_num != 2)
            load_conv("converse.b");
        if(a > 200) // (quick fix for U6: anything over 200 is a temporary npc)
        {
            Actor *npc = actors->get_actor(a);
            if(npc->get_obj_n() == 373) // OBJ_U6_WISP
                a = 201;
            else if(npc->get_obj_n() == 382) // OBJ_U6_GUARD
                a = 202;
        }
        else if(a == 188)  // U6: temp. fix for shrines
            a = 191; // ??? -> Exodus
        else if(a >= 191 && a <= 197) // shrines except spirituality & humility
            a += 2;
        else if(a == 198)
            a = 192; // Spirituality -> Honesty
        else if(a == 199)
            a = 200; // Humility -> Singularity
        return(a - 99);
    }
}


/* Returns name of loaded source file, identified by `src_num'.
 */
const char *Converse::src_name()
{
    if(src_num == 0)
        return("");
    if(gametype == NUVIE_GAME_U6)
        return((src_num == 1) ? "converse.a" : "converse.b");
    if(gametype == NUVIE_GAME_MD)
        return("talk.lzc");
    if(gametype == NUVIE_GAME_SE)
        return("talk.lzc");

 return("");
}


/* Get an NPC conversation from the source file.
 * Returns new ConvScript object.
 */
ConvScript *Converse::load_script(uint32 n)
{
    ConvScript *loaded = new ConvScript(src, n);
    if(!loaded->loaded())
    {
        delete loaded;
        loaded = NULL;
    }
    else
        fprintf(stderr, "Read %s npc script (%s:%d)\n",
                loaded->compressed ? "encoded" : "unencoded", src_name(), (unsigned int)n);
    return(loaded);
}


/* Initialize Converse variable list, and set globals from game.
 */
void Converse::init_variables()
{
    variables = new converse_variables_s[U6TALK_VAR__LAST_ + 1];
    for(uint32 v = 0; v <= U6TALK_VAR__LAST_; v++)
    {
        variables[v].cv = 0;
        variables[v].sv = NULL;
    }
    set_var(U6TALK_VAR_SEX, player->get_gender());
    set_var(U6TALK_VAR_KARMA, player->get_karma());
    set_var(U6TALK_VAR_PARTYLIVE, player->get_party()->get_party_size() - 1);
    // FIXME: count dead party members in PARTYALL, not in PARTYLIVE
    set_var(U6TALK_VAR_PARTYALL, get_var(U6TALK_VAR_PARTYLIVE));
    set_var(U6TALK_VAR_HP, player->get_actor()->get_hp());
    set_var(U6TALK_VAR_QUESTF, player->get_quest_flag());
    set_var(U6TALK_VAR_WORKTYPE, npc->get_worktype());
}


/* Free memory used by Converse variable list.
 */
void Converse::delete_variables()
{
    player->set_quest_flag((uint8)get_var(U6TALK_VAR_QUESTF));

    for(uint32 v = 0; v <= U6TALK_VAR__LAST_; v++)
        if(variables[v].sv)
            free(variables[v].sv);
    delete [] variables;
    variables = NULL;
}


/* Create new script interpreter for the current game.
 * Returns pointer to object which is derived from ConverseInterpret.
 */
ConverseInterpret *Converse::new_interpreter()
{
    ConverseInterpret *ci = NULL;
    switch(gametype)
    {
        case NUVIE_GAME_U6:
            ci = (ConverseInterpret*)new U6ConverseInterpret(this);
            break;
//        case NUVIE_GAME_MD:
//            ci = (ConverseInterpret *)new MDTalkInterpret(this);
//            break;
//        case NUVIE_GAME_SE:
//            ci = (ConverseInterpret *)new SETalkInterpret(this);
//            break;
    }
    return(ci);
}


/* Returns false if a conversation cannot be started with the NPC. This
 * represents an internal error, and doesn't have anything to do with the NPC
 * not wanting/being able to talk to the Avatar.
 */
bool Converse::start(uint8 n)
{
    uint32 script_num = 0;

    // load, but make sure previous script is unloaded first
    if(running())
        stop();
    if(!(npc = actors->get_actor(n)))
        return(false);
    // get script num for npc number (and open file)
    script_num = load_conv(n);
    if(!src)
        return(false);
    script = load_script(script_num);

    // begin
    if(script)
    {
        active = true;
        if(!(conv_i = new_interpreter()))
        {
            fprintf(stderr, "Can't talk: Unimplemented or unknown game type\n");
            return(false);
        }
        // set current NPC and start conversation
        npc_num = n;
        init_variables();
        show_portrait(npc_num);
        Game::get_game()->get_sound_manager()->musicPause();
        Game::get_game()->get_event()->set_mode(WAIT_MODE); // ignore player actions
        unwait();
        fprintf(stderr, "Begin conversation with \"%s\" (npc %d)\n", npc_name(n), n);
        return(true);
    }
    fprintf(stderr, "Error loading npc %d from %s:%d\n",
            n, src_name(), script_num);
    return(false);
}


/* Stop execution of the current script.
 */
void Converse::stop()
{
    reset(); // free memory

    scroll->set_talking(false);
    scroll->display_string("\n");
    scroll->display_prompt();
    views->set_inventory_mode();
    Game::get_game()->get_sound_manager()->musicPlay();
    Game::get_game()->get_event()->set_mode(MOVE_MODE); // return control to player

    active = false;
    fprintf(stderr, "End conversation\n");
}


/* Returns true if there is input available (placed at `in_str'.)
 */
bool Converse::input()
{
    if(scroll->has_input())
    {
     std::string s = scroll->get_input();
        set_input(s);
#ifdef CONVERSE_DEBUG
        fprintf(stderr, "Converse: INPUT \"%s\"\n\n", get_input().c_str());
#endif
        return(true);
    }
    return(false);
}


/* Output string `s' or the current set output to the scroll view.
 */
void Converse::print(const char *s)
{
#ifdef CONVERSE_DEBUG
    fprintf(stderr, "Converse: PRINT \"%s\"\n\n",s ? s : get_output().c_str());
#endif
    if(s)
        scroll->display_string(s);
    else
        scroll->display_string(get_output());
}


/* Get string value of variable `varnum'.
 */
const char *Converse::get_svar(uint8 varnum)
{
    if(varnum <= U6TALK_VAR__LAST_ && variables[varnum].sv)
        return((const char *)variables[varnum].sv);
    return("");
}


/* Set string value of variable `varnum'.
 */
void Converse::set_svar(uint8 varnum, const char *set)
{
    if(varnum <= U6TALK_VAR__LAST_)
        variables[varnum].sv = strdup(set);
}


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
    if((actor->get_flags() & 1) || player->get_party()->contains_actor(actor)
         || (n >= 188 && n <= 200)) // always known NPCs
        nameret = npc_name(n);
    else
        nameret = actors->look_actor(actor, false);
    views->set_portrait_mode(actor, (char *)nameret);
}


/* Copy the NPC num's name from their conversation script. This is very U6
 * specific.
 * Returns the name as a non-modifiable string of 16 characters maximum.
 */
const char *Converse::npc_name(uint8 num)
{
    ConvScript *temp_script;
    convscript_buffer s_pt;
    aname[15] = '\0';

// FIX (crashing)
//    if(actors->get_actor(num))
//        actors->get_actor(num)->set_name(name);

    if(num == 0 || num == 1)
        strncpy(aname, player->get_name(), 15);
    // load another script, or use NPC name
    else if((num == npc_num) && name)
        strncpy(aname, name, 15);
    else
    {
//        uint32 temp_num = num;
        num = load_conv(num); // get idx number; won't actually reload file
        temp_script = new ConvScript(src, num);
        s_pt = temp_script->get_buffer();
        if(!s_pt)
            return(NULL);

        // read name up to LOOK section, convert "_" to "."
        uint32 c;
        for(c = 0; s_pt[c+2] != 0xf1 && s_pt[c+2] != 0xf3 && c <= 15; c++)
            aname[c] = s_pt[c+2] != '_' ? s_pt[c+2] : '.';

        aname[c] = '\0';
        delete temp_script;
    }
    return(aname);
}


/* Start checking i/o object for some input, (optionally block all but allowed
 * input) and tell interpreter to wait.
 */
void Converse::poll_input(const char *allowed, bool nonblock)
{
    if(allowed_input)
        free(allowed_input);
    allowed_input = NULL;
    allowed_input = (allowed && strlen(allowed)) ? strdup(allowed) : NULL;

    scroll->set_input_mode(true, allowed_input, nonblock);
    need_input = true;
    conv_i->wait();
}


/* Stop polling i/o, tell interpreter to stop waiting.
 */
void Converse::unwait()
{
    need_input = false;
    conv_i->unwait();
}


/* Check talk input and determine if it needs to be handled before being passed
 * to the interpreter.
 * Returns false if the conversation should be stopped.
 */
bool Converse::override_input()
{
    if(in_str.empty())
        in_str = "bye";
    else if(in_str == "look")
    {
        print("You see ");
        print(desc);
        script->seek(script->pos() - 1); // back to ASK command
    }
    else if(party_all_the_time && in_str == "join")
    {
        if(!player->get_party()->contains_actor(npc))
            player->get_party()->add_actor(npc);
        print("\"Friends of Nuvie? Sure, I'll come along!\"\n");
        return(false);
    }
    else if(party_all_the_time && in_str == "leave")
    {
        if(player->get_party()->contains_actor(npc))
            player->get_party()->remove_actor(npc);
        print("\"For Nuvie!\"\n");
        return(false);
    }
    return(true);
}


/* If not waiting, continue the active script. If waiting for input, check i/o
 * object (scroll), taking the input if available. Else wait until the scroll's
 * page is unbroken.
 */
void Converse::continue_script()
{
    if(running())
    {
        if(!conv_i->waiting())
            conv_i->step();
        else if(need_input && input())
        {
            print("\n\n");
            if(!override_input())
            {
                stop();
                return;
            }
            // assign value to declared input variable
            if(conv_i->var_input())
                conv_i->assign_input();
            set_svar(U6TALK_VAR_INPUT, get_input().c_str()); // set $Z
            unwait();
        }
        else if(!need_input && !scroll->get_page_break())
        {
            // if page unbroken, unpause script
            unwait();
        }
        // interpreter has stopped itself
        if(conv_i->end())
            stop();
    }
}


/*** ConvScript ***/

/* Init. and read data from U6Lib.
 */
ConvScript::ConvScript(U6Lib_n *s, uint32 idx)
{
    buf = NULL;
    buf_len = 0;
    src = s;
    src_index = idx;

    ref = 0;
    cpy = NULL;

    read_script();
    rewind();
}


/* Init. and use data from another ConvScript.
 */
ConvScript::ConvScript(ConvScript *orig)
{
    buf = NULL;
    buf_len = 0;

    cpy = orig;
    ref = 1;
    cpy->ref += 1;

    rewind();
}


ConvScript::~ConvScript()
{
    if(ref == 0)
        free(buf);
    else if(cpy)
        cpy->ref -= 1;
}


/* Read (decode if necessary) the script data (with the pre-set item index) from
 * the loaded converse library.
 */
void ConvScript::read_script()
{
    unsigned char *undec_script = 0; // item as it appears in library
    unsigned char *dec_script = 0; // decoded
    uint32 undec_len = 0, dec_len = 0;
    U6Lzw decoder;

    undec_len = src->get_item_size(src_index);
    if(undec_len > 4)
    {
        undec_script = src->get_item(src_index);
        // decode
        if(!(undec_script[0] == 0 && undec_script[1] == 0
             && undec_script[2] == 0 && undec_script[3] == 0))
        {
            compressed = true;
            dec_script =
                    decoder.decompress_buffer(undec_script, undec_len, dec_len);
            free(undec_script);
        }
        else
        {
            compressed = false;
            dec_len = undec_len - 4;
            dec_script = (unsigned char *)malloc(dec_len);
            memcpy(dec_script, undec_script + 4, dec_len);
            free(undec_script);
        }
    }
    if(dec_len)
    {
        buf = (convscript_buffer)dec_script;
        buf_len = dec_len;
    }
}


/* Returns 8bit value from current script location in LSB-first form.
 */
converse_value ConvScript::read(uint32 advance)
{
    uint8 val = 0;
    while(advance--)
    {
        val = *buf_pt;
        ++buf_pt;
    }
    return(val);
}


/* Returns 16bit value from current script location in LSB-first form.
 */
converse_value ConvScript::read2()
{
    uint16 val = 0;
    val = *(buf_pt++);
    val += *(buf_pt++) << 8;
    return(val);
}


/* Returns 32bit value from current script location in LSB-first form.
 */
converse_value ConvScript::read4()
{
    uint32 val = 0;
    val = *(buf_pt++);
    val += *(buf_pt++) << 8;
    val += *(buf_pt++) << 16;
    val += *(buf_pt++) << 24;
    return(val);
}

