#ifndef __CombatPathFinder_h__
#define __CombatPathFinder_h__

#include "ActorPathFinder.h"

class CombatPathFinder: public ActorPathFinder
{
protected:
    Actor *target;
    enum
    {
        PATHFINDER_NONE,
        PATHFINDER_CHASE,
        PATHFINDER_FLEE
    } target_mode;

    bool update_location();

    uint8 max_dist;

public:
    CombatPathFinder(Actor *a);
    CombatPathFinder(Actor *a, Actor *t);
    ~CombatPathFinder();
    bool set_flee_mode(Actor *actor);
    bool set_chase_mode(Actor *actor);
    void set_distance(uint8 dist) { max_dist = dist; }

    bool get_next_move(MapCoord &step);
};

#endif /* __CombatPathFinder_h__ */
