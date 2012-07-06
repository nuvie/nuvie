#include "Actor.h"
#include "CombatPathFinder.h"


CombatPathFinder::CombatPathFinder(Actor *a)
                                    : ActorPathFinder(a, a->get_location())
{
    target_mode = PATHFINDER_NONE;
    max_dist = 0;
    target = NULL;
}

/* Without a mode set, CombatPathFinder is identical to ActorPathFinder. */
CombatPathFinder::CombatPathFinder(Actor *a, Actor *t)
                                    : ActorPathFinder(a, t->get_location())
{
    target_mode = PATHFINDER_CHASE;
    target = t;
    max_dist = 0;
}

CombatPathFinder::~CombatPathFinder()
{

}

bool CombatPathFinder::reached_goal()
{
    if(target_mode == PATHFINDER_CHASE)
        return(loc.distance(goal) <= 1);
    if(target_mode == PATHFINDER_FLEE)
        return(max_dist != 0 && loc.distance(goal) > max_dist);
    return true;
}

bool CombatPathFinder::set_flee_mode(Actor *actor)
{
    target_mode = PATHFINDER_FLEE;
    target = actor;
    update_location();
    return true;
}

bool CombatPathFinder::set_chase_mode(Actor *actor)
{
    target_mode = PATHFINDER_CHASE;
    target = actor;
    update_location();
    return true;
}

bool CombatPathFinder::set_mode(CombatPathFinderMode mode, Actor *actor)
{
    target_mode = mode;
    target = actor;
    return true;
}

bool CombatPathFinder::update_location()
{
    ActorPathFinder::update_location();
    set_goal(target->get_location());
    if(max_dist != 0 && loc.distance(goal) > max_dist)
        target_mode = PATHFINDER_NONE;
    return true;
}

bool CombatPathFinder::get_next_move(MapCoord &step)
{
    if(target_mode == PATHFINDER_CHASE)
        return ActorPathFinder::get_next_move(step);
    if(target_mode == PATHFINDER_FLEE)
    {
        get_closest_dir(step);
        step.sx = -step.sx; step.sy = -step.sy;
        if(check_dir(loc, step))
        {
            step = loc.abs_coords(step.sx, step.sy);
            return true;
        }
    }
    return false;
}
