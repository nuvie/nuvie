
#include "U6def.h"

#include "Actor.h"
#include "ZPath.h"


ZPath::ZPath(Actor *a, MapCoord &d, uint32 spd)
{
    path = NULL;
    step_count = 0;
    next_step = 0;
    dest2 = NULL;

    actor = a;
    set_dest(d);
    speed = spd ? spd : 1; // default: take 1 step at a time
    wait(0);
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
void ZPath::walk_path(uint32 step_speed)
{
    if(delay) // don't walk yet
    {
        --delay;
        return;
    }
    bool moved = false;
    uint16 x = actor->x, y = actor->y, z = actor->z;
    uint16 dx = dest.x, dy = dest.y;

    if(speed == 0)
        speed = 1; // don't want infinite moves
    do
    {
//printf("Actor %d step from %d,%d to %d,%d\n",actor->get_actor_num(),x,y,dx,dy);
        sint8 xdir = (x < dx) ? 1 : (x == dx) ? 0 : -1; // relative directions
        sint8 ydir = (y < dy) ? 1 : (y == dy) ? 0 : -1;
        sint8 oxdir = xdir, oydir = ydir; // originals
        sint32 dc = 0, dir = ((ydir < 0 && dest.x < x)
                              || (xdir < 0 && dest.y >= y)
                              || (ydir > 0 && dest.x >= x)
                              || (xdir > 0 && dest.y <= y)) ? -1 : 1;
        if(dest2
           && ((ydir < 0 && dest2->x < x) || (xdir < 0 && dest2->y >= y)
               ||(ydir > 0 && dest2->x >= x) || (xdir > 0 && dest2->y <= y)))
            dir = -1;
        actor->set_direction(xdir, ydir);
        bool flip = false;
//printf(" %d,%d\n",xdir,ydir);
        while(!(moved = actor->moveRelative(xdir, ydir)) && (dc < 8))
        {
#if 0
            if(dc == 4) // checked all in one direction, now check other direction
                dir = (dir > 0) ? -4 : 4; // 4: rrrr, or -4: llll
            else
                dir = (dir > 0) ? 1 : -1; // 1: r, or -1: l
#endif
            // alternate directions
            xdir = oxdir; ydir = oydir;
            get_adjacent_dir(xdir, ydir, dir);
//printf(" turn %s %d -> %d,%d\n",dir>0?"right":"left",abs(dir),xdir,ydir);
            if(flip)
            {
                dir += (dir > 0) ? 1 : -1;
                dir = -dir;
                flip = false;
            }
            else
            {
                dir = -dir;
                flip = true;
            }
            ++dc;
            actor->set_direction(xdir, ydir);
        }

        // one more chance to move to destination (if only a tile away)
        // only if both moves are diagonal or both are straight
        if(x != dx && y != dy) // (and only if not at the destination already)
        {
            x = actor->x, y = actor->y;
            xdir = (x < dx) ? 1 : (x == dx) ? 0 : -1; // direction from current
            ydir = (y < dy) ? 1 : (y == dy) ? 0 : -1;
            if(((oxdir && oydir && xdir && ydir) // diag
               || (!(oxdir && oydir) && !(xdir && ydir))) // straight
               && (x + xdir) == dx && (y + ydir) == dy) // one tile away
            {
                actor->set_direction(xdir, ydir);
                actor->moveRelative(xdir, ydir);
            }
        }

    } while(--speed && x != dx && y != dy); // stop if at destination
}
