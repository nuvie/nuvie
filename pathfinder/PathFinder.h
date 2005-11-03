#ifndef	__PathFinder_h__
#define	__PathFinder_h__

#include "Map.h"

class Actor;
class MapCoord;

/* Generic interface and support for short and long range path-finders.
 */
class PathFinder
{
protected:
    Actor *actor; // the actor can be moved directly
    MapCoord src, dest, *dest2; // source, destinations
    MapCoord *path; // list of tiles in the path
    uint32 step_count; // number of tiles in the path
    uint32 next_step;
    uint8 delay; // number of walk_path() calls to skip walking
    uint32 speed; // number of moves given per walk_path(), 0=default

public:
    PathFinder(Actor *a, MapCoord &d, uint32 spd = 1);
    PathFinder();
    virtual ~PathFinder();

    // fast direction finder for following? (Actor::swalk())
    // or slower path-finder for travel? (Actor::lwalk())
    virtual bool can_follow() { return(false); }
    virtual bool can_travel() { return(false); }

    virtual bool walk_path(MapCoord &returned_step, uint32 step_speed = 0) = 0;
    virtual void set_dest(MapCoord &d);
    virtual void set_dest2(MapCoord &d2);
    void set_speed(uint8 sp) { speed = sp; }
    void wait(uint8 turns) { delay = turns; }

    bool reached_goal();
    void get_adjacent_dir(sint8 &xdir, sint8 &ydir, sint8 rotate);
    void delete_path();

    virtual MapCoord get_next_step() { return(dest); }

};

#endif /* __PathFinder_h__ */
