#include <cassert>
#include "nuvieDefs.h"

#include "Game.h"
#include "Actor.h"
#include "MapWindow.h"
#include "Map.h"
#include "Party.h"
#include "Event.h"
#include "UseCode.h"
#include "U6LList.h"
#include "MsgScroll.h"
#include "GameClock.h"
// FIXME: effects use timers, not the other way around (make a movement effect?)
#include "EffectManager.h"
#include "Effect.h"

#include "TimedEvent.h"

#define MESG_TIMED CB_TIMED

/* Activate all events for the current time, deleting those that have fired
 * and are of no more use. Repeated timers are requeued.
 */
void TimeQueue::call_timers(uint32 now)
{
    while(!empty() && call_timer(now))
    {
        TimedEvent *tevent = pop_timer(); // remove

        if(tevent->repeat_count != 0) // repeat! same delay, add time
        {
            // use updated time so it isn't repeated too soon
            tevent->set_time();
//            tevent->time = clock->get_ticks() + tevent->delay;
//            tevent->time = now + tevent->delay;
            add_timer(tevent);
            if(tevent->repeat_count > 0) // don't reduce count if infinite (-1)
                --tevent->repeat_count;
        }
        else
            delete_timer(tevent); // if not repeated, safe to delete
    }
}


/* Add new timed event to queue, which will activate `event' when time is
 * `evtime'.
 */
void TimeQueue::add_timer(TimedEvent *tevent)
{
    std::list<TimedEvent *>::iterator t;
    if(tq.empty())
    {
        tq.push_front(tevent);
        return;
    }
    // in case it's already queued, remove the earlier instance(s)
    remove_timer(tevent);
    // add after events with earlier/equal time
    t = tq.begin();
    while(t != tq.end() && (*t)->time <= tevent->time) t++;
    tq.insert(t, tevent);
}


/* Remove timed event from queue.
 */
void TimeQueue::remove_timer(TimedEvent *tevent)
{
    std::list<TimedEvent *>::iterator t;
    t = tq.begin();
    while(t != tq.end())
      {
        if(*t == tevent)
        {
            t = tq.erase(t);
        }
        else ++t; // this deletes all duplicates
      }
}


/* Remove and return timed event at front of queue, or NULL if empty.
 */
TimedEvent *TimeQueue::pop_timer()
{
    TimedEvent *first = NULL;
    if(!empty())
    {
        first = tq.front();
        tq.pop_front(); // remove it
    }
    return(first);
}


/* Call timed event at front of queue, whose time is <= `now'.
 * ***Does not handle repeating timers***
 * Returns true if an event handler was called. (false if time isn't up yet)
 */
bool TimeQueue::call_timer(uint32 now)
{
    if(empty())
        return(false);
    TimedEvent *first = tq.front();
    if(first->defunct)
    {
        assert(pop_timer() == first);
        delete_timer(first);
        return(false);
    }
    if(first->time > now)
        return(false);

    first->timed(now); // fire
    return(true);
}


/* Delete a timer, if its can_delete flag is true. Remove from the queue first!
 */
bool TimeQueue::delete_timer(TimedEvent *tevent)
{
    if(tevent->tq_can_delete) {
        delete tevent;
        return true;
    }

    return false;
}


/* Accepts delay, immediate toggle(false), and realtime switch(true). It must
 * be queued afterwards to start.
 */
TimedEvent::TimedEvent(uint32 reltime, bool immediate, bool realtime)
            : delay(reltime), repeat_count(0), ignore_pause(false),
              real_time(realtime), tq_can_delete(true), defunct(false)
{
    tq = NULL;

    if(immediate) // start now (useful if repeat is true)
        time = 0;
    else
        set_time();
}


/* Add myself to the TimeQueue.
 */
void TimedEvent::queue()
{
    Event *event = Game::get_game()->get_event();
    if(tq == NULL)
    {
        if(real_time)
            tq = event->get_time_queue();
        else
            tq = event->get_game_time_queue();
        tq->add_timer(this);
    }
}


/* Remove myself from the TimeQueue.
 */
void TimedEvent::dequeue()
{
    if(tq)
    {
        tq->remove_timer(this);
        tq = NULL;
    }
}


/* Add delay to current time and set absolute time.
 */
void TimedEvent::set_time()
{
    GameClock *clock = Game::get_game()->get_clock();
    time = delay + (real_time ? clock->get_ticks()
                              : clock->get_game_ticks());

}


/*** TimedPartyMove ***/

/* Party movement to/from dungeon or moongate, with a certain number of
 * milliseconds between each step.
 */
TimedPartyMove::TimedPartyMove(MapCoord *d, MapCoord *t, uint32 step_delay)
                              : TimedEvent(step_delay, true)
{
    init(d, t, NULL);
}

/* Movement through temporary moongate.
 */
TimedPartyMove::TimedPartyMove(MapCoord *d, MapCoord *t, Obj *use_obj, uint32 step_delay)
                              : TimedEvent(step_delay, true)
{
    init(d, t, use_obj);
}

TimedPartyMove::~TimedPartyMove()
{
    delete dest;
    delete target;
}

/* Set destination.
 */
void TimedPartyMove::init(MapCoord *d, MapCoord *t, Obj *use_obj)
{
    map_window = Game::get_game()->get_map_window();
    party = Game::get_game()->get_party();
    target = NULL;
    moves_left = party->get_party_size() * 2; // step timeout
    wait_for_effect = 0;
    actor_to_hide = NULL;
    falling_in = false;

    dest = new MapCoord(*d);
    if(t)
        target = new MapCoord(*t);
    moongate = use_obj;

    queue(); // start
}

/* Party movement to/from dungeon or to moongate. Repeated until everyone has
 * entered, then the entire party is moved to the destination, and this waits
 * until the visual effects complete.
 */
void TimedPartyMove::timed(uint32 evtime)
{
    if(wait_for_effect != 0) // ignores "falling_in"
    {
        repeat(); // repeat once more (callback() must call stop())
        return;
    }
    stop(); // cancelled further down with repeat(), if still moving

    if(moves_left)
    {
        if(((falling_in == false) && move_party())
           || ((falling_in == true) && fall_in()))
            repeat(); // still moving
    }
    else // timed out, make sure nobody is walking
        for(uint32 m = 0; m < party->get_party_size(); m++)
            party->get_actor(m)->stop_walking();

    // NOTE: set by repeat() or stop()
    if(repeat_count == 0) // everyone is in position
    {
        if(falling_in == false) // change location, get in formation
        {
            change_location(); // fade map, move and show party
            party->stop_walking(); // return control (and change viewpoint)

            // wait for effect or line up now; Party called unpause_user()
            Game::get_game()->pause_user();
            if(wait_for_effect == 0)
            {
                delay = 50; set_time(); // fall-in as fast as possible (but visibly)
                moves_left = party->get_party_size() - 1; // followers
                falling_in = true;
            }
            repeat(); // don't stop yet!
        }
        else // already changed location
        {
            Game::get_game()->unpause_user();
            stop_timer(); // done
        }
    }
    if(moves_left > 0)
        --moves_left;
}

/* Assume the teleport-effect is complete. (don't bother checking msg)
 */
uint16 TimedPartyMove::callback(uint16 msg, CallBack *caller, void *data)
{
    if(wait_for_effect == 1) // map-change
    {
        wait_for_effect = 0;
        Game::get_game()->unpause_anims();

        delay = 50; set_time(); // fall-in as fast as possible (but visibly)
        moves_left = party->get_party_size() - 1; // followers
        falling_in = true;
    }
    else if(wait_for_effect == 2) // vanish
    {
        wait_for_effect = 0;
        Game::get_game()->unpause_anims();
//        move_party();
    }
    return(0);
}

/* Returns true if people are still walking.
 */
bool TimedPartyMove::move_party()
{
    bool moving = false; // moving or waiting
    Actor *used_gate = NULL; // someone just stepped into the gate (for effect)

    if(actor_to_hide)
    {
        hide_actor(actor_to_hide);
        moving = true; // allow at least one more tick so we see last actor hide
    }
    actor_to_hide = NULL;

    for(uint32 a = 0; a < party->get_party_size(); a++)
    {
        Actor *person = party->get_actor(a);

        if(person->is_visible())
        {
            MapCoord loc(person->get_location());
            bool really_visible = map_window->in_window(loc.x, loc.y, loc.z);

            if(a == 0) // FIXME: should be done automatically, but world is frozen
                map_window->centerMapOnActor(person);

            if(loc != *dest && really_visible)
            {
                // nobody has just used the gate (who may still be vanishing)
                if(!used_gate || loc.distance(*dest) > 1) // or we aren't close to gate yet
                {
                    person->swalk(*dest); // start (or continue) going to gate
                    person->update(); // ActorManager is paused
                    loc = person->get_location(); // don't use the old location
                }
                else
                    person->stop_walking(); // wait for whoever is at gate
            }
            if(loc == *dest // actor may have just arrived this turn
               || !really_visible)
            {
                person->stop_walking();
                if(moongate) used_gate = person; // hide after this turn
                else if(!actor_to_hide) actor_to_hide = person; // hide before next turn
            }
            moving = true; // even if at gate, might not be hidden yet
        }
    }

    if(used_gate) // wait until now (instead of in loop) so others can catch up before effect
        hide_actor(used_gate);
    return(moving);
}

/* Start a visual effect and hide the party member.
 */
void TimedPartyMove::hide_actor(Actor *person)
{
    EffectManager *effect_mgr = Game::get_game()->get_effect_manager();
    if(wait_for_effect != 2)
    {
        if(moongate) // vanish
        {
            effect_mgr->watch_effect(this, new VanishEffect()); // wait for callback
            wait_for_effect = 2;
            delay = 1; set_time(); // effect will be longer than original delay
        }
        person->hide();
    }
}

/* Start a visual effect and move the party to the target.
 */
void TimedPartyMove::change_location()
{
    EffectManager *effect_mgr = Game::get_game()->get_effect_manager();
    SDL_Surface *mapwindow_capture = NULL;
    if(wait_for_effect != 1)
    {
        if(moongate)
        {
            // get image before deleting moongate
            mapwindow_capture = map_window->get_sdl_surface();
            // must delete moongate here because dest may be the same as target...
            // remove moongate before moving so the tempobj cleanup doesn't bite us
            Game::get_game()->get_obj_manager()->remove_obj(moongate);
            delete_obj(moongate);
        }

        party->move(target->x, target->y, target->z);
        party->show(); // unhide everyone

        if(mapwindow_capture) // could check this or moongate again
        {
            // start fade-to
            effect_mgr->watch_effect(this, /* call me */
                                     new FadeEffect(FADE_PIXELATED, FADE_OUT, mapwindow_capture));
            SDL_FreeSurface(mapwindow_capture);

            Game::get_game()->pause_anims();
            wait_for_effect = 1;
        }
    }
}


/* Pass a few times so everyone in the party can get into formation.
 * Returns true if party needs to move more to get into formation. */
bool TimedPartyMove::fall_in()
{
    bool not_in_position = false; // assume false until someone checks true
    party->follow();
    for(uint8 p = 1; p < party->get_party_size(); p++)
    {
        Actor *follower = party->get_actor(p);
        MapCoord at = follower->get_location(),
                 desired = party->get_formation_coords(p);
        follower->update();
        if(at != desired)
            not_in_position = true;
    }
    return(not_in_position);
}


/*** TimedPartyMoveToVehicle ***/


/* Party movement to vehicle. Second target is unused.
 */
TimedPartyMoveToVehicle::TimedPartyMoveToVehicle(MapCoord *d, Obj *obj,
                                                 uint32 step_delay)
: TimedPartyMove(d, NULL, step_delay)
{
    ship_obj = obj;
}


/* Repeat until everyone is in the boat, then start it up.
 */
void TimedPartyMoveToVehicle::timed(uint32 evtime)
{
    stop(); // cancelled further down with repeat(), if still moving
    for(uint32 a = 0; a < party->get_party_size(); a++)
    {
        Actor *person = party->get_actor(a);
        MapCoord loc(person->get_location());
        // not at boat location
        if(loc != *dest)
        {
            // offscreen (or timed out), teleport to target
            MapWindow *map_window = Game::get_game()->get_map_window();
            if(!map_window->in_window(loc.x, loc.y, loc.z) || moves_left == 0)
                person->move(dest->x, dest->y, dest->z, ACTOR_FORCE_MOVE);
            else // keep walking
                person->swalk(*dest);
            person->update();
            repeat(); // repeat once more
        }
        else // at destination
        {
            person->stop_walking();
            person->hide(); // set in-vehicle
        }
    }

    if(repeat_count == 0) // everyone is in the boat
    {
        Game::get_game()->get_usecode()->use_obj(ship_obj);
        party->stop_walking(); // return control to player
    }
    if(moves_left > 0)
        --moves_left;
}


/* Dump one item at a time out of a container, and print it's name to MsgScroll.
 */
TimedContainerSearch::TimedContainerSearch(Obj *obj) : TimedEvent(500, TIMER_DELAYED)
{
    Game *game = Game::get_game();
    scroll = game->get_scroll();
    uc = game->get_usecode();
    om = game->get_obj_manager();

    container_obj = obj;
    prev_obj = NULL;

    game->set_pause_flags((GamePauseState)(game->get_pause_flags() | PAUSE_USER));
    queue(); // start
}


void TimedContainerSearch::timed(uint32 evtime)
{
    prev_obj = uc->get_obj_from_container(container_obj);
    if(prev_obj)
    {
        scroll->display_string(om->look_obj(prev_obj, true));
        if(container_obj->container->end()) // more objects left
            scroll->display_string(container_obj->container->end()->prev
                                   ? ", " : ", and ");
        repeat();
    }
    else
    {
        Game::get_game()->unpause_all();
        stop();
    }
}



/*** TimedCallback ***/
TimedCallback::TimedCallback(CallBack *t, void *d, uint32 wait_time, bool repeat)
                            : TimedEvent(wait_time, TIMER_DELAYED, TIMER_REALTIME)
{
    set_target(t);
    set_user_data(d);
    repeat_count = repeat ? -1 : 0;

    queue(); // start
}


void TimedCallback::timed(uint32 evtime)
{
    if(callback_target)
        message(MESG_TIMED, &evtime);
    else
        stop();
}


GameTimedCallback::GameTimedCallback(CallBack *t, void *d, uint32 wait_time, bool repeating)
                                    : TimedCallback(t, d, wait_time, repeating)
{
    // re-queue timer using game ticks
    dequeue();
    real_time = TIMER_GAMETIME;
    set_time();// change to game time
    queue(); // start
}



/*** TimedAdvance: Advance game time by rate until hours has passed. **/
#define TIMEADVANCE_PER_SECOND 1000 /* frequency of timer calls */
TimedAdvance::TimedAdvance(uint8 hours, uint16 r)
                          : TimedCallback(NULL, NULL, 1000/TIMEADVANCE_PER_SECOND, true),
                            clock(Game::get_game()->get_clock()), minutes(0),
                            minutes_this_hour(0)
{
    init(hours * 60, r);
}


/* Advance to time indicated by timestring, of the format "HH:MM".
 */
TimedAdvance::TimedAdvance(std::string timestring, uint16 r)
                          : TimedCallback(NULL, NULL, 1000/TIMEADVANCE_PER_SECOND, true),
                            clock(Game::get_game()->get_clock()), minutes(0),
                            minutes_this_hour(0)
{
    uint8 hour = 0, minute = 0;

    get_time_from_string(hour, minute, timestring); // set stop time

    // set number of hours and minutes to advance
    uint16 advance_h = (clock->get_hour() == hour) ? 24
                       : (clock->get_hour() < hour) ? (hour-clock->get_hour())
                       : (24-(clock->get_hour()-hour));
    uint16 advance_m;
    if(clock->get_minute()<=minute)
        advance_m = minute - clock->get_minute();
    else
    {
        advance_m = (60 - (clock->get_minute()-minute));
        if(advance_h > 0)
            advance_h -= 1;
        else
            advance_h = 23;
    }
    // go
    init((advance_h * 60) + advance_m, r);
}


TimedAdvance::~TimedAdvance()
{

}


/* Set time advance.
 */
void TimedAdvance::init(uint16 min, uint16 r)
{
    advance = min;
    rate = r;
    prev_evtime = clock->get_ticks();
printf("TimedAdvance(): %02d:%02d + %02d:%02d (rate=%d)\n",
       clock->get_hour(), clock->get_minute(), advance/60, advance%60, rate);
}


/* Advance game time by rate each second. Timer is stopped after after the time
 * has been advanced as requested.
 */
void TimedAdvance::timed(uint32 evtime)
{
    uint32 milliseconds = (evtime - prev_evtime) > 0 ? (evtime - prev_evtime) : 1;
    uint32 fraction = 1000 / milliseconds; // % of second
    uint32 minutes_per_fraction = rate / (fraction > 0 ? fraction : 1);
    bool hour_passed = false; // another hour has passed
    prev_evtime = evtime;

    for(uint32 m = 0; m < minutes_per_fraction; m++)
    {
        clock->inc_minute();
        minutes += 1;
        if(++minutes_this_hour > 59)
        {
            minutes_this_hour = 0;
            hour_passed = true;
        }
        if(time_passed())
            break;
    }
    Game::get_game()->get_map_window()->updateBlacking();

    if(hour_passed && callback_target) // another hour has passed
        message(MESG_TIMED, &evtime);

    if(time_passed())
    {
printf("~TimedAdvance(): now %02d:%02d\n", clock->get_hour(), clock->get_minute());
        if(callback_target && !hour_passed) // make sure to call target
            message(MESG_TIMED, &evtime);
        stop(); // done
    }
}


/* Returns true when the requested amount of time has passed.
 */
bool TimedAdvance::time_passed()
{
    return(minutes >= advance);
}


/* Set hour and minute from "HH:MM" string.
 */
void TimedAdvance::get_time_from_string(uint8 &hour, uint8 &minute, std::string timestring)
{
    char *hour_s = NULL, *minute_s = NULL;
    hour_s = strdup(timestring.c_str());
    for(uint32 c = 0; c < strlen(hour_s); c++)
        if(hour_s[c] == ':') // get minutes
        {
            minute_s = strdup(&hour_s[c + 1]);
            hour_s[c] = '\0';
            break;
        }

    if(hour_s)
    {
        hour = strtol(hour_s, NULL, 10);
        free(hour_s);
    }
    if(minute_s)
    {
        minute = strtol(minute_s, NULL, 10);
        free(minute_s);
    }
}

