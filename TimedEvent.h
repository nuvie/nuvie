#ifndef __TimedEvent_h__
#define __TimedEvent_h__

#include <list>
#include <string>
#include <cstdio>

class Event;
class MapCoord;
class MsgScroll;
class Party;
class TimedCallbackTarget;
class TimedEvent;

/* A queue for our events.
 */
class TimeQueue
{
    std::list<TimedEvent *> tq;
public:
    TimeQueue() : tq() { }
    ~TimeQueue() { clear(); }

    bool empty() { return(tq.empty()); }
    void clear() { while(!empty()) pop_timer(); }

    void add_timer(TimedEvent *tevent);
    TimedEvent *pop_timer();
    bool call_timer(uint32 now); // activate
    void call_timers(uint32 now); // activate all
};


/* Event activated by a timer.
 */
class TimedEvent
{
friend class TimeQueue;
friend class Event;
protected:
    Event *event;
    uint32 delay, time; // timer delay, next absolute time to activate
    bool ignore_pause; // activates even if game is paused
    bool repeat; // put back into queue with same delay after activation
    bool real_time; // time and delay is in milliseconds (false=game ticks/turns)
public:
//    TimedEvent() { fprintf(stderr, "Event: timer must be given activation time!\n"); abort(); }
    TimedEvent(uint32 reltime, bool immediate = false, bool realtime = true);
    virtual ~TimedEvent() { } // event queue will destroy after use
    virtual void timed(uint32 evtime) = 0;
};


/* Print to stdout. (timer test)
 */
class TimedMessage : public TimedEvent
{
    std::string msg;
public:
    TimedMessage(uint32 reltime, const char *m, bool rep = false)
    : TimedEvent(reltime), msg(m) { repeat = rep; }
    void timed(uint32 evtime)
    {
        printf("Activate! evtime=%d msg=\"%s\"\n", evtime, msg.c_str());
    }
};


/* Move the party to/from a dungeon or ladder. Characters off-screen will
 * teleport.
 */
class TimedPartyMove : public TimedEvent
{
protected:
    Party *party; // the party
    MapCoord *dest; // destination, where all actors walk to and disappear
    MapCoord *target; // where they reappear at the new plane
    uint32 moves_left; // walk timeout
public:
    TimedPartyMove(MapCoord *d, MapCoord *t, uint32 step_delay = 500);
    ~TimedPartyMove();
    
    void timed(uint32 evtime);
};


/* Move the party into a vehicle and start it when everyone is there.
 */
class TimedPartyMoveToVehicle : public TimedPartyMove
{
    Obj *ship_obj; // vehicle center
public:
    TimedPartyMoveToVehicle(MapCoord *d, Obj *obj, uint32 step_delay = 125);
    void timed(uint32 evtime);
};


class TimedRTC : public TimedEvent
{
public:
    TimedRTC() : TimedEvent(1000) { repeat = true; }
    void timed(uint32 evtime)
    {
//        Game::get_game()->get_player()->pass();
    }
};


/* Dump one item at a time out of a container, and print it's name to MsgScroll.
 */
class TimedContainerSearch : public TimedEvent
{
    MsgScroll *scroll;
    UseCode *uc;
    ObjManager *om;
    Obj *container_obj;
    Obj *prev_obj; // removed from container
public:
    TimedContainerSearch(Obj *obj);
    void timed(uint32 evtime);
};


/* A callback timer target. Classes using timed callbacks must inherit this.
 */
class TimedCallbackTarget
{
public:
    TimedCallbackTarget()  { }
    virtual ~TimedCallbackTarget() { }
    virtual void timed_callback(void *data)
    {
        fprintf(stderr, "void timer target activated\n");
    }
};


/* Call TimedCallbackTarget::timer_func() after `wait_time' is up, passing it
 * some target-defined data.
 *  new TimedCallback(PowderKeg, (void *)my_powderkeg_data, time_to_explode);
 */
class TimedCallback : public TimedEvent
{
    TimedCallbackTarget *target;
    char *data;
public:
    TimedCallback(TimedCallbackTarget *t, void *d, uint32 wait_time,
                  bool repeating = false);
    ~TimedCallback() {  }
    void timed(uint32 evtime);
};


class GameTimedCallback : public TimedCallback
{
public:
    GameTimedCallback(TimedCallbackTarget *t, void *d, uint32 wait_time, bool repeating = false);
};

#endif /* __TimedEvent_h__ */
