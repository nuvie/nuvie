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
public:
    ZPath(Actor *a, MapCoord &d, uint32 speed = 1);
    ~ZPath() { }
    bool can_follow() { return(true); }

    void set_dest2(MapCoord &d2);
    void walk_path(uint32 speed = 1);
    MapCoord get_next_step() { return(dest); }
};


#endif /* __ZPath_h__ */
