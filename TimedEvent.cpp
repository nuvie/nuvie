
#include "U6def.h"

#include "Actor.h"
#include "Game.h"
#include "MapWindow.h"
#include "Party.h"
#include "TimedEvent.h"


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


/* Call timed event at front of queue, whose time is <= `evtime'.
 * Returns true if an event handler was called. (false if time isn't up yet)
 */
bool TimeQueue::call_timer(uint32 evtime)
{
    if(empty())
        return(false);
    TimedEvent *first = tq.front();
    if(first->time > evtime)
        return(false);
    first->timed(evtime);
    if(first->repeat) // repeat! same delay, add time
    {
        first->time = evtime + first->delay;
        add_timer(first);
    }
    return(true);
}


/* This constructor must be called by all subclasses to add to queue.
 */
TimedEvent::TimedEvent(uint32 reltime, bool immediate) : delay(reltime),
            time(reltime + SDL_GetTicks()), ignore_pause(false), repeat(false)
{
    if(immediate)
        time = 0; // start now (useful if repeat == true)
    event = Game::get_game()->get_event();
    event->get_time_queue()->add_timer(this);
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
        // if not at target area (FIXME: only checks Z for now)
        if(loc.z != target->z) //FIX for moongate teleport on the same plane.
        {
            // at destination or offscreen, teleport to target
            MapWindow *map_window = Game::get_game()->get_map_window();
            if(loc == *dest || !map_window->in_window(loc.x, loc.y, loc.z))
                person->move(target->x, target->y, target->z, ACTOR_FORCE_MOVE);
            else // keep walking to destination
                person->swalk(*dest);
            person->update();
            repeat = true;
        }
        // at target area (FIXME: only checks Z for now)
        if(loc.z == target->z)
            person->stop_walking(); // return control
    }
    if(!repeat) // everyone is at target area
        party->stop_walking();
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
            // offscreen, teleport to target
            MapWindow *map_window = Game::get_game()->get_map_window();
            if(!map_window->in_window(loc.x, loc.y, loc.z))
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
}
