#ifndef __LPath_h__
#define __LPath_h__

#include "AStar.h"

class Actor;
class MapCoord;

/* X Searches with A*. Actor makes wide straight turns along roads and passages.
 * X
 * XXX
 */
class LPath : public AStar
{
public:
    LPath(Actor *a, MapCoord &d, uint32 speed = 1);
    ~LPath() { }
    bool can_travel() { return(true); }

    //MapCoord *find_path(uint32 *sc, MapCoord start, MapCoord goal, uint32 max_steps);
    void walk_path(uint32 speed = 1);
    void set_dest(MapCoord &d);
};


#endif /* __LPath_h__ */
