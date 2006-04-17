#ifndef __SchedPathFinder_h__
#define __SchedPathFinder_h__

#include "ActorPathFinder.h"

/* Long-range pathfinder for NPCs.
 */
class SchedPathFinder: public ActorPathFinder
{
protected:
    uint32 prev_step_i, next_step_i; /* step counters */

public:
    /* Pass 'path_type' to define search rules and methods to be used. The
       PathFinder is responsible for deleting it when finished. */
    SchedPathFinder(Actor *a, MapCoord g, Path *path_type);
    ~SchedPathFinder();

    bool get_next_move(MapCoord &step); /* returns the next step in the path */
    bool find_path(); /* gets a NEW path from location->goal */
    void actor_moved(); /* update location and step counters */

protected:
    bool is_location_in_path();
    void incr_step();
};

#endif /* __PathFinder_h__ */
