#include "Actor.h"
#include "Game.h"
#include "ObjManager.h"
#include "UseCode.h"
#include "AStar.h"


/* Create a list of tiles backwards from the goal node to the start.
 */
void AStar::create_path(astar_node *end)
{
    astar_node *i = end; // iterator through s, from back
    uint32 path_size = 0; // allocated elements
    step_count = 0;
    next_step = 0;
    free(path);
    path = NULL;

    while(i)
    {
        if(step_count >= path_size)
        {
            path_size += 32;
            path = (MapCoord*)realloc(path, path_size * sizeof(MapCoord));
        }
        path[step_count++] = i->loc;
        i = i->parent;
    }
    path = (MapCoord*)realloc(path, step_count * sizeof(MapCoord));
    next_step = (step_count > 0) ? step_count - 1 : 0;
}


/* Do A* search of tiles to create a path from `start' to `goal'.
 * Returns true if a path is created, or false if it can't be.
 */
bool AStar::node_search(MapCoord &start, MapCoord &goal)
{
printf("SEARCH: ACTOR %d\n", actor->get_actor_num());
uint32 blocked_count = 0; // number of tiles blocked completely
    astar_node *start_node = new astar_node;

    start_node->loc = start;
    start_node->to_start = 0;
    start_node->to_goal = estimate_cost(*start_node, goal);
    start_node->score = start_node->to_start + start_node->to_goal;
    push_open_node(start_node);

    while(!open_nodes.empty() && blocked_count < 300)
    {
        astar_node *nnode = pop_open_node();
        if(nnode->loc == goal || nnode->score >= max_node_score)
        {
            create_path(nnode);
            delete_nodes();
            return(true); // reached goal - success
        }
        // check cardinal neighbors (starting at top going clockwise)
        for(uint32 dir = 1; dir < 8; dir += 2)
        {
            sint8 sx = -1, sy = -1;
            get_adjacent_dir(sx, sy, dir); // sx,sy = neighbor -1,-1 + dir
            astar_node *neighbor = new astar_node;
            // get neighbor of nnode towards sx,sy
            neighbor->loc = nnode->loc.abs_coords(sx, sy);
            sint32 nnode_to_neighbor =
                            estimate_step_cost(nnode->loc, neighbor->loc);
            uint32 neighbor_to_start = nnode->to_start + nnode_to_neighbor;

            // this neighbor is blocked
            if(nnode_to_neighbor == -1)
            {
                ++blocked_count;
                delete neighbor;
                continue;
            }

            // ignore this neighbor if already checked and closer to goal
            astar_node *in_open = find_open_node(neighbor),
                       *in_closed = find_closed_node(neighbor);
            if((in_open && in_open->to_start <= neighbor_to_start)
               || (in_closed && in_closed->to_start <= neighbor_to_start))
            {
                delete neighbor;
                continue;
            }

            neighbor->parent = nnode;
            neighbor->to_start = neighbor_to_start;
            neighbor->to_goal = estimate_cost(*neighbor, goal);
            neighbor->score = neighbor->to_start + neighbor->to_goal;

            // too far away
            if(neighbor->score > max_node_score)
            {
                delete neighbor;
                continue;
            }

            // take neighbor out of closed list and put into open list
            if(in_closed)
                remove_closed_node(in_closed);
            if(!in_open)
                push_open_node(neighbor);
        }
        // node and neighbors checked, put into closed
        closed_nodes.push_back(nnode);
    }
printf(" FAIL\n");
    delete_nodes();
    return(false); // out of open nodes - failure
}


/* Return an estimate of the cost to location `g' from the location `n'.
 * Multiply by weight value of 1. (everything has the same terrain score)
 */
uint32 AStar::estimate_cost(astar_node &n, MapCoord &g)
{
    return(n.loc.distance(g));
}


/* Return an estimate of the cost to location `n2 from `n1'.
 */
uint32 AStar::estimate_cost(astar_node &n1, astar_node &n2)
{
    return(estimate_cost(n1, n2.loc));
}


/* Return an estimate of the cost from location `c1' to location `c2'. The two
 * tiles are assumed to be next to each other. Blocking objects are checked for
 * Returns -1 if c2 can not be stepped onto from c1, or the cost if it can.
 */
sint32 AStar::estimate_step_cost(MapCoord &c1, MapCoord &c2)
{
//    if(!actor->check_move(c2, c1))
    if(!actor->check_move(c2.x, c2.y, c2.z))
    {
        ObjManager *om = Game::get_obj_manager();
        UseCode *uc = Game::get_usecode();
        Obj *block = om->get_obj(c2.x, c2.y, c2.z);
        if(!block || !uc->is_unlocked_door(block))
            return(-1);
    }
    return((c1.x != c2.x && c1.y != c2.y) ? 2 : 1); // prefer non-diagonal
}


/* Return an item in the list of closed nodes whose location matches `ncmp'.
 */
astar_node *AStar::find_closed_node(astar_node *ncmp)
{
    std::list<astar_node *>::iterator n;
    for(n = closed_nodes.begin(); n != closed_nodes.end(); n++)
        if((*n)->loc == ncmp->loc)
            return(*n);
    return(NULL);
}


/* Return an item in the list of closed nodes whose location matches `ncmp'.
 */
astar_node *AStar::find_open_node(astar_node *ncmp)
{
    std::list<astar_node *>::iterator n;
    for(n = open_nodes.begin(); n != open_nodes.end(); n++)
        if((*n)->loc == ncmp->loc)
            return(*n);
    return(NULL);
}


/* Add new node pointer to the list of open nodes (sorting by score).
 */
void AStar::push_open_node(astar_node *node)
{
    std::list<astar_node *>::iterator n, next;
    if(open_nodes.empty())
    {
        open_nodes.push_front(node);
        return;
    }
    n = open_nodes.begin();
    // get to end of list or to a node with equal or greater score
    while(n != open_nodes.end() && (*n++)->score < node->score);
    open_nodes.insert(n, node); // and add before that location
}


/* Return pointer to the highest priority node from the list of open nodes, and
 * remove it.
 */
astar_node *AStar::pop_open_node()
{
    astar_node *best = open_nodes.front();
    open_nodes.pop_front(); // remove it
    return(best);
}


/* Find item in the list of closed nodes whose location matched `ncmp', and
 * remove it from the list.
 */
void AStar::remove_closed_node(astar_node *ncmp)
{
    std::list<astar_node *>::iterator n;
    for(n = closed_nodes.begin(); n != closed_nodes.end(); n++)
        if((*n)->loc == ncmp->loc)
        {
            *n = closed_nodes.front(); // copy front to current
            closed_nodes.pop_front(); // and remove front
        }
}


/* Delete nodes dereferenced from pointers in the lists.
 */
void AStar::delete_nodes()
{
    while(!open_nodes.empty())
    {
        astar_node *delnode = open_nodes.front();
        open_nodes.pop_front();
        delete delnode;
    }
    while(!closed_nodes.empty())
    {
        astar_node *delnode = closed_nodes.front();
        closed_nodes.pop_front();
        delete delnode;
    }
}
