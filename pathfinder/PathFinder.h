#ifndef	__PathFinder_h__
#define	__PathFinder_h__

#include "U6def.h"
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
    uint32 max_cost; // absolute maximum cost for any path

public:
    PathFinder(Actor *a, MapCoord &d, uint32 speed = 1);
    PathFinder();
    virtual ~PathFinder();

    // fast direction finder for following? (Actor::swalk())
    // or slower path-finder for travel? (Actor::lwalk())
    virtual bool can_follow() { return(false); }
    virtual bool can_travel() { return(false); }

    virtual void walk_path(uint32 speed = 1) = 0;
    virtual void set_dest(MapCoord &d);
    virtual void set_dest2(MapCoord &d2) { dest2 = NULL; }
    void set_speed(uint8 sp) { }

    bool reached_goal();
    void get_adjacent_dir(sint8 &xdir, sint8 &ydir, sint8 rotate);
    void delete_path();
    uint32 get_max_cost(uint32 cost);

    virtual MapCoord get_next_step() { return(dest); }

};

#endif /* __PathFinder_h__ */
