#ifndef __ZPath_h__
#define __ZPath_h__

#include "PathFinder.h"

class Actor;


/*  XXX
 *   X Direction finder (not really a path-finder) that calculates each step
 *  X  as it walks. Use for party formation, or chasing.
 * XXX
 */
class ZPath : public PathFinder
{
    Map *map;
    ActorManager *actor_manager;

public:
    ZPath(Actor *a, MapCoord &d, uint32 speed = 0);
    ~ZPath() { }
    bool can_follow() { return(true); }
    bool using_path() { return(step_count != 0); }

    void push_dest(MapCoord &d3);
    MapCoord pop_dest();
    inline MapCoord get_next_step(); // return dest or next in path

    void walk_path(uint32 step_speed = 0);
    uint32 try_step(MapCoord &d, uint32 count = 1);
    bool move_actor(uint16 x, uint16 y);

    uint8 get_closest_dir(MapCoord &start, MapCoord &d1, MapCoord &d2, uint8 skip = 0);
    uint8 get_closest_dir(MapCoord &start, MapCoord &dest, uint8 skip = 0) { return(get_closest_dir(start, dest, dest, skip)); }

    bool avoid_obstacle(MapCoord &start, MapCoord &goal);
    void get_obstacle_tracer(MapCoord &start, sint32 xdir, sint32 ydir, sint32 &Axdir,
                             sint32 &Aydir, sint32 &Bxdir, sint32 &Bydir);
};


#endif /* __ZPath_h__ */
