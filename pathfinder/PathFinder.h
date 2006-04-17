#ifndef __PathFinder_h__
#define __PathFinder_h__

#include "Map.h"

class Path;

class PathFinder
{
protected:
    MapCoord start, goal, loc; /* source, destination, current location */

    Path *search; /* contains path-search algorithms, and
                                 game-specific step costs */

    void new_search(Path *new_path);
//    bool is_hazardous(MapCoord &loc); will have to check objects and tiles

public:
    PathFinder();
    PathFinder(MapCoord s, MapCoord g);
    virtual ~PathFinder();
    void set_search(Path *new_path) { new_search(new_path); }

    virtual void set_start(const MapCoord &s);
    virtual void set_goal(const MapCoord &g);
    virtual void set_location(const MapCoord &l) { loc = l; }

    virtual MapCoord get_location() { return loc; }
    virtual MapCoord get_goal()     { return goal; }
    virtual bool reached_goal()     { return(loc.x == goal.x && loc.y == goal.y
                                             && loc.z == goal.z); }

    virtual bool check_dir(const MapCoord &from, const MapCoord &rel);
    virtual bool check_loc(const MapCoord &loc) = 0;
    bool check_loc(uint16 x, uint16 y, uint8 z);

    virtual bool find_path(); /* get path to goal if one doesn't already exist */
    virtual bool have_path(); /* a working path exists */
    virtual bool is_path_clear(); /* recheck each location in path */
    virtual bool get_next_move(MapCoord &step) = 0;
};

#endif /* __PathFinder_h__ */
