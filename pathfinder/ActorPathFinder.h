#ifndef __ActorPathFinder_h__
#define __ActorPathFinder_h__

#include "PathFinder.h"
#include "DirFinder.h"

class Actor;

class ActorPathFinder: public PathFinder, public DirFinder
{
protected:
    Actor *actor;

public:
    ActorPathFinder(Actor *a, MapCoord g);
    virtual ~ActorPathFinder();
    void set_actor(Actor *a);

    virtual bool update_location(); /* get location from actor (use any time) */
    virtual void actor_moved(); /* the actor moved ON PATH...
                                   (use after get_next_move()) */

    bool check_loc(const MapCoord &loc);

    void get_closest_dir(MapCoord &rel_step); // relative dir loc->goal
    virtual bool get_next_move(MapCoord &step);

protected:
    bool seek_target(MapCoord &rel_step);
    bool search_towards_target(const MapCoord &g, MapCoord &rel_step);
    bool check_dir(const MapCoord &loc, MapCoord &rel, sint8 rot=0);
    bool check_dir_and_distance(MapCoord loc, MapCoord g, MapCoord &rel_step, sint8 rotate);
};

#endif /* __ActorPathFinder_h__ */
