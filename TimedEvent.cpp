
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
            tevent->time = now + tevent->delay;
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
    while(t != tq.end() && (*t++)->time <= tevent->time);
    tq.insert(t, tevent);
}


/* Remove timed event from queue. 
 */
void TimeQueue::remove_timer(TimedEvent *tevent)
{
    std::list<TimedEvent *>::iterator t;
    t = tq.begin();
    while(t != tq.end())
        if(*t == tevent)
        {
            tq.erase(t);
            t = tq.begin();
            continue; // in case there are repeats (?), remove all of them
        }
        else ++t;
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
    if(first->time > now)
        return(false);

    first->timed(now); // fire
    return(true);
}


/* Delete a timer, if its can_delete flag is true. Remove from the queue first!
 */
bool TimeQueue::delete_timer(TimedEvent *tevent)
{
    if(tevent->tq_can_delete)
        delete tevent;
    return(tevent->tq_can_delete);
}


/* Accepts delay, immediate toggle(false), and realtime switch(true). It must
 * be queued afterwards to start.
 */
TimedEvent::TimedEvent(uint32 reltime, bool immediate, bool realtime)
            : delay(reltime), ignore_pause(false), repeat_count(0),
              real_time(realtime), tq_can_delete(true)
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

/* Party movement to/from dungeon, with a certain number of
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
    party = Game::get_game()->get_party();
    target = NULL;
    moves_left = party->get_party_size() * 2; // step timeout

    dest = new MapCoord(*d);
    if(t)
        target = new MapCoord(*t);
    moongate = use_obj;

    queue(); // start
}


/* Party movement to/from dungeon or to moongate. Repeated until everyone has
 * entered, then the entire party is moved to the destination.
 */
void TimedPartyMove::timed(uint32 evtime)
{
    stop(); // cancelled further down with repeat(), if still moving
    for(uint32 a = 0; a < party->get_party_size(); a++)
    {
        Actor *person = party->get_actor(a);
        MapCoord loc(person->get_location());
        // if not disappeared
	if(person->is_visible())
        {
            // at destination or offscreen (or timed out), hide
            MapWindow *map_window = Game::get_game()->get_map_window();
            if(loc == *dest || !map_window->in_window(loc.x, loc.y, loc.z)
               || moves_left == 0)
                person->hide();
            else // keep walking to destination
                person->swalk(*dest);
            person->update(); // update here because ActorManager is paused
            repeat(); // repeat once more
        }
        // entered dungeon/moongate (hidden)
        if(!person->is_visible())
            person->stop_walking(); // stop and wait for everyone else
    }
    if(repeat_count == 0) // everyone is ready to go to the target area
    {
        // must delete moongate here because dest may be the same as target...
        // remove moongate before moving so the tempobj cleanup doesn't bite us
        if(moongate)
        {
            Game::get_game()->get_obj_manager()->remove_obj(moongate);
            Game::get_game()->get_obj_manager()->delete_obj(moongate);
        }
        party->move(target->x, target->y, target->z);
        party->show();
        party->stop_walking(); // return control
    }

    if(moves_left > 0)
        --moves_left;
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
printf("new GameTimedCallback(callback=%x, user_data=%x, delay=%d, repeat=%d)\n", callback_target, callback_user_data, delay, repeat_count);
}
