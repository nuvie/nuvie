#include "nuvieDefs.h"
#include "Game.h"

#include "Actor.h"
#include "Map.h"
#include "AnimManager.h"
#include "MapWindow.h"
#include "TileManager.h"
#include "GameClock.h"
#include "EffectManager.h"
#include "UseCode.h"
#include "Effect.h"

#define MESG_ANIM_HIT_WORLD  ANIM_CB_HIT_WORLD
#define MESG_ANIM_HIT        ANIM_CB_HIT
#define MESG_ANIM_DONE       ANIM_CB_DONE
#define MESG_EFFECT_COMPLETE EFFECT_CB_COMPLETE

QuakeEffect *QuakeEffect::current_quake = NULL;


/* Add self to effect list (for future deletion).
 */
Effect::Effect() : defunct(false)
{
    game = Game::get_game();
    effect_manager = game->get_effect_manager();
    effect_manager->add_effect(this);
}


Effect::~Effect()
{
    // FIXME: should we remove self from Callbacks' default targets?
}


/* Start managing new animation. (AnimMgr will do that actually, but we point to
 * it and can stop it when necessary)
 */
void Effect::add_anim(NuvieAnim *anim)
{
    anim->set_target(this); // add self as callback target for anim
    game->get_map_window()->get_anim_manager()->new_anim(anim);
}


/* Fire from a cannon in direction: 0=north, 1=east, 2=south, 3=west,
 *                                 -1=use cannon frame
 */
CannonballEffect::CannonballEffect(Obj *src_obj, sint8 direction)
                                   : target_loc()
{
    usecode = game->get_usecode();
    obj = src_obj;
    MapCoord obj_loc(obj->x, obj->y, obj->z);
    target_loc = obj_loc;

    if(direction == -1)
        direction = obj->frame_n;

    uint8 target_dist = 5; // distance that cannonball will fly
    if(direction == 0)
        target_loc.y -= target_dist;
    else if(direction == 1)
        target_loc.x += target_dist;
    else if(direction == 2)
        target_loc.y += target_dist;
    else if(direction == 3)
        target_loc.x -= target_dist;

    start_anim();
}


/* Pause world & start animation.
 */
void CannonballEffect::start_anim()
{
    MapCoord obj_loc(obj->x, obj->y, obj->z);

    game->pause_world();
    game->pause_user();

    anim = new TossAnim(game->get_tile_manager()->get_tile(399),
                        &obj_loc, &target_loc);
    add_anim(anim);
}


/* Handle messages from animation. Hit actors & walls.
 */
uint16 CannonballEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    bool stop_effect = false;
    switch(msg)
    {
        case MESG_ANIM_HIT_WORLD:
        {
            MapCoord *hit_loc = static_cast<MapCoord *>(msg_data);
            uint16 tile_num = game->get_game_map()->get_tile(hit_loc->x,hit_loc->y,hit_loc->z)->tile_num;
            // main U6 wall tiles FIX for WOU games
            if(tile_num >= 140 && tile_num <= 187)
            {
                new ExplosiveEffect(hit_loc->x, hit_loc->y, 2);
                stop_effect = true;
            }
            break;
        }
        case MESG_ANIM_HIT:
        {
            MapEntity *hit_ent = static_cast<MapEntity *>(msg_data);
            if(hit_ent->entity_type == ENT_ACTOR)
            {
                hit_ent->actor->hit(32);
                stop_effect = true;
            }
            if(hit_ent->entity_type == ENT_OBJ)
            {
                printf("hit object %d at %x,%x,%x\n", hit_ent->obj->obj_n, hit_ent->obj->x, hit_ent->obj->y, hit_ent->obj->z);
            }
            break;
        }
        case MESG_ANIM_DONE:
            new ExplosiveEffect(target_loc.x, target_loc.y, 3);
            stop_effect = true;
            break;
    }

    if(stop_effect)
    {
        if(msg != MESG_ANIM_DONE)
            anim->stop();
        game->unpause_all();
        usecode->message_obj(obj, MESG_EFFECT_COMPLETE, this);
        delete_self();
    }
    return(0);
}


/* Shake the visible play area around for `duration' milliseconds. Magnitude
 * determines the speed of movement. An actor may be selected to keep the
 * MapWindow centered on after the Quake.
 */
QuakeEffect::QuakeEffect(uint8 magnitude, uint32 duration, Actor *keep_on)
{
    // single use only, so MapWindow doesn't keep moving away from center
    // ...and do nothing if magnitude isn't usable
    if(current_quake || magnitude == 0)
    {
        delete_self();
        return;
    }
    current_quake = this; // cleared in timer function

    map_window = game->get_map_window();
    stop_time = game->get_clock()->get_ticks() + duration;
    strength = magnitude;

    // get random direction (always move left-right more than up-down)
    init_directions();

    map_window->get_pos(&orig.x, &orig.y);
    map_window->get_level(&orig.z);
    orig_actor = keep_on;

    start_timer(strength * 5);
}


QuakeEffect::~QuakeEffect()
{
}


/* On TIMED: Move map.
 */
uint16 QuakeEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    uint8 twice_strength = strength * 2;
    if(msg != MESG_TIMED)
        return(0);
    if(game->get_clock()->get_ticks() >= stop_time)
    {
        stop_quake();
        return(0);
    }
    recenter_map();
    map_window->shiftMapRelative(sx, sy);

    // move in opposite direction on next call
    if(sx == -(4*strength) || sx == (4*strength))
        sx = (sx == -(4*strength)) ? (2*strength) : -(2*strength);
    else if(sx == -(2*strength) || sx == (2*strength))
        sx = 0;

    if(sy == -(2*strength) || sy == (2*strength))
        sy = 0;

    if(sx == 0 && sy == 0)
        init_directions();
    return(0);
}


/* Finish effect. Move map back to initial position.
 */
void QuakeEffect::stop_quake()
{
    current_quake = NULL;
    recenter_map();
    delete_self();
}


/* Set sx,sy to a random direction. (always move left-right more than up-down)
 */
void QuakeEffect::init_directions()
{
    uint8 dir = NUVIE_RAND() % 8;
    sx = 0; sy = 0;
    if(dir == 0) // up
    {
        sy = -(strength*2);
    }
    else if(dir == 1) // up-right
    {
        sx = (strength*4); sy = -(strength*2);
    }
    else if(dir == 2) // right
    {
        sx = (strength*4);
    }
    else if(dir == 3) // down-right
    {
        sx = (strength*4); sy = (strength*2);
    }
    else if(dir == 4) // down
    {
        sy = (strength*2);
    }
    else if(dir == 5) // down-left
    {
        sx = -(strength*4); sy = (strength*2);
    }
    else if(dir == 6) // left
    {
        sx = -(strength*4);
    }
    else if(dir == 7) // up-left
    {
        sx = -(strength*4); sy = -(strength*2);
    }
}


/* Center map on original actor or move to original location.
 */
void QuakeEffect::recenter_map()
{
    if(orig_actor)
        map_window->centerMapOnActor(orig_actor);
    else
        map_window->moveMap(orig.x, orig.y, orig.z);
}


/*** HitEffect ***/
/* Hit target actor. FIXME: implement duration and hitting a location
 */
HitEffect::HitEffect(Actor *target, uint32 duration)
{
    add_anim(new HitAnim(target));
}


/* On ANIM_DONE: end
 */
uint16 HitEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    if(msg == MESG_ANIM_DONE)
        delete_self();
    return(0);
}


/*** ExplosiveEffect ***/
ExplosiveEffect::ExplosiveEffect(uint16 x, uint16 y, uint32 size, uint16 dmg)
                                 : start_at()
{
    start_at.x = x; start_at.y = y;
    radius = size;
    hit_damage = dmg;

    start_anim();
}


/* Pause world & start animation.
 */
void ExplosiveEffect::start_anim()
{
    game->pause_world();
    game->pause_user();
    add_anim(new ExplosiveAnim(&start_at, radius));
}


/* Handle messages from animation. Hit actors & objects.
 */
uint16 ExplosiveEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    bool stop_effect = false;
    switch(msg)
    {
        case MESG_ANIM_HIT:
        {
            MapEntity *hit_ent = static_cast<MapEntity *>(msg_data);
            if(hit_ent->entity_type == ENT_ACTOR)
            {
                if(hit_damage != 0) // hit actor if effect causes damage
                    hit_ent->actor->hit(hit_damage);
            }
            else if(hit_ent->entity_type == ENT_OBJ)
            {
                printf("Explosion hit object %d (%x,%x)\n", hit_ent->obj->obj_n, hit_ent->obj->x, hit_ent->obj->y);
                stop_effect = hit_object(hit_ent->obj);
            }
            break;
        }
        case MESG_ANIM_DONE:
            stop_effect = true;
            break;
    }

    if(stop_effect)
    {
        if(msg != MESG_ANIM_DONE)
            anim->stop();
        game->unpause_all();
        delete_self();
    }
    return(0);
}


/* UseCodeExplosiveEffect: before deleting send message to source object
 */
void UseCodeExplosiveEffect::delete_self()
{
    if(obj)
        game->get_usecode()->message_obj(obj, MESG_EFFECT_COMPLETE, this);
    Effect::delete_self();
}


/* The explosion hit an object.
 * Returns true if the effect should end, false to continue.
 */
bool UseCodeExplosiveEffect::hit_object(Obj *hit_obj)
{
    // ignite & destroy powder kegs (U6)
    if(hit_obj->obj_n == 223 && hit_obj != original_obj)
    {
        // FIXME: this doesn't belong here (U6/obj specific)
        uint16 x = hit_obj->x, y = hit_obj->y;
        game->get_obj_manager()->remove_obj(hit_obj);
        game->get_obj_manager()->delete_obj(hit_obj);
        if(obj) // pass our source obj on to next effect as original_obj
            new UseCodeExplosiveEffect(NULL, x, y, 2, hit_damage, obj);
        else // pass original_obj on to next effect
            new UseCodeExplosiveEffect(NULL, x, y, 2, hit_damage, original_obj);
    }
    return(false);
}


/*** ThrowObjectEffect ***/
ThrowObjectEffect::ThrowObjectEffect()
{
    obj_manager = game->get_obj_manager();

    anim = NULL;
    throw_obj = NULL;
    throw_speed = 0;
}


void ThrowObjectEffect::start_anim()
{
    game->pause_world();
    game->pause_user();
    anim = new ThrowObjectAnim(throw_obj, &start_at, &stop_at, throw_speed);
    add_anim(anim);
}


/* Object has stopped.
 */
void ThrowObjectEffect::hit_target()
{
    game->unpause_all();
    delete_self();
}


/* The animation will travel from original object location to drop location if
 * NULL actor is specified.
 */
DropEffect::DropEffect(Obj *obj, uint16 qty, Actor *actor, MapCoord *drop_loc)
{
    drop_from_actor = actor;
    start_at = drop_from_actor ? drop_from_actor->get_location() : MapCoord(obj->x, obj->y, obj->z);
    stop_at = *drop_loc;

    get_obj(obj, qty); // remove from actor, set throw_obj

    if(start_at != stop_at)
    {
        throw_speed = 296; // animation speed
        start_anim();
    }
    else
        hit_target(); // done already? why bother calling DropEffect? :p
}


/* Take `qty' objects of a stack if necessary, and remove from the actor's
 * inventory. Set `throw_obj'.
 */
void DropEffect::get_obj(Obj *obj, uint16 qty)
{
    throw_obj = obj_manager->get_obj_from_stack(obj, qty);
    if(drop_from_actor)
        drop_from_actor->inventory_remove_obj(throw_obj);
}


/* On ANIM_HIT_WORLD: end at hit location
 * On ANIM_DONE: end
 */
uint16 DropEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    // if throw_obj is NULL, object already hit target
    if(!throw_obj || (msg != MESG_ANIM_DONE && msg != MESG_ANIM_HIT_WORLD))
        return(0);

    if(msg == MESG_ANIM_HIT_WORLD && stop_at == *(MapCoord *)msg_data)
        anim->stop();

    hit_target();
    return(0);
}


/* Add object to map. (call before completing effect)
 */
void DropEffect::hit_target()
{
    throw_obj->x = stop_at.x;
    throw_obj->y = stop_at.y;
    throw_obj->z = stop_at.z;
    obj_manager->add_obj(throw_obj);
    throw_obj = NULL; // set as dropped

    ThrowObjectEffect::hit_target(); // calls delete_self()
}


/*** MissileEffect ***/
/* Using an actor as the target.
 */
MissileEffect::MissileEffect(Obj *obj, MapCoord *source, Actor *target,
                             uint32 dmg, uint32 spd)
{
    init(obj, source, NULL, target, dmg, spd);
}


MissileEffect::MissileEffect(Obj *obj, MapCoord *source, MapCoord *target,
                             uint32 dmg, uint32 spd)
{
    init(obj, source, target, NULL, dmg, spd);
}


/* Start effect. If target is unset then the actor is the target.
 */
void MissileEffect::init(Obj *obj, MapCoord *src, MapCoord *target, Actor *a,
                         uint32 dmg, uint32 spd)
{
    assert(obj && src);
    throw_obj = obj;
    hit_actor = a;
    throw_speed = spd;
    hit_damage = dmg;

    start_at = *src;
    stop_at = target ? *target : a->get_location();

    start_anim();
}


/* On HIT: hit Actor and end
 */
uint16 MissileEffect::callback(uint16 msg, CallBack *caller, void *msg_data)
{
    if(msg == MESG_ANIM_HIT && ((MapEntity *)msg_data)->entity_type == ENT_ACTOR)
    {
        if(hit_actor && hit_damage != 0) // stop at actor if effect causes damage
        {
            anim->stop();
            ((MapEntity *)msg_data)->actor->hit(hit_damage);
            hit_target();
        }
    }
    else if(msg == MESG_ANIM_DONE)
        hit_target();
    return(0);
}
