#ifndef __AStarPath_h__
#define __AStarPath_h__
#include <list>
#include "Map.h"
#include "Path.h"
typedef struct astar_node_s
{    MapCoord loc; // location
    uint32 to_start; // costs from this node to start and to goal
    uint32 to_goal;
    uint32 score; // node score
    struct astar_node_s *parent;
    astar_node_s() : loc(0,0,0), to_start(0), to_goal(0), score(0),
                     parent(NULL) { }
} astar_node;
/* Provides A* search and cost methods for PathFinder and subclasses.
 */class AStarPath: public Path
{protected:
    std::list<astar_node *> open_nodes, closed_nodes; // nodes seen
    astar_node *final_node; // last node in path search, used by create_path()
    /* Forms a usable path from results of a search. */
    void create_path();
    /* Search routine. */
    bool search_node_neighbors(astar_node *nnode, MapCoord &goal, const uint32 max_score);
    bool compare_neighbors(astar_node *nnode, astar_node *neighbor,
                           sint32 nnode_to_neighbor, astar_node *in_open,
                           astar_node *in_closed);
    bool score_to_neighbor(sint8 dir, astar_node *nnode, astar_node *neighbor,
                           sint32 &nnode_to_neighbor);
public:
    AStarPath();
    ~AStarPath() { }
    bool path_search(MapCoord &start, MapCoord &goal);
    uint32 path_cost_est(astar_node &n1, astar_node &n2) { return(Path::path_cost_est(n1.loc, n2.loc)); }
    sint32 step_cost(MapCoord &c1, MapCoord &c2);
protected:
    /* FIXME: These node functions can be replaced with a priority_queue and a list. */
    astar_node *find_open_node(astar_node *ncmp);
    void push_open_node(astar_node *node);
    astar_node *pop_open_node();
    astar_node *find_closed_node(astar_node *ncmp);
    void remove_closed_node(astar_node *ncmp);
    void delete_nodes();
};
#endif /* __AStarPath_h__ */
