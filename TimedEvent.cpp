
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


/* Activate all events for the current time, deleting those that have fired
 * and are of no more use.
 */
void TimeQueue::call_timers(uint32 now)
{
    while(!empty() && call_timer(now))
    {
        TimedEvent *tevent = pop_timer(); // remove
        if(!tevent->repeat)
            delete tevent; // if not repeated, safe to delete
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
    // add after events with earlier time
    t = tq.begin();
    while(t != tq.end() && (*t++)->time <= tevent->time);
    tq.insert(t, tevent);
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
 * Returns true if an event handler was called. (false if time isn't up yet)
 */
bool TimeQueue::call_timer(uint32 now)
{
    if(empty())
        return(false);
    TimedEvent *first = tq.front();
    if(first->time > now)
        return(false);

    first->timed(now);
    if(first->repeat) // repeat! same delay, add time
    {
        first->time = now + first->delay;
        add_timer(first);
    }
    return(true);
}


/* This constructor must be called by all subclasses to add to queue.
 */
TimedEvent::TimedEvent(uint32 reltime, bool immediate, bool realtime)
            : delay(reltime), ignore_pause(false), repeat(false), real_time(realtime)
{
    GameClock *clock = Game::get_game()->get_clock();
    event = Game::get_game()->get_event();

    time = reltime + (real_time ? clock->get_ticks() : clock->get_turn());
    if(immediate)
        time = 0; // start now (useful if repeat == true)
    if(real_time)
        event->get_time_queue()->add_timer(this);
    else
        event->get_game_time_queue()->add_timer(this);
//    if(immediate)
//        this->timed(now);
}



/* Party movement to/from dungeon, with a certain number of
 * milliseconds between each step. Construct & Set destination.
 */
TimedPartyMove::TimedPartyMove(MapCoord *d, MapCoord *t, uint32 step_delay)
                              : TimedEvent(step_delay, true)
{
    party = Game::get_game()->get_party();
    dest = new MapCoord(*d);
    if(t)
        target = new MapCoord(*t);
    else
        target = NULL;
    moves_left = party->get_party_size() * 2;
}

TimedPartyMove::~TimedPartyMove()
{
 delete dest;
 delete target;
}


/* Party movement to/from dungeon or to vehicle. Repeated until everyone is at
 * the target area.
 */
void TimedPartyMove::timed(uint32 evtime)
{
    Player *player = Game::get_game()->get_player();
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    repeat = false;
    for(uint32 a = 0; a < party->get_party_size(); a++)
    {
        Actor *person = party->get_actor(a);
        MapCoord loc(person->get_location());
        // if not at target area
	if(!person->is_nearby(*target))
        {
            // at destination or offscreen (or timed out), teleport to target
            MapWindow *map_window = Game::get_game()->get_map_window();
            if(loc == *dest || !map_window->in_window(loc.x, loc.y, loc.z)
               || moves_left == 0)
                person->move(target->x, target->y, target->z, ACTOR_FORCE_MOVE);
            else // keep walking to destination
                person->swalk(*dest);
            person->update();
            repeat = true;
        }
        // at target area
        if(person->is_nearby(*target))
            person->stop_walking(); // return control
    }
    if(!repeat) // everyone is at target area
        party->stop_walking();
    if(moves_left > 0)
        --moves_left;
}


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
    Player *player = Game::get_game()->get_player();
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    repeat = false;
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
            repeat = true;
        }
        else // at destination
        {
            person->stop_walking();
            person->hide(); // set in-vehicle
        }
    }

    if(!repeat) // everyone is in the boat
    {
        Game::get_game()->get_usecode()->use_obj(ship_obj);
        party->stop_walking(); // return control to player
    }
    if(moves_left > 0)
        --moves_left;
}


/* Dump one item at a time out of a container, and print it's name to MsgScroll.
 */
TimedContainerSearch::TimedContainerSearch(Obj *obj) : TimedEvent(500, false)
{
    Game *game = Game::get_game();
    scroll = game->get_scroll();
    uc = game->get_usecode();
    om = game->get_obj_manager();

    container_obj = obj;
    prev_obj = NULL;
    Game::get_game()->get_player()->uncontrol();
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
        repeat = true;
    }
    else
    {
        Game::get_game()->get_player()->control();
        repeat = false;
    }
}


TimedCallback::TimedCallback(TimedCallbackTarget *t, void *d, uint32 wait_time, bool repeating)
                            : TimedEvent(wait_time, false, false)
{
    target = t;
    data = (char *)d;
    repeat = repeating;
}


void TimedCallback::timed(uint32 evtime)
{
    target->timed_callback((void *)data);
}


GameTimedCallback::GameTimedCallback(TimedCallbackTarget *t, void *d, uint32 wait_time, bool repeating)
                                     : TimedCallback(t, d, wait_time, repeating)
{
}
