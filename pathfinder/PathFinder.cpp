
#include "nuvieDefs.h"

#include "Actor.h"
#include "Map.h"
#include "PathFinder.h"


PathFinder::PathFinder(Actor *a, MapCoord &d, uint32 speed)
                       : src(0,0,0), dest(0,0,0)
{
    path = NULL;
    step_count = next_step = 0;
    dest2 = NULL;
}


PathFinder::PathFinder() : src(0, 0, 0), dest(0, 0, 0)
{
    path = NULL;
    step_count = next_step = 0;
    dest2 = NULL;
}


PathFinder::~PathFinder()
{
    if(path)
        free(path);
    if(dest2)
        delete(dest2);
}


/* Free and zero path/steps.
 */
void PathFinder::delete_path()
{
    if(path)
        free(path);
    path = NULL;
    step_count = next_step = 0;
}


/* Returns true if actor is at the destination.
 */
bool PathFinder::reached_goal()
{
    return(actor->x == dest.x && actor->y == dest.y && actor->z == dest.z);
}


/* From a normalized direction xdir,ydir as base, get the normalized direction
 * towards one of the seven nearby tiles to the left or right of base.
 * For rotate: -n = left n tiles, n = right n tiles
 */
 void PathFinder::get_adjacent_dir(sint8 &xdir, sint8 &ydir, sint8 rotate)
{
    struct
    {
        sint8 x, y;
    } neighbors[8] = { {-1,-1}, {+0,-1}, {+1,-1},
                       {+1,+0},/*ACTOR*/ {+1,+1},
                       {+0,+1}, {-1,+1}, {-1,+0} };

    for(uint32 start = 0; start < 8; start++)
        if(neighbors[start].x == xdir && neighbors[start].y == ydir)
        {
            sint32 dest = start + rotate;
            while(dest < 0 || dest > 7)
                dest += (dest < 0) ? 8 : -8;
            xdir = neighbors[dest].x; ydir = neighbors[dest].y;
            break;
        }
}


/* Set new destination. Any existing path is cleared.
 */
void PathFinder::set_dest(MapCoord &d)
{
    if(d != dest)
        delete_path();
    dest.x = d.x;
    dest.y = d.y;
    dest.z = d.z;
}


/* Secondary destination.
 */
void PathFinder::set_dest2(MapCoord &d2)
{
    if(!dest2)
        dest2 = new MapCoord(d2.x, d2.y, d2.z);
    else
    {
        dest2->x = d2.x;
        dest2->y = d2.y;
        dest2->z = d2.z;
    }
}
