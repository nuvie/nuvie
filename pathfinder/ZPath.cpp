#include <cstdlib>
#include <vector>
#include "nuvieDefs.h"
#include "Actor.h"
#include "Game.h"
#include "ActorManager.h"
#include "ZPath.h"

using std::vector;

ZPath::ZPath(Actor *a, MapCoord &d, uint32 spd)
{
    map = Game::get_game()->get_game_map();
    actor_manager = Game::get_game()->get_actor_manager();

    actor = a;
    set_dest(d);
    speed = spd ? spd : 1; // default: take 1 step at a time
    wait(0);
}


/* Move actor towards x,y.
 * Returns true on successful move.
 */
bool ZPath::move_actor(uint16 x, uint16 y)
{
    sint8 xdir = (actor->x < x) ? 1 : (actor->x == x) ? 0 : -1;
    sint8 ydir = (actor->y < y) ? 1 : (actor->y == y) ? 0 : -1;
    if(xdir == 0 && ydir == 0)
        return(true);
    if(!actor->moveRelative(xdir, ydir))
        return(false);
    actor->set_direction(xdir, ydir);
    return(true);
}


/* Returns the number of steps taken (up to `count') to reach the destination.
 */
uint32 ZPath::try_step(MapCoord &d, uint32 count)
{
    uint32 steps_taken = 0;
    sint8 xdir = (actor->x < d.x) ? 1 : (actor->x == d.x) ? 0 : -1;
    sint8 ydir = (actor->y < d.y) ? 1 : (actor->y == d.y) ? 0 : -1;
// FIXME: this actor check is so that actors in a line don't push someone who is
// in position back out of the way, taking their moves, but sometimes actors in
// position can be pushed; at least replace with a no-push move flag
    while((!actor_manager->get_actor(actor->x+xdir,actor->y+ydir,actor->z)
           || actor_manager->get_actor(actor->x+xdir,actor->y+ydir,actor->z)->id_n == 1)
          && move_actor(d.x, d.y))
        if(++steps_taken >= count || (d.x == actor->x && d.y == actor->y)) // moved in primary direction
            return(steps_taken); // out of moves
    // will try to move around obstacle (and not come back to second_start)
    MapCoord second_start(actor->x, actor->y, actor->z), here(second_start);
    if(steps_taken < count && here != d)
    {
        // original vector (from second_start)
        xdir = (actor->x < d.x) ? 1 : (actor->x == d.x) ? 0 : -1;
        ydir = (actor->y < d.y) ? 1 : (actor->y == d.y) ? 0 : -1;
        // find secondary direction to `d' (left or right from primary)
        uint8 dir = get_closest_dir(second_start, d);
        // just wait (don't go backwards) if blocked by party member
        if(dir != 1 && dir != 7)
        {
            MapCoord blocked = here.abs_coords(xdir, ydir);
            Actor *blocker = actor_manager->get_actor(blocked.x, blocked.y, blocked.z);
            if(blocker && blocker->in_party && actor->in_party)
                return(steps_taken);
        }
        if(dir)
        {
            get_adjacent_dir(xdir, ydir, dir); // modify vector
            if(move_actor(actor->x + xdir, actor->y + ydir)) // try again
                ++steps_taken;
            here.x = actor->x; here.y = actor->y;
            if(here == d) return(steps_taken); // easy!
            if(steps_taken < count && move_actor(d.x, d.y)) // try primary once more
            {
                here.x = actor->x; here.y = actor->y;
                if(here != second_start || here == d) // havn't gone backwards
                    return(++steps_taken); // still easy (don't search yet)
            }
        }
        // second step failed, or third didn't reach goal (search below)
        here.x = actor->x; here.y = actor->y;
    }
    else if(here == d) return(steps_taken); // got to destination on first try!
    // will have to find some path-nodes to walk around
// FIXME: use dir to original move instead?
    xdir = (actor->x < d.x) ? 1 : (actor->x == d.x) ? 0 : -1;
    ydir = (actor->y < d.y) ? 1 : (actor->y == d.y) ? 0 : -1;
    MapCoord blocked = here.abs_coords(xdir, ydir);
    if((blocked != d)
       && !actor_manager->get_actor(blocked.x, blocked.y, blocked.z))
        if(avoid_obstacle(here, d) && steps_taken < count)
        {
            blocked = get_next_step();
            if(move_actor(blocked.x, blocked.y)) // try first step in path
                ++steps_taken;
        }
    return(steps_taken); // that's enough for ZPath (will return if possible)
}


void ZPath::walk_path(uint32 step_speed)
{
    uint32 s = 0;
    if(delay)// don't walk yet
    {
        --delay;
        return;
    }
    step_speed = (step_speed != 0) ? step_speed : speed;
    actor->moves = step_speed;
    do
    {
        MapCoord start(actor->x, actor->y, actor->z);
        MapCoord goal = get_next_step();
        if(start == goal)
        {
            if(!using_path()) // at dest
               break;
            pop_dest();
	    continue;
        }
        uint32 s_add = try_step(goal, step_speed - s);
        s += (s_add != 0) ? s_add : 1; // count unsuccessful tries
        MapCoord result(actor->x, actor->y, actor->z);
        // if at goal and it's an obstacle-avoidance node, remove it from path
        if(result == goal && using_path())
            pop_dest();
    } while(s < step_speed);
    if(s) actor->moves = 0; // can't move again this turn
}


/* Return direction 0-7 to neighbor tile of `start' that is closest to `d1'.
 * The second direction `d2' defines a bounding region that the step will try
 * to stay within. Moves with a direct line-of-site to target are preferred.
 * (dir0=original dir, 7=left, 1=right)
 */
// FIXME: use d2, and fix distance check
uint8
ZPath::get_closest_dir(MapCoord &start, MapCoord &d1, MapCoord &d2, uint8 skip)
{
    LineTestResult lt;
    uint8 dir = 0, cdir = 0, tries = 0;
    sint8 turn = 0;
    sint16 xdiff = d1.x - start.x, ydiff = d1.y - start.y;
    sint8 xdir = (xdiff == 0) ? 0 : (xdiff < 0) ? -1 : 1, ydir = (ydiff == 0) ? 0 : (ydiff < 0) ? -1 : 1;
    sint8 oxdir = xdir, oydir = ydir;
    uint32 cdist = 0;
    // turn towards closest axis (furthest distance)
    if(xdiff == 0 || ydiff == 0 || xdiff == ydiff) // moving in a straight line, or same distances
        turn = -1;
    else if(xdiff > ydiff) // moving diagonal, X-axis closest
        turn = (xdir == -1 && ydir == -1) ? -1 :
              (xdir == +1 && ydir == -1) ? 1 :
              (xdir == +1 && ydir == +1) ? -1 :
              (xdir == -1 && ydir == +1) ? 1 : 0;
    else if(ydiff > xdiff) // moving diagonal, Y-axis closest
        turn = (xdir == -1 && ydir == -1) ? 1 :
              (xdir == +1 && ydir == -1) ? -1 :
              (xdir == +1 && ydir == +1) ? 1 :
              (xdir == -1 && ydir == +1) ? -1 : 0;
    dir = (turn < 0) ? 7 : 1;
    cdist = start.xdistance(d1) + start.ydistance(d1);
//    cdist = (xdiff + xdir) + (ydiff + ydir); cdist not used if cdir==0
    while(tries < 7)
    {
        get_adjacent_dir(xdir, ydir, turn);
        if(actor->check_move(start.x+xdir,start.y+ydir,start.z))
        {
            // if there is a direct line-of-site, return immediately
            if(!map->lineTest(start.x+xdir,start.y+ydir,d1.x,d1.y,start.z,LT_HitUnpassable,lt))
                return(dir);
            // get distance, see if this dir is closer than closest
            MapCoord neighbor(start.x+xdir, start.y+ydir, start.z);
            if(cdir == 0 || (neighbor.xdistance(d1) + neighbor.ydistance(d1)) < cdist)
            {
                cdist = neighbor.xdistance(d1) + neighbor.ydistance(d1);
                cdir = dir;
            }
        }
        turn += (turn < 0) ? -1 : 1;
        if(tries < 4) // switch directions (except for last 3 tries)
            turn = -turn;
        if(tries == 0)      dir = (turn < 0) ? 7 : 1;
        else if(tries == 1) dir = (turn < 0) ? 7 : 1;
        else if(tries == 2) dir = (turn < 0) ? 6 : 2;
        else if(tries == 3) dir = (turn < 0) ? 6 : 2;
        else if(tries == 4) dir = (turn < 0) ? 5 : 3;
        else if(tries == 5) dir = 4; // opposite original move
        else if(tries == 6) dir = (turn < 0) ? 3 : 5; // opposite turn
        ++tries;
    }
    return(cdir);
}


/* Scan edges of an obstacle (typically a wall) in two directions until a
 * passable tile towards the original direction is hit.
 * Returns true if a path was created, false if no route is found or needed.
 */ // FIXME: this is still too long & complicated
bool ZPath::avoid_obstacle(MapCoord &start, MapCoord &goal)
{
    LineTestResult lt;
    uint32 scan_max = 8; // number of scans (each direction) before giving up
    sint8 xdir = ((goal.x-start.x)==0) ? 0 : ((goal.x-start.x)<0) ? -1 : 1,
          ydir = ((goal.y-start.y)==0) ? 0 : ((goal.y-start.y)<0) ? -1 : 1;
    sint32 Axdir = 0, Aydir = 0; // vector of line segment A relative to start
    sint32 Bxdir = 0, Bydir = 0;
    sint32 Acx = xdir, Acy = ydir; // directions from line to scan
    sint32 Bcx = xdir, Bcy = ydir;
    MapCoord pointA(start), pointB(start); // end of line segments (scan from)
    std::vector<MapCoord> *Anodes = NULL, // points to be added to path
                          *Bnodes = NULL;
    bool scanA = true, scanB = true; // still scanning this direction?
    bool Abend = false, Bbend = false; // lines are rotated (scan blocking wall)

    if(start == goal || ((start.x+xdir) == goal.x && (start.y+ydir) == goal.y))
        return(false);

    // determine if each line will be vertical or horizontal
    get_obstacle_tracer(start, xdir, ydir, Axdir, Aydir, Bxdir, Bydir);
    Acx = (Aydir) ? xdir : 0, Acy = (Axdir) ? ydir : 0;
    Bcx = (Bydir) ? xdir : 0, Bcy = (Bxdir) ? ydir : 0;

    uint32 s = 0;
    while(s++ < scan_max)
    {
        if(scanA)
        {
            pointA.x += Axdir; pointA.y += Aydir;
            MapCoord check_point(pointA.x + Acx, pointA.y + Acy, pointA.z);
            bool clear_to_goal = !map->lineTest(check_point.x,check_point.y,goal.x,goal.y,goal.z,LT_HitUnpassable,lt);
            // scan line blocked
            if(!actor->check_move(pointA.x, pointA.y, pointA.z, ACTOR_IGNORE_OTHERS))
            {
                // bend line away from check direction (if not already bent)
                if(!Abend)
                {
                    if(!Anodes) Anodes = new std::vector<MapCoord>;
                    pointA.x -= Axdir; pointA.y -= Aydir; // step back
                    Anodes->push_back(pointA); // new line curve
                    sint32 old_Acx = Acx, old_Acy = Acy;
                    Acx = Axdir; Acy = Aydir; // new check direction
                    Axdir = -old_Acx; Aydir = -old_Acy; // new line direction
                    Abend = true;
                }
                else // can't bend again
                {
                    if(Anodes) delete Anodes;
                    Anodes = NULL;
                    scanA = false; // no path
                }
            }
            else if(actor->check_move(check_point.x, check_point.y, check_point.z, ACTOR_IGNORE_OTHERS)
                    || clear_to_goal)
            {
                if(!Abend || clear_to_goal) // line is not bent
                {
                    if(!Anodes) Anodes = new std::vector<MapCoord>;
                    Anodes->push_back(pointA);
                    Anodes->push_back(check_point);
                    scanA = false; // found path
                    if(clear_to_goal) break; // don't even try other side
                }
                else // unbend line (towards check direction)
                {
                    if(!Anodes) Anodes = new std::vector<MapCoord>;
                    Anodes->push_back(pointA); // new line curve
                    pointA = check_point; // step fowards to check_point
                    sint32 old_Axdir = Axdir, old_Aydir = Aydir;
                    Axdir = Acx; Aydir = Acy; // new line direction
                    Acx = -old_Axdir; Acy = -old_Aydir; // new check direction
                    Abend = false;
                }
            }
        }
        if(scanB)
        {
            pointB.x += Bxdir; pointB.y += Bydir;
            MapCoord check_point(pointB.x + Bcx, pointB.y + Bcy, pointB.z);
            bool clear_to_goal = !map->lineTest(check_point.x,check_point.y,goal.x,goal.y,goal.z,LT_HitUnpassable,lt);
            // scan line blocked
            if(!actor->check_move(pointB.x, pointB.y, pointB.z, ACTOR_IGNORE_OTHERS))
            {
                // bend line away from check direction (if not already bent)
                if(!Bbend)
                {
                    if(!Bnodes) Bnodes = new std::vector<MapCoord>;
                    pointB.x -= Bxdir; pointB.y -= Bydir; // step back
                    Bnodes->push_back(pointB); // new line curve
                    sint32 old_Bcx = Bcx, old_Bcy = Bcy;
                    Bcx = Bxdir; Bcy = Bydir; // new check direction
                    Bxdir = -old_Bcx; Bydir = -old_Bcy; // new line direction
                    Bbend = true;
                }
                else // can't bend again
                {
                    if(Bnodes) delete Bnodes;
                    Bnodes = NULL;
                    scanB = false; // no path
                }
            }
            else if(actor->check_move(check_point.x, check_point.y, check_point.z, ACTOR_IGNORE_OTHERS)
                    || clear_to_goal)
            {
                if(!Bbend || clear_to_goal) // line is not bent
                {
                    if(!Bnodes) Bnodes = new std::vector<MapCoord>;
                    Bnodes->push_back(pointB);
                    Bnodes->push_back(check_point);
                    scanB = false; // found path
                    if(clear_to_goal) break; // don't even try other side
                }
                else // unbend line (towards check direction)
                {
                    if(!Bnodes) Bnodes = new std::vector<MapCoord>;
                    Bnodes->push_back(pointB); // new line curve
                    pointB = check_point; // step fowards to check_point
                    sint32 old_Bxdir = Bxdir, old_Bydir = Bydir;
                    Bxdir = Bcx; Bydir = Bcy; // new line direction
                    Bcx = -old_Bxdir; Bcy = -old_Bydir; // new check direction
                    Bbend = false;
                }
            }
        }
    }

    std::vector<MapCoord> *nodes = NULL; // which line to follow (closest)
    if(Anodes)
    {
        nodes = Anodes;
        if(Bnodes)
        {
            pointA = Anodes->back(), pointB = Bnodes->back();
            if((pointB.xdistance(goal)+pointB.ydistance(goal)) < (pointA.xdistance(goal)+pointA.ydistance(goal)))
                nodes = Bnodes;
        }
    }
    else if(Bnodes) nodes = Bnodes;
    while(nodes && !nodes->empty()) // found point(s) past the obstacle
    {
        push_dest(nodes->back());
        nodes->pop_back();
    }
    if(Anodes) delete Anodes;
    if(Bnodes) delete Bnodes;
    return(nodes ? true : false);
}


/* Get two relative directions that a line can travel to trace around an
 * obstacle towards `xdir',`ydir'.
 */
void inline
ZPath::get_obstacle_tracer(MapCoord &start, sint32 xdir, sint32 ydir, sint32 &Axdir,
                           sint32 &Aydir, sint32 &Bxdir, sint32 &Bydir)
{
    if(xdir && ydir) // original direction is diagonal
    {
        if(actor->check_move(start.x + xdir, start.y, start.z)) // can go in X
        {
            Axdir = xdir; Aydir = 0; // Horizontal; in X direction
        }
        else // X is blocked, must go in Y
        {
            Axdir = 0; Aydir = -ydir; // Vertical; opposite Y direction
        }
        if(actor->check_move(start.x, start.y + ydir, start.z)) // can go in Y
        {
            Bxdir = 0; Bydir = ydir; // Vertical; in Y direction
        }
        else // Y is blocked, must go in X
        {
            Bxdir = -xdir; Bydir = 0; // Horizontal; opposite X direction
        }
    }
    else
    {
        // scan in straight line, opposite directions
        Axdir=ydir;   Aydir=xdir;
        Bxdir=-Axdir; Bydir=-Aydir;
    }
}


/* Add obstacle-avoidance node to path.
 */
void ZPath::push_dest(MapCoord &d3)
{
    path = (MapCoord*)realloc(path, sizeof(MapCoord) * (step_count + 1));
    path[step_count] = d3;
    ++step_count;
}


/* Returns next obstacle-avoidance node, and removes it from path.
 */
MapCoord ZPath::pop_dest()
{
    MapCoord d3(dest.x,dest.y,dest.z);
    if(step_count)
    {
        d3 = path[step_count - 1];
        path = (MapCoord*)realloc(path, sizeof(MapCoord) * (step_count - 1));
        --step_count;
    }
    if(!step_count) path = NULL;
    return(d3);
}


/* Return next obstacle-avoidance node in path, or `dest' if there is no path.
 */
inline MapCoord ZPath::get_next_step()
{
    if(using_path())
        return(path[step_count - 1]);
    return(dest);
}
