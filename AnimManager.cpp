#include <cmath>
#include "SDL.h"
#include "nuvieDefs.h"
#include <vector>

#include "Actor.h"
#include "ActorManager.h"
#include "Game.h"
#include "Map.h"
#include "MapWindow.h"
#include "ObjManager.h"
#include "Screen.h"
#include "Text.h"
#include "AnimManager.h"

#define MESG_ANIM_HIT_WORLD ANIM_CB_HIT_WORLD
#define MESG_ANIM_HIT       ANIM_CB_HIT
#define MESG_ANIM_DONE      ANIM_CB_DONE

static float get_relative_degrees(sint16 sx, sint16 sy, float angle_up = 0);


/* Convert a non-normalized relative direction (difference) from any center
 * point to degrees, where direction 0,-1 (up) is `angle_up' degrees.
 * Returns the angle.
 */
static float get_relative_degrees(sint16 sx, sint16 sy, float angle_up)
{
    float angle = 0;
    uint16 x = abs(sx), y = abs(sy);

    if(sx > 0 && sy < 0)
        angle = (x > y) ? 68 : (y > x) ? 23 : 45;
    else if(sx > 0 && sy == 0)
        angle = 90;
    else if(sx > 0 && sy > 0)
        angle = (x > y) ? 113 : (y > x) ? 158 : 125;
    else if(sx == 0 && sy > 0)
        angle = 180;
    else if(sx < 0 && sy > 0)
        angle = (x > y) ? 248 : (y > x) ? 203 : 225;
    else if(sx < 0 && sy == 0)
        angle = 270;
    else if(sx < 0 && sy < 0)
        angle = (x > y) ? 293 : (y > x) ? 338 : 315;
    // (sx == 0 && sy < 0) = 0

    angle += angle_up;
    if(angle >= 360)
        angle -= 360;
    return(angle);
}


AnimManager::AnimManager(Screen *screen, SDL_Rect *clipto)
                        : next_id(0)
{
        map_window = Game::get_game()->get_map_window();
        tile_pitch = 16;

        viewsurf = screen;
        if(clipto)
            viewport = *clipto;
}


/* Returns an iterator to the animation with requested id_n.
 */
AnimIterator AnimManager::get_anim_iterator(uint32 anim_id)
{
        AnimIterator i = anim_list.begin();
        while(i != anim_list.end())
        {
            if((*i)->id_n == anim_id)
                return(i);
            ++i;
        }
        return(anim_list.end());
}


/* Returns animation with requested id_n.
 */
NuvieAnim *AnimManager::get_anim(uint32 anim_id)
{
        AnimIterator i = get_anim_iterator(anim_id);
        if(i != anim_list.end())
            return(*i);
        return(NULL);
}


/* Update all animations.
 */
void AnimManager::update()
{
        AnimIterator i = anim_list.begin();
        while(i != anim_list.end())
        {
            (*i)->updated = (*i)->update();
            ++i;
        }

        // remove completed animations
        i = anim_list.begin();
        while(i != anim_list.end())
            if(!(*i)->running)
            {
                destroy_anim(*i);
                i = anim_list.begin();
            }
            else ++i;
}


/* Draw all animations that have been updated.
 */
void AnimManager::display()
{
        AnimIterator i = anim_list.begin();
        while(i != anim_list.end())
        {
            if((*i)->updated)
            {
                (*i)->display();
                (*i)->updated = false;
            }
            ++i;
        }
}


/* Add animation pointer to managed list.
 * Returns id_n of new animation for future reference.
 */
sint32 AnimManager::new_anim(NuvieAnim *new_anim)
{
        if(new_anim)
        {
            new_anim->id_n = next_id++;
            new_anim->anim_manager = this;
            anim_list.push_back(new_anim);
            new_anim->start();
            return((uint32)new_anim->id_n);
        }
        fprintf(stderr, "Anim: tried to add NULL anim\n");
        return(-1);
}


/* Delete all animations.
 */
void AnimManager::destroy_all()
{
    while(!anim_list.empty())
        if(!destroy_anim(*anim_list.begin()))
            break;
}


/* Delete an animation.
 */
bool AnimManager::destroy_anim(uint32 anim_id)
{
    if(anim_id >= 0)
        return(destroy_anim(get_anim(anim_id)));
     return(false);
}


/* Delete an animation.
 */
bool AnimManager::destroy_anim(NuvieAnim *anim_pt)
{
        AnimIterator i = get_anim_iterator(anim_pt->id_n);
        if(i != anim_list.end())
        {
//            (*i)->message(MESG_ANIM_DONE); // FIXME: for now Anims send this for various reasons
            if((*i)->safe_to_delete)
                delete *i;
            anim_list.erase(i);
            return(true);
        }
        fprintf(stderr, "Anim: error deleting %d\n", anim_pt->id_n);
        return(false);
}


/* Draw tile on viewsurf at x,y.
 */
void AnimManager::drawTile(Tile *tile, uint16 x, uint16 y)
{
    viewsurf->blit(x, y, tile->data, 8, tile_pitch, tile_pitch, 16,
                   tile->transparent, &viewport);
}


/* Draw tile on viewsurf at map location wx,wy (offset by add_x,add_y).
 */
void AnimManager::drawTileAtWorldCoords(Tile *tile, uint16 wx, uint16 wy,
                                                    uint16 add_x, uint16 add_y)
{
    sint16 cur_x = map_window->cur_x;
    sint16 cur_y = map_window->cur_y;
    drawTile(tile, ((wx - cur_x) * tile_pitch) + add_x,
                   ((wy - cur_y) * tile_pitch) + add_y);
}


/*** NuvieAnim ***/
NuvieAnim::NuvieAnim()
{
    anim_manager = NULL;

    id_n = 0;

    vel_x = vel_y = 0;
    px = py = 0;

    safe_to_delete = true;
    updated = true;
    running = true;

    last_move_time = SDL_GetTicks();
}


NuvieAnim::~NuvieAnim()
{

}


/* Set velocity (x,y) for moving towards a target at distance (xdir,ydir).
 */
void NuvieAnim::set_velocity_for_speed(sint16 xdir, sint16 ydir, uint32 spd)
{
    uint16 max_dist, min_dist;
    uint16 xdist = abs(xdir), ydist = abs(ydir);
    sint32 xvel = spd, yvel = spd;

    if(xdir == 0 || ydir == 0)
    {
        if(xdir == 0) xvel = 0;
        if(ydir == 0) yvel = 0;
    }
    else if(xdist > ydist) // MAX_DIR = xdir
    {
        max_dist = xdist;
        min_dist = ydist;
        yvel = xvel / (max_dist / min_dist); // max_vel/min_vel = max_dir/min_dir
    }
    else // MAX_DIR = ydir
    {
        min_dist = xdist;
        max_dist = ydist;
        xvel = yvel / (max_dist / min_dist);
    }
    set_velocity((xdir == 0) ? 0 : (xdir > 0) ? xvel : -xvel,
                 (ydir == 0) ? 0 : (ydir > 0) ? yvel : -yvel);
}


/* Move per velocity.
 */
void NuvieAnim::update_position()
{
    uint32 this_time = SDL_GetTicks();

    if(this_time - last_move_time >= 100) // only move every 10th sec
    {
        sint32 vel_x_incr = vel_x / 10, vel_y_incr = vel_y / 10;
        if(vel_x && !vel_x_incr) // move even if fps was < 10
            vel_x_incr = (vel_x < 0) ? -1 : 1;
        if(vel_y && !vel_y_incr)
            vel_y_incr = (vel_y < 0) ? -1 : 1;

        shift(vel_x_incr, vel_y_incr);
        last_move_time = this_time;
    }
}


/*** TileAnim ***/
TileAnim::TileAnim()
{
    map_window = Game::get_game()->get_map_window();
    tx = ty = px = py = 0;
}


TileAnim::~TileAnim()
{
    while(tiles.size())
        remove_tile();
}


/* Find a tile in the animation and return its id or list position.
 * Returns -1 if it isn't found.
 */
sint32 TileAnim::get_tile_id(PositionedTile *find_tile)
{
    uint32 tile_count = tiles.size();
    for(uint32 t = 0; t < tile_count; t++)
        if(find_tile == tiles[t])
            return(t);
    return(-1);
}


/* Display ordered tile list.
 */
void TileAnim::display()
{
    uint32 tcount = tiles.size();
    for(sint32 t = (tcount-1); t >= 0; t--)
        anim_manager->drawTileAtWorldCoords(tiles[t]->tile,
                                            tx+tiles[t]->pos_x,ty+tiles[t]->pos_y,
                                            px+tiles[t]->px, py+tiles[t]->py);
}


/* Add tile to list for this Anim, relative to the Anim coordinates.
 */
PositionedTile *TileAnim::add_tile(Tile *tile, sint16 x, sint16 y,
                                   uint16 add_x, uint16 add_y)
{
    PositionedTile *new_tile = new PositionedTile;
    new_tile->tile = tile;
    new_tile->pos_x = x;
    new_tile->pos_y = y;
    new_tile->px = add_x;
    new_tile->py = add_y;
    tiles.insert(tiles.begin(), new_tile);
    return(new_tile);
}


/* Remove tile from list position `i'. (default = last added)
 */
void TileAnim::remove_tile(uint32 i)
{
    if(i < tiles.size())
    {
        //vector<PositionedTile*>::iterator ti = &tiles[i];
        std::vector<PositionedTile*>::iterator ti = tiles.begin();
        for(uint32 j = 0; j < i; j++)
            ti++;

        delete *ti;
        tiles.erase(ti);
    }
}


/* Move tile(s) by the pixel, in direction sx,sy.
 */
void TileAnim::shift(sint32 sx, sint32 sy)
{
    uint8 tile_pitch = anim_manager->get_tile_pitch();
    uint32 total_px = (tx * tile_pitch) + px, total_py = (ty * tile_pitch) + py;
    total_px += sx;
    total_py += sy;
    move(total_px / tile_pitch, total_py / tile_pitch,
         total_px % tile_pitch, total_py % tile_pitch);
}


/* Move a tile by the pixel, relative to the animation, in direction sx,sy.
 */
void TileAnim::shift_tile(uint32 ptile_num, sint32 sx, sint32 sy)
{
    if(tiles.size() <= ptile_num)
        return;

    uint8 tile_pitch = anim_manager->get_tile_pitch();
    uint16 tx = tiles[ptile_num]->pos_x, ty = tiles[ptile_num]->pos_y;
    uint16 px = tiles[ptile_num]->px, py = tiles[ptile_num]->py;
    uint32 total_px = (tx * tile_pitch) + px, total_py = (ty * tile_pitch) + py;
    total_px += sx;
    total_py += sy;
    // move
    tiles[ptile_num]->pos_x = total_px / tile_pitch;
    tiles[ptile_num]->pos_y = total_py / tile_pitch;
    tiles[ptile_num]->px = total_px % tile_pitch;
    tiles[ptile_num]->py = total_py % tile_pitch;
}


/*** HitAnim ***/

/* Construct TimedEvent with effect duration as time.
 */
HitAnim::HitAnim(MapCoord *loc)
{
    hit_actor = NULL;
    add_tile(map_window->get_tile_manager()->get_tile(257), // U6 HIT_EFFECT
             0, 0);
    move(loc->x, loc->y);
}


HitAnim::HitAnim(Actor *actor)
{
    hit_actor = actor;
    add_tile(map_window->get_tile_manager()->get_tile(257), // U6 HIT_EFFECT
             0, 0);
    MapCoord loc = hit_actor->get_location();
    move(loc.x, loc.y);
}


/* Keep effect over actor.
 */
bool HitAnim::update()
{
    if(hit_actor)
    {
        MapCoord loc = hit_actor->get_location();
        move(loc.x, loc.y);
    }
    return(true);
}


/*** TossAnim ***/
TossAnim::TossAnim(Tile *tile, MapCoord *start, MapCoord *stop, uint8 stop_flags)
{
    toss_tile = tile;
    src = new MapCoord(*start);
    target = new MapCoord(*stop);
    blocking = stop_flags;
}


TossAnim::TossAnim(CallBack *t, void *d, Tile *tile, MapCoord *start, MapCoord *stop, uint8 stop_flags)
{
    set_target(t);
    set_user_data(d);

    toss_tile = tile;
    src = new MapCoord(*start);
    target = new MapCoord(*stop);
    blocking = stop_flags;
}


TossAnim::~TossAnim()
{
    delete src;
    delete target;
}


/* Start tile at source. Set velocity.
 */
void TossAnim::start()
{
    move(src->x, src->y);
    add_tile(toss_tile, 0, 0);

    sint32 velocity = 320;
    set_velocity((target->x == src->x) ? 0 : (target->x > src->x) ? velocity : -velocity,
                 (target->y == src->y) ? 0 : (target->y > src->y) ? velocity : -velocity);
}


/* Slide tile. Check map for interception. (stop if hit target)
 */
bool TossAnim::update()
{
    update_position();
    MapCoord new_loc = get_location();

    // update_position() can miss the exact target; don't go too far from src
    if((new_loc != *target) && (new_loc.distance(*src) >= src->distance(*target)))
    {
        move(target->x, target->y);
        hit_target();
    }
    return(true);
}


void TossAnim::hit_target()
{
    if(!running)
        return;
    stop();
    message(MESG_ANIM_DONE, NULL);
}

void TossAnim::hit_something(Obj *obj)
{
    if(!running)
        return;
    MapEntity obj_ent(obj);
    if(blocking & TOSS_TO_OBJECT)
        stop();
    message(MESG_ANIM_HIT, &obj_ent);
}

void TossAnim::hit_something(Actor *actor)
{
    if(!running)
        return;
    MapEntity actor_ent(actor);
    if(blocking & TOSS_TO_ACTOR)
        stop();
    message(MESG_ANIM_HIT, &actor_ent);
}

void TossAnim::hit_blocking(MapCoord obj_loc)
{
    if(!running)
        return;
    if(blocking & TOSS_TO_BLOCKING)
        stop();
    message(MESG_ANIM_HIT_WORLD, &obj_loc);
}


/* Update position (move per velocity) and "hit" certain locations in the path.
 * Possible things to hit are actors and objects. Possible locations to hit are
 * boundary tiles (blocking) and the target.
 */
void TossAnim::update_position()
{
    ActorManager *actor_manager = Game::get_game()->get_actor_manager();
    ObjManager *obj_manager = Game::get_game()->get_obj_manager();
    Map *map = Game::get_game()->get_game_map();
    sint32 deltax, deltay; // line plotting is from Map::lineTest()
    sint32 xinc1, xinc2;
    sint32 yinc1, yinc2;
    sint32 dinc1, dinc2;
    sint32 d, count;
    uint32 this_time = SDL_GetTicks();
    uint8 tile_center = anim_manager->get_tile_pitch() / 2; // 8

    if(this_time - last_move_time >= 100) // only move every 10th sec
    {
        sint32 vel_x_incr = vel_x / 10, vel_y_incr = vel_y / 10;
        // no movement, just stop and hit_target()
        if(vel_x_incr == 0 && vel_y_incr == 0)
        {
            hit_target();
            return;
        }
        deltax = abs(vel_x_incr); // line settings
        deltay = abs(vel_y_incr);
        if(deltax >= deltay)
        {
            d = (deltay << 1) - deltax;
            count = deltax + 1;
            dinc1 = deltay << 1;
            dinc2 = (deltay - deltax) << 1;
            xinc1 = 1;
            xinc2 = 1;
            yinc1 = 0;
            yinc2 = 1;
        }
        else
        {
            d = (deltax << 1) - deltay;
            count = deltay + 1;
            dinc1 = deltax << 1;
            dinc2 = (deltax - deltay) << 1;
            xinc1 = 0;
            xinc2 = 1;
            yinc1 = 1;
            yinc2 = 1;
        }
        if(vel_x_incr < 0) // moving left
        {
            xinc1 = -xinc1;
            xinc2 = -xinc2;
        }
        if(vel_y_incr < 0) // moving up
        {
            yinc1 = -yinc1;
            yinc2 = -yinc2;
        }

        // plotted direction of a line of pixels from starting location to
        //  the distance we can move in this time increment
        // (move one pixel at a time, check when hitting a new tile)
        MapCoord old_loc = get_location(); // record
        // if center of anim/tile is not at old_loc, change old_loc to fit
        if(px >= tile_center) // 8
            old_loc.x += 1;
        if(py >= tile_center)
            old_loc.y += 1;

        for(sint32 i = 0; i < count; i++)
        {
            // move
            if(d < 0)
            {
               d += dinc1;
               shift(xinc1, yinc1);
            }
            else
            {
               d += dinc2;
               shift(xinc2, yinc2);
            }
            MapCoord new_loc = get_location(); // record
            // if center of anim/tile is not at new_loc, change new_loc to fit
            if(px >= tile_center) // 8
                new_loc.x += 1;
            if(py >= tile_center)
                new_loc.y += 1;

            // test the current location
            // FIXME: check for hit edge of visible area (or left the mapwindow)
            if(new_loc != old_loc)
            {
                old_loc = new_loc;
                uint8 mapwindow_level;
                map_window->get_level(&mapwindow_level);

                // actor or object (can be one or the other)
                Actor *hitActor = actor_manager->get_actor(new_loc.x, new_loc.y, mapwindow_level);
                Obj *hitObj = obj_manager->get_obj(new_loc.x, new_loc.y, mapwindow_level);
                if(hitActor)
                {
                    hit_something(hitActor);
                }
                else if(hitObj)
                {
                    hit_something(hitObj);
                }

                // blocking tile/object
                if(map->is_boundary(new_loc.x, new_loc.y, mapwindow_level)
                   || (hitObj && !map->is_passable(new_loc.x, new_loc.y, mapwindow_level)))
                {
                    hit_blocking(MapCoord(new_loc.x, new_loc.y, mapwindow_level));
                }

                // target (mandatory stop)
                if(new_loc.x == target->x && new_loc.y == target->y && mapwindow_level == target->z)
                    hit_target();
                if(!running) // don't bother moving any more
                    return;
            }
        }

        last_move_time = this_time;
    }
}


/*** ExplosiveAnim ***/


ExplosiveAnim::ExplosiveAnim(MapCoord *start, uint32 size)
{
    exploding_tile_num = 393; // U6 FIREBALL_EFFECT

    center = *start;
    radius = size;
}


/* Delete the rotated tileset.
 */
ExplosiveAnim::~ExplosiveAnim()
{
// removed - this is deleted in TileAnim
//    for(uint32 i = 0; i < tiles.size(); i++)
//        delete tiles[i]->tile;
}


/* Set up how many fireballs shoot out and in what directions. Start them at
 * the center.
 */
void ExplosiveAnim::start()
{
    TileManager *tile_manager = map_window->get_tile_manager();
    sint32 s = 8; // how many pixels each new fireball is from the previous

    move(center.x, center.y);
    flame.resize(radius * 5);

    for(uint32 t = 0; t < flame.size(); t++)
    {
        flame[t].travelled = 0;
        flame[t].tile = add_tile(tile_manager->get_tile(exploding_tile_num), 0, 0);

        uint8 dir = (t < 8) ? t : NUVIE_RAND() % 8;
        switch(dir)
        {
            case 0:
                flame[t].direction = MapCoord(0, -s);
                break;
            case 1:
                flame[t].direction = MapCoord(s, -s);
                break;
            case 2:
                flame[t].direction = MapCoord(s, 0);
                break;
            case 3:
                flame[t].direction = MapCoord(s, s);
                break;
            case 4:
                flame[t].direction = MapCoord(0, s);
                break;
            case 5:
                flame[t].direction = MapCoord(-s, s);
                break;
            case 6:
                flame[t].direction = MapCoord(-s, 0);
                break;
            case 7:
                flame[t].direction = MapCoord(-s, -s);
        }
        if(t >= 8) // after the cardinals, randomly alter direction
        {
            flame[t].direction.sx += ((NUVIE_RAND() % 7) - 3);
            flame[t].direction.sy += ((NUVIE_RAND() % 7) - 3);
        }

        // rotate sprite
        if(!(flame[t].direction.sx == 0 && flame[t].direction.sy == 0))
        {
            Tile *rot_tile = NULL;
            rot_tile = tile_manager->get_rotated_tile(flame[t].tile->tile,
                                                      get_relative_degrees(flame[t].direction.sx, flame[t].direction.sy));
            flame[t].tile->tile = rot_tile;
        }
    }

    // start timer: larger explosions are faster
    uint32 delay;
    if(radius <= 3)
        delay = 15;
    else if(radius <= 5)
        delay = 5;
    else
        delay = 4;
    start_timer(delay);
}


/* On TIMED: Expand explosion. End when explosion is finished.
 */
uint16 ExplosiveAnim::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    bool animating = false; // true = still exploding
    uint32 flame_size = flame.size();

    if(msg != MESG_TIMED)
        return(0);

    for(uint32 t = 0; t < flame_size; t++) // update each line of fire
    {
        uint32 r = radius;
        if((r > 1) && flame[t].direction.sx && flame[t].direction.sy) // diagonal
            r -= 1;
        if(flame[t].travelled < r) // keep exploding; add another fireball
        {
            flame[t].tile = add_tile(flame[t].tile->tile,
                                     flame[t].tile->pos_x, flame[t].tile->pos_y,
                                     flame[t].tile->px, flame[t].tile->py);
            shift_tile(0, flame[t].direction.sx, flame[t].direction.sy);
            flame[t].travelled++; // flame travel distance from center
            animating = true;
        }
    }

    if(!animating) // all flames stopped moving, so we're done
    {
        message(MESG_ANIM_DONE); // FIXME: in the future make all Anims send when deleted
        stop();
    }
    return(0);
}


/* Check map for interception with any flames. Stop exploding in direction of
 * a blocked tile. Hit actors & volatile objects (powder kegs).
 */
bool ExplosiveAnim::update()
{
   // ActorManager *actor_manager = Game::get_game()->get_actor_manager();
   // ObjManager *obj_manager = Game::get_game()->get_obj_manager();
    Map *map = Game::get_game()->get_game_map();
    uint8 mapwindow_level;
    LineTestResult lt;

    map_window->get_level(&mapwindow_level);
    for(uint32 t = 0; t < flame.size(); t++) // update each line of fire
    {
        if(flame[t].direction.sx == 0 && flame[t].direction.sy == 0)
            continue; // not moving; check next flames
        // (absolute) location of last fireball (the one that matters)
        MapCoord edge(flame[t].tile->pos_x + center.x,
                      flame[t].tile->pos_y + center.y, mapwindow_level);
        uint16 edge_px = flame[t].tile->px; // pixel offset
        uint16 edge_py = flame[t].tile->py;
        // effective edge is location of the CENTER of this fireball/tile, if it
        // is moving down or right, but I'm not sure why direction affects it
        if(flame[t].direction.sx > 0 && edge_px >= 8) edge.x += 1;
        if(flame[t].direction.sy > 0 && edge_py >= 8) edge.y += 1;

        if(map->lineTest(center.x, center.y, edge.x, edge.y, mapwindow_level,
                         LT_HitActors, lt, 1)
           && !already_hit(MapEntity(lt.hitActor)))
            hit_something(lt.hitActor);
        else if(map->lineTest(center.x, center.y, edge.x, edge.y, mapwindow_level,
                              LT_HitObjects, lt, 1)
                && !already_hit(MapEntity(lt.hitObj)))
            hit_something(lt.hitObj);

        // stop in this direction
        // FIXME: more things than boundaries (walls) stop explosions
        if(map->is_boundary(edge.x, edge.y, mapwindow_level) && edge != center)
            flame[t].direction.sx = flame[t].direction.sy = 0;
//        get_shifted_location(edge.x, edge.y, edge_px, edge_py,
//                             flame[t].direction.sx, flame[t].direction.sy);
//        if(map->lineTest(center.x, center.y, edge.x, edge.y, mapwindow_level,
//                         LT_HitUnpassable, lt, 1))
//            flame[t].direction.sx = flame[t].direction.sy = 0;
    }

    return(true);
}


/* Returns true if the explosion has already the particular thing this MapEntity
 * points to. (and shouldn't hit it again)
 */
bool ExplosiveAnim::already_hit(MapEntity ent)
{
    for(uint32 e = 0; e < hit_items.size(); e++)
        if(hit_items[e].entity_type == ent.entity_type)
            if(hit_items[e].data == ent.data)
                return(true);
    return(false);
}


/* Also adds object to hit_items list for already_hit() to check.
 */
void ExplosiveAnim::hit_something(Obj *obj)
{
    if(!running)
        return;
    MapEntity obj_ent(obj);
    hit_items.push_back(obj_ent);
    message(MESG_ANIM_HIT, &obj_ent);
}


/* Also adds actor to hit_items list for already_hit() to check.
 */
void ExplosiveAnim::hit_something(Actor *actor)
{
    if(!running)
        return;
    MapEntity actor_ent(actor);
    hit_items.push_back(actor_ent);
    message(MESG_ANIM_HIT, &actor_ent);
}


/* Get the location something on the map would be at were it
 * shifted by sx,sy pixels from x,y+px,py.
 * FIXME: duplicated code
 */
void ExplosiveAnim::get_shifted_location(uint16 &x, uint16 &y, uint16 &px, uint16 &py, uint32 sx, uint32 sy)
{
    uint8 tile_pitch = anim_manager->get_tile_pitch();
    uint32 total_px = (x * tile_pitch) + px, total_py = (y * tile_pitch) + py;
    total_px += sx;
    total_py += sy;

    x = total_px / tile_pitch;
    y = total_py / tile_pitch;
    px = total_px % tile_pitch;
    px = total_py % tile_pitch;
}


/*** ThrowObjectAnim ***/
ThrowObjectAnim::ThrowObjectAnim(Obj *obj, MapCoord *from, MapCoord *to, uint32 vel)
                                 : TossAnim(NULL, from, to)
{
    direction.sx = target->x - src->x;
    direction.sy = target->y - src->y;

    toss_tile = Game::get_game()->get_obj_manager()->get_obj_tile(obj->obj_n, obj->frame_n);
    toss_tile = Game::get_game()->get_tile_manager()
                         ->get_rotated_tile(toss_tile,
                                            get_relative_degrees(direction.sx,
                                                                 direction.sy, 90));

    // set velocity (x,y) based on direction & speed
    set_velocity_for_speed(direction.sx, direction.sy, vel);
}


ThrowObjectAnim::~ThrowObjectAnim()
{
    delete toss_tile;
}


void ThrowObjectAnim::start()
{
    move(src->x, src->y);
    add_tile(toss_tile, 0, 0);
}
