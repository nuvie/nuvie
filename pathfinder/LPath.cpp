
#include "nuvieDefs.h"

#include <cstdlib>
#include "Actor.h"
#include "Game.h"
#include "ObjManager.h"
#include "UseCode.h"
#include "LPath.h"


LPath::LPath(Actor *a, MapCoord &d, uint32 speed)
{
    path = NULL;
    step_count = 0;
    next_step = 0;
    dest2 = NULL;

    actor = a;
    set_dest(d);

    max_node_score = 8*2*10; // ten screenfulls
    wait(0); // number of turns to wait until trying again when blocked
}


/* Set new source and destination.
 */
void LPath::set_dest(MapCoord &d)
{
    PathFinder::set_dest(d);
    actor->get_location(&src.x, &src.y, &src.z);
}


/* Move the actor along the path. Find a new path if blocked (except by actors.)
 */
void LPath::walk_path(uint32 speed)
{
// FIXME: need to move all this to a few functions
    // teleport if both locations are off-screen
    if(!dest.is_visible() && !MapCoord(actor->x,actor->y,actor->z).is_visible())
    {
        actor->move(dest.x, dest.y, dest.z);
        return;
    }
    if(delay)
    {
        --delay;
        return;
    }
    if(!path)
    {
        // don't loop back to original start if at end of partial path
        src.x = actor->x; src.y = actor->y; src.z = actor->z;
        if(!node_search(src, dest))
        {
            wait(10); // try again soon
            return;
        }
    }
    // have a path, take a step
    uint16 nx = path[next_step].x, ny = path[next_step].y;
    uint8 nz = path[next_step].z;
    if(actor->check_move(nx, ny, nz))
        actor->face_location(nx, ny);
    bool moved = actor->move(nx, ny, nz);
    if(moved)
    {
        if(next_step > 0)
            next_step--;
        else
        {
            free(path); // out of steps
            path = NULL;
        }
    }
}

