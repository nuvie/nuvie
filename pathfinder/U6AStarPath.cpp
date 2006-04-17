#include "nuvieDefs.h"

#include "Game.h"
#include "ObjManager.h"
#include "UseCode.h"
#include "U6AStarPath.h"

/* Return the cost of moving one step from `c1' to `c2'. An optional first of
 * three steps can be passed to avoid bending paths.
 * Blocking objects are checked for, and doors may be passable
 * Returns -1 if c2 is blocked.
 */
sint32 U6AStarPath::step_cost(MapCoord &c1, MapCoord &c2)
{
    sint32 c = 1;

    // FIXME: need an actor->check_move(loc2, loc1) to check one step only
    if(c2.distance(c1) > 1)
        return(-1);
    if(!pf->check_loc(c2.x, c2.y, c2.z))
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
