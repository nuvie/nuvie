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

    max_node_score = 8*2*3;
wait_for = 0; // number of turns to wait until trying again
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
    // teleport if both locations are off-screen
    if(!dest.is_visible() && !MapCoord(actor->x,actor->y,actor->z).is_visible())
    {
        actor->move(dest.x, dest.y, dest.z);
        return;
    }

    if(wait_for)
    {
        --wait_for;
        return;
    }

    if(!path)
    {
        // don't loop back to original start if at end of partial path
        src.x = actor->x; src.y = actor->y; src.z = actor->z;
        if(!node_search(src, dest))
        {
            wait_for = 10; // wait to try again
            return;
        }
    }
    // have a path, take a step
    uint16 nx = path[next_step].x, ny = path[next_step].y;
    uint8 nz = path[next_step].z;
//    actor->set_direction(MapCoord(actor->x, actor->y, actor->z), MapCoord(nx, ny, nz));
    if(actor->x < nx)
        actor->set_direction(ACTOR_DIR_R);
    else if(actor->x > nx)
        actor->set_direction(ACTOR_DIR_L);
    else if(actor->y < ny)
        actor->set_direction(ACTOR_DIR_D);
    else
        actor->set_direction(ACTOR_DIR_U);
    Obj *door = NULL;
    bool moved = actor->move(nx, ny, nz,ACTOR_FORCE_MOVE);
    if(!moved) // check for unlocked door, and open it
    {
        ObjManager *obj_manager = Game::get_obj_manager();
        UseCode *uc = Game::get_usecode();
        Obj *door = obj_manager->get_obj(nx, ny, nz);
        if(door && uc->is_unlocked_door(door) && uc->use_obj(door))
            moved = actor->move(nx, ny, path[next_step].z); // try again
        else
            door = NULL;
    }
    if(moved)
    {
        // FIXME: close doors that are moved away from
        if(next_step > 0)
            next_step--;
        else
        {
            free(path); // out of steps
            path = NULL;
        }
    }
}

