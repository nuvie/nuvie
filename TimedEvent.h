#ifndef __TimedEvent_h__
#define __TimedEvent_h__

#include <list>

class Event;
class Party;
class MapCoord;
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
    bool call_timer(uint32 evtime); // activate
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
public:
//    TimedEvent() { fprintf(stderr, "Event: timer must be given activation time!\n"); abort(); }
    TimedEvent(uint32 reltime, bool immediate = false);
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


#endif /* __TimedEvent_h__ */
