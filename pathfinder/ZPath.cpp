#include "Actor.h"
#include "ZPath.h"


ZPath::ZPath(Actor *a, MapCoord &d, uint32 speed)
{
    path = NULL;
    step_count = 0;
    next_step = 0;
    dest2 = NULL;

    actor = a;
    set_dest(d);
}


/* Destination 2 is used when destination is blocked.
 */
void ZPath::set_dest2(MapCoord &d2)
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


/* Move the actor closer to the destination. If blocked, try to move around the
 * obstacle, preferring movement towards destination2. Speed is the number of
 * steps to take.
 */
void ZPath::walk_path(uint32 speed)
{
    uint16 x = actor->x, y = actor->y, z = actor->z;
    uint16 dx = dest.x, dy = dest.y;

    sint8 xdir = (x < dx) ? 1 : (x == dx) ? 0 : -1; // relative directions
    sint8 ydir = (y < dy) ? 1 : (y == dy) ? 0 : -1;

    if(speed == 0)
        speed = 1; // don't want infinite moves
    do
    {
// check original move in both directions
// if blocked, try all other directions, preferring moves towards target2
// (ex: if following player at right, try 1,2,3 then -1,-2,-3,-4;
//      if following player at left, try -1,-2,-3 then 1,2,3,4)
        sint32 dir = 1, dc = 0;
        if(dest2
           && ((ydir < 0 && dest2->x < x) || (xdir < 0 && dest2->y > y)
               ||(ydir > 0 && dest2->x > x) || (xdir > 0 && dest2->y < y)))
            dir = -1;
        while(!actor->check_move(x + xdir, y + ydir, z) && (dc < 8))
        {
            if(dc == 4) // checked all in one direction, now check other direction
                dir = (dir > 0) ? -4 : 4; // 4: rrrr, or -4: llll
            else
                dir = (dir > 0) ? 1 : -1; // 1: r, or -1: l
            get_adjacent_dir(xdir, ydir, dir);
            ++dc;
        }

        actor->set_direction((xdir == 1) ? ACTOR_DIR_R : ACTOR_DIR_L);
        actor->set_direction((ydir == 1) ? ACTOR_DIR_D : ACTOR_DIR_U);
        actor->moveRelative(xdir, ydir);
    }
    while(--speed);
}
