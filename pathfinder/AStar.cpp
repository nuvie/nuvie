
#include "nuvieDefs.h"

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
    next_step = (step_count > 1) ? step_count - 2 : 0; // skip last
}


/* Do A* search of tiles to create a path from `start' to `goal'. Don't search
 * past nodes with a score over the max. score, and create a partial path to
 * low-score nodes with a distance-to-start over the max. step count.
 * Returns true if a path is created, or false if it can't be.
 */
bool AStar::node_search(MapCoord &start, MapCoord &goal)
{
//printf("SEARCH: %d: %d,%d -> %d,%d\n",actor->get_actor_num(),start.x,start.y,goal.x,goal.y);
    uint32 max_score, max_steps;
    astar_node *start_node = new astar_node;
    start_node->loc = start;
    start_node->to_start = 0;
    start_node->to_goal = path_cost(start, goal);
    start_node->score = start_node->to_start + start_node->to_goal;
    push_open_node(start_node);

    max_score = get_max_score(start_node->to_goal);
    max_steps = 8*2*4; // walk up to four screen lengths before searching again

    while(!open_nodes.empty()/* && blocked_count < 300*/)
    {
        astar_node *nnode = pop_open_node(); // next closest
        if(nnode->loc == goal || nnode->to_start >= max_steps)
        {
//printf("GOAL\n");
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
                            step_cost(nnode->loc, neighbor->loc);
            // this neighbor is blocked
            if(nnode_to_neighbor == -1)
            {
                //++blocked_count;
                delete neighbor;
                continue;
            }
            uint32 neighbor_to_start = nnode->to_start + nnode_to_neighbor;

            // ignore this neighbor if already checked and closer to start
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
            neighbor->to_goal = path_cost(neighbor->loc, goal);
            neighbor->score = neighbor->to_start + neighbor->to_goal;

            // too far away
            if(neighbor->score > max_score)
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
//printf("FAIL\n");
    delete_nodes();
    return(false); // out of open nodes - failure
}


/* Take estimate of a path, and return the highest allowed score of any nodes
 * in the search of that path.
 */
uint32 AStar::get_max_score(uint32 cost)
{
    uint32 max_score = cost * 2;
    // search at least this far (else short paths will have too
    //                           low of a maximum score to move around walls)
    if(max_score < 8*2*3)
        max_score = 8*2*3;
    return(max_score);
}


/* Return a weighted estimate of the highest cost from location `s' to `n'.
 */
uint32 AStar::path_cost(MapCoord &s, MapCoord &g)
{
    uint32 major = (s.xdistance(g) >= s.ydistance(g))
                   ? s.xdistance(g) : s.ydistance(g);
    uint32 minor = (s.xdistance(g) >= s.ydistance(g))
                   ? s.ydistance(g) : s.xdistance(g);
    return(2*major + minor);
}


/* Return the cost of moving from location `c1' to neighboring location `c2'.
 * Blocking objects are checked for, and doors may be passable
 * Returns -1 if c2 is blocked. Returns the cost if it isn't blocked.
 */
sint32 AStar::step_cost(MapCoord &c1, MapCoord &c2)
{
    sint32 c = 1;
//    if(!actor->check_move(c2, c1))
    if(!actor->check_move(c2.x, c2.y, c2.z, ACTOR_IGNORE_OTHERS))
    {
        // check for door
        Game *game = Game::get_game();
        Obj *block = game->get_obj_manager()->get_obj(c2.x, c2.y, c2.z);
        // HACK: check the neighboring tiles for the "real" door
        Obj *real = game->get_obj_manager()->get_obj(c2.x + 1, c2.y, c2.z);
        if(!real || !game->get_usecode()->is_unlocked_door(real))
            real = game->get_obj_manager()->get_obj(c2.x, c2.y + 1, c2.z);
        if(!block || !game->get_usecode()->is_unlocked_door(block) || real)
            return(-1);
        c += 2;
    }
    if(c1.x != c2.x && c1.y != c2.y) // prefer non-diagonal
        ++c;
    return(c);
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
