#ifndef __AStar_h__
#define __AStar_h__

#include <list>
#include "PathFinder.h"

class MapCoord;

typedef struct astar_node_s
{
    MapCoord loc; // location
    uint32 to_start; // costs from this node to start and to goal
    uint32 to_goal;
    uint32 score; // node score
    struct astar_node_s *parent;
    astar_node_s() : loc(0,0,0), to_start(0), to_goal(0), score(0),
                     parent(NULL) { }
} astar_node;

/* Provides A* search and cost methods for subclasses.
 */
class AStar : public PathFinder
{
protected:
    list<astar_node *> open_nodes, closed_nodes; // nodes seen
    uint32 max_node_score; // highest score any node can have

protected:
    void create_path(astar_node *end);
    bool node_search(MapCoord &start, MapCoord &goal);

    astar_node *find_open_node(astar_node *ncmp);
    void push_open_node(astar_node *node);
    astar_node *pop_open_node();

    astar_node *find_closed_node(astar_node *ncmp);
    void remove_closed_node(astar_node *ncmp);

    void delete_nodes();

    uint32 estimate_cost(astar_node &n, MapCoord &g);
    uint32 estimate_cost(astar_node &n1, astar_node &n2);
    sint32 estimate_step_cost(MapCoord &c1, MapCoord &c2);
};

#endif /* __AStar_h__ */
