
#include "Path.h"
#include "PathFinder.h"

PathFinder::PathFinder() : start(0,0,0), goal(0,0,0), loc(0,0,0), search(0)
{

}

PathFinder::PathFinder(MapCoord s, MapCoord g)
                   : start(s), goal(g), loc(0, 0, 0), search(0)
{

}

PathFinder::~PathFinder()
{
    delete search;
}

bool PathFinder::check_dir(const MapCoord &from, const MapCoord &rel)
{
    return check_loc(MapCoord(from.x+rel.sx,from.y+rel.sy,from.z));
}

bool PathFinder::check_loc(uint16 x, uint16 y, uint8 z)
{
    return check_loc(MapCoord(x,y,z));
}

void PathFinder::new_search(Path *new_path)
{
    delete search;
    search = new_path;
    search->set_pathfinder(this);
}

bool PathFinder::find_path()
{
    if(search)
    {
        if(search->have_path())
            search->delete_path();
        return(search->path_search(loc, goal));
    }
    return false; // no path-search object
}

bool PathFinder::have_path()
{
    return(search && search->have_path());
}

void PathFinder::set_goal(const MapCoord &g)
{
    goal = g;
    if(have_path())
        search->delete_path();
}

void PathFinder::set_start(const MapCoord &s)
{
    start = s;
    if(have_path())
        search->delete_path();
}

bool PathFinder::is_path_clear()
{
    uint32 num_steps = search->get_num_steps();
    for(unsigned int n=0; n<num_steps;n++)
    {
        MapCoord loc = search->get_step(n);
        if(!check_loc(loc))
            return false;
    }
    return true;
}
