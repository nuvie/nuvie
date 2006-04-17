#ifndef __U6AStarPath_h__
#define __U6AStarPath_h__

#include "AStarPath.h"

/* This provides a U6-specific step_cost() method. */
class U6AStarPath: public AStarPath
{
public:
    sint32 step_cost(MapCoord &c1, MapCoord &c2);
};


#endif /* __U6AStarPath_h__ */
