#include <cassert>
#include "nuvieDefs.h"

#include "Actor.h"
#include "Map.h"
#include "Path.h"
#include "SchedPathFinder.h"

/* NOTE: Path_type must always be valid. */
SchedPathFinder::SchedPathFinder(Actor *a, MapCoord g, Path *path_type)
                               : ActorPathFinder(a, g), prev_step_i(0), next_step_i(0)
{
    new_search(path_type);
    assert(search && actor);
}

SchedPathFinder::~SchedPathFinder()
{

}

bool SchedPathFinder::get_next_move(MapCoord &step)
{
    // jump to goal if both locations are off-screen
    if(!goal.is_visible() && !loc.is_visible())
    {
        if(check_loc(goal))
        {
            search->delete_path();
            step = goal;
            return true;
        }
    }

    if(!search->have_path())
        if(!find_path())
            return false;
    step = search->get_step(next_step_i); // have a path, take a step
    return true;
}

bool SchedPathFinder::find_path()
{
    if(search->have_path())
        search->delete_path();
    if(!search->path_search(loc, goal))
    {
        PERR("warning: actor %d failed to find a path to %x,%x\n", actor->get_actor_num(), goal.x, goal.y);
        return false;
    }
    prev_step_i = next_step_i = 0;
    incr_step(); // the first step is the start location, so skip it
    return true;
}

/* Returns true if actor location is correct. */
bool SchedPathFinder::is_location_in_path()
{
    MapCoord prev_step = search->get_step(prev_step_i);
    return(loc == prev_step);
}

/* Update previous and next steps in path. */
void SchedPathFinder::incr_step()
{
    MapCoord prev_loc = search->get_step(prev_step_i);
    MapCoord next_loc = search->get_step(next_step_i);
    MapCoord last_loc = search->get_last_step();
    if(prev_loc != last_loc)
    {
        if(prev_loc != next_loc) // prev_step is going to stay behind next_step
            ++prev_step_i;
        if(next_loc != last_loc)
            ++next_step_i;
    }
}

void SchedPathFinder::actor_moved()
{
    update_location();
    if(search->have_path())
        incr_step();
}

/* Don't bother moving around other actors. They will probably move soon. */
bool SchedPathFinder::check_loc(const MapCoord &loc)
{
    return actor->check_move(loc.x, loc.y, loc.z, ACTOR_IGNORE_OTHERS);
}
