#ifndef __TimedEvent_h__
#define __TimedEvent_h__

#include <list>
#include <string>
#include <cstdio>
#include "CallBack.h"

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
    void remove_timer(TimedEvent *tevent);
    TimedEvent *pop_timer();
    bool delete_timer(TimedEvent *tevent);

    bool call_timer(uint32 now); // activate
    void call_timers(uint32 now); // activate all
};


#define TIMER_IMMEDIATE true
#define TIMER_DELAYED false
#define TIMER_REALTIME true
#define TIMER_GAMETIME false

/* Event activated by a timer. Add to one of the Event time-queues to start.
 * (THERE IS ONLY ONE SET)  The timed() method is called on activation,
 * and the timer may be automatically deleted or repeated.
 */
class TimedEvent
{
friend class TimeQueue;
friend class Event;
protected:
    TimeQueue *tq; // the TimeQueue; so we can add ourself
    uint32 delay, time; // timer delay, and next absolute time to activate
    bool ignore_pause; // activates even if game is paused
    bool real_time; // time and delay is in milliseconds (false=game ticks/turns)
    bool tq_can_delete; // can TimeQueue delete this TimedEvent when done?
    sint8 repeat_count; // repeat how many times? (-1=infinite;0=stop)

public:
    TimedEvent(uint32 reltime, bool immediate = TIMER_DELAYED, bool realtime = TIMER_REALTIME);
    virtual ~TimedEvent() { }
    virtual void timed(uint32 evtime) { fprintf(stderr, "TimedEvent: undefined timer method\n"); }

    void queue(); // set tq, add to tq
    void dequeue(); // remove from tq, clear tq
    // stop repeating, remove from tq if it won't delete it
    void stop() { repeat_count = 0; if(!tq_can_delete) dequeue(); }
    // repeat once (or for requested count)
    void repeat(uint32 count = 1) { repeat_count = count; }

    void set_time(); // set `time' from `delay'
};


/* Print to stdout. (timer test)
 */
class TimedMessage : public TimedEvent
{
    std::string msg;
public:
    TimedMessage(uint32 reltime, const char *m, bool repeat = false)
    : TimedEvent(reltime), msg(m) { repeat_count = repeat ? -1 : 0; }
    void timed(uint32 evtime)
    {
        printf("Activate! evtime=%d msg=\"%s\"\n", evtime, msg.c_str());
    }
};


/* Move the party to/from a dungeon or ladder or moongate. Characters off-screen
 * will teleport.
 */
class TimedPartyMove : public TimedEvent
{
protected:
    Party *party; // the party
    MapCoord *dest; // destination, where all actors walk to and disappear
    MapCoord *target; // where they reappear at the new plane
    uint32 moves_left; // walk timeout
    Obj *moongate; // if using a moongate
public:
    TimedPartyMove(MapCoord *d, MapCoord *t, uint32 step_delay = 500);
    TimedPartyMove(MapCoord *d, MapCoord *t, Obj *use_obj, uint32 step_delay = 500);
    ~TimedPartyMove();
    void init(MapCoord *d, MapCoord *t, Obj *use_obj);
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
    TimedRTC() : TimedEvent(1000) { repeat_count = -1; }
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


/* Send timer message to callback target after `wait_time' is up, passing it
 * some target-defined data.
 *  new TimedCallback(PowderKeg, (void *)my_powderkeg_data, time_to_explode);
 */
class TimedCallback : public TimedEvent, public CallBack
{
public:
    TimedCallback(CallBack *t, void *d, uint32 wait_time,
                  bool repeat = false);
    ~TimedCallback() {  }
    void timed(uint32 evtime);
    void clear_target() { set_target(NULL); }
};


class GameTimedCallback : public TimedCallback
{
public:
    GameTimedCallback(CallBack *t, void *d, uint32 wait_time, bool repeat = false);
    ~GameTimedCallback() {  }
};

#endif /* __TimedEvent_h__ */
