#ifndef __SeekPath_h__
#define __SeekPath_h__

#include <vector>
#include "Path.h"

/* Provides routines for building short paths around obstacles and seeking a
 * target. Much of the work doesn't involve finding a path at all, but instead
 * finding the direction closest to the target.
 */
class SeekPath: public Path
{
protected:
    std::vector<MapCoord> A_scan, B_scan; // nodes of a line scanned by trace_obstacle()

    void create_path(MapCoord &start, MapCoord &goal);
    std::vector<MapCoord>* get_best_scan(MapCoord &start, MapCoord &goal);
    void delete_nodes();
    bool trace_check_obstacle(bool &turned, MapCoord &line, sint32 &deltax, sint32 &deltay, sint32 &xdir, sint32 &ydir, std::vector<MapCoord> *scan);
    void trace_around_corner(MapCoord &line, sint32 &deltax, sint32 &deltay, sint32 &xdir, sint32 &ydir, std::vector<MapCoord> *scan);

public:
    SeekPath();
    ~SeekPath();
    sint32 step_cost(MapCoord &c1, MapCoord &c2) { return -1; }
    bool path_search(MapCoord &start, MapCoord &goal);
    void delete_path() { Path::delete_path(); delete_nodes(); }

    /* Trace obstacle towards xdir,ydir for a possible opening. */
    bool trace_obstacle(MapCoord line, sint32 deltax, sint32 deltay, sint32 xdir, sint32 ydir, std::vector<MapCoord> *scan);
    /* Get two relative directions that a line can travel to trace around an
       obstacle towards `xdir',`ydir'. */
    bool get_obstacle_tracer(MapCoord &start, sint32 xdir, sint32 ydir,
                             sint32 &Axdir, sint32 &Aydir,
                             sint32 &Bxdir, sint32 &Bydir);
};

#endif /* __SeekPath_h__ */
