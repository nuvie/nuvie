#include <cstring>
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
#include "ViewManager.h"
#include "MsgScroll.h"
#include "Effect.h"

#include <cassert>

#define MESG_ANIM_HIT_WORLD  ANIM_CB_HIT_WORLD
#define MESG_ANIM_HIT        ANIM_CB_HIT
#define MESG_ANIM_DONE       ANIM_CB_DONE
#define MESG_EFFECT_COMPLETE EFFECT_CB_COMPLETE

#define TRANSPARENT_COLOR 0xFF /* transparent pixel color */

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
    game->pause_anims();
    game->pause_user();

    anim = new TossAnim(game->get_tile_manager()->get_tile(399),
                        &obj_loc, &target_loc, TOSS_TO_BLOCKING | TOSS_TO_ACTOR | TOSS_TO_OBJECT);
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
            // main U6 wall tiles FIX for WOU games (and this is probably a tileflag)
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
        if(msg != MESG_ANIM_DONE) // this msg means anim stopped itself
            anim->stop();
        game->unpause_all();
        usecode->message_obj(obj, MESG_EFFECT_COMPLETE, this);
        delete_self();
    }
    return(0);
}


/*** TimedEffect ***/
void TimedEffect::start_timer(uint32 delay)
{
    if(!timer)
        timer = new TimedCallback(this, NULL, delay, true);
}


void TimedEffect::stop_timer()
{
    if(timer)
    {
        timer->clear_target();
        timer = NULL;
    }
}


/*** QuakeEffect ***/
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
  //  uint8 twice_strength = strength * 2;
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
        delete_obj(hit_obj);
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
    game->pause_anims();
    game->pause_world();
    game->pause_user();
    anim = new ThrowObjectAnim(throw_obj, &start_at, &stop_at, throw_speed);
    add_anim(anim);
}


/* Object has stopped.
 */
void ThrowObjectEffect::hit_target()
{
    if(anim)
        anim->stop();
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
        throw_speed = 192; // animation speed
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

    if(msg == MESG_ANIM_HIT_WORLD && stop_at == *(MapCoord *)msg_data
       && anim)
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
    obj_manager->add_obj(throw_obj, OBJ_ADD_TOP);
    throw_obj = NULL; // set as dropped

    // not appropriate to do "Event::endAction(true)" from here to display
    // prompt, as we MUST unpause_user() in ThrowObjectEffect::hit_target, and
    // that would be redundant and may not unpause everything if wait mode was
    // already cancelled... so just prompt
    game->get_scroll()->display_string("\n");
    game->get_scroll()->display_prompt();
 
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
            if(anim)
                anim->stop();
            ((MapEntity *)msg_data)->actor->hit(hit_damage);
            hit_target();
        }
    }
    else if(msg == MESG_ANIM_DONE)
        hit_target();
    return(0);
}


/*** SleepEffect ***/
/* The TimedAdvance is started after the fade-out completes. */
SleepEffect::SleepEffect(std::string until)
                         : timer(NULL),
                           stop_hour(0),
                           stop_minute(0),
                           stop_time("")
{
    stop_time = until;
    game->pause_user();
    effect_manager->watch_effect(this, new FadeEffect(FADE_PIXELATED, FADE_OUT));
}


SleepEffect::SleepEffect(uint8 to_hour)
                       : timer(NULL),
                         stop_hour(to_hour),
                         stop_minute(0),
                         stop_time("")
{
    game->pause_user();
    effect_manager->watch_effect(this, new FadeEffect(FADE_PIXELATED, FADE_OUT));
}


SleepEffect::~SleepEffect()
{
    if(timer) // make sure it doesn't try to call us again
        timer->clear_target();
}


/* As with TimedEffect, make sure the timer doesn't try to use callback again.
 */
void SleepEffect::delete_self()
{
    timer->clear_target(); // this will also stop/delete the TimedAdvance
    timer = NULL;
    Effect::delete_self();
}


/* Resume normal play when requested time has been reached.
 */
//FIXME: need to handle TimedAdvance() errors and fade-in
uint16 SleepEffect::callback(uint16 msg, CallBack *caller, void *data)
{
    uint8 hour = Game::get_game()->get_clock()->get_hour();
    uint8 minute = Game::get_game()->get_clock()->get_minute();

    // waited for FadeEffect
    if(msg == MESG_EFFECT_COMPLETE)
    {
        if(timer == NULL) // starting
        {
            if(stop_time != "") // advance to start time
            {
                timer = new TimedAdvance(stop_time, 360); // 6 hours per second FIXME: it isnt going anywhere near that fast
                timer->set_target(this);
                timer->get_time_from_string(stop_hour, stop_minute, stop_time);
                // stop_hour & stop_minute are checked each hour
            }
            else // advance a number of hours
            {
                uint16 advance_h = (hour == stop_hour) ? 24
                                 : (hour < stop_hour) ? (stop_hour-hour)
                                 : (24-(hour-stop_hour));
                timer = new TimedAdvance(advance_h, 360);
                timer->set_target(this);
                stop_minute = minute;
            }
        }
        else // stopping
        {
            game->unpause_all();
            delete_self();
        }
        return(0);
    }
    // assume msg == MESG_TIMED; will stop after effect completes
    if(hour == stop_hour && minute >= stop_minute)
        effect_manager->watch_effect(this, new FadeEffect(FADE_PIXELATED, FADE_IN));

    return(0);
}


/*** FadeEffect ***/
FadeEffect::FadeEffect(FadeType fade, FadeDirection dir, uint32 color, uint32 speed)
{
    speed = speed ? speed : 256000;
    init(fade, dir, color, NULL, speed);
}


/* Takes an image to fade from/to. */
FadeEffect::FadeEffect(FadeType fade, FadeDirection dir, SDL_Surface *capture, uint32 speed)
{
    speed = speed ? speed : 196000;
    init(fade, dir, 0, capture, speed); // color=black
}


void FadeEffect::init(FadeType fade, FadeDirection dir, uint32 color, SDL_Surface *capture, uint32 speed)
{
    screen = game->get_screen();
    map_window = game->get_map_window();
    viewport = new SDL_Rect(map_window->GetRect());

    fade_type = fade;
    fade_dir = dir;
    fade_speed = speed; // pixels-per-second (to check, not draw)

    evtime = prev_evtime = 0;
    fade_x = fade_y = 0;
    fade_from = NULL;
    if(capture)
        fade_from = SDL_CreateRGBSurfaceFrom(capture->pixels, capture->w, capture->h,
                                             capture->format->BitsPerPixel, capture->pitch, 0, 0, 0, 0);

    if(fade_type == FADE_PIXELATED || fade_type == FADE_PIXELATED_ONTOP)
    {
        pixelated_color = color;
        init_pixelated_fade();
    }
    else
        init_circle_fade();
}


FadeEffect::~FadeEffect()
{
    delete viewport;
    if(fade_dir == FADE_IN) // overlay should be empty now, so just delete it
        map_window->set_overlay(NULL);
    if(fade_from)
        SDL_FreeSurface(fade_from);
}


/* Start effect.
 */
void FadeEffect::init_pixelated_fade()
{
    int fillret = -1; // check error
    overlay = map_window->get_overlay();
    if(overlay != NULL)
    {
        pixel_count = fade_from ? fade_from->w * fade_from->h
                                : overlay->w * overlay->h;
        // clear overlay to fill color or transparent
        if(fade_dir == FADE_OUT)
        {
            if(fade_from) // fade from captured surface to transparent
            {
                SDL_Rect fade_from_rect = { 0, 0, fade_from->w, fade_from->h };
                SDL_Rect overlay_rect = { fade_x, fade_y, fade_from->w, fade_from->h };
                fillret = SDL_BlitSurface(fade_from, &fade_from_rect,
                                          overlay, &overlay_rect);
            }
            else // fade from transparent to color
                fillret = SDL_FillRect(overlay, NULL, uint32(TRANSPARENT_COLOR));
        }
        else
        {
            if(fade_from) // fade from transparent to captured surface
                fillret = SDL_FillRect(overlay, NULL, uint32(TRANSPARENT_COLOR));
            else // fade from color to transparent
                fillret = SDL_FillRect(overlay, NULL, uint32(pixelated_color));
        }
    }
    if(fillret == -1)
    {
        fprintf(stderr, "FadeEffect: error creating overlay surface\n");
        delete_self();
        return;
    }

    // if FADE_PIXELATED_ONTOP is set, place the effect layer above the map border
    map_window->set_overlay_level((fade_type == FADE_PIXELATED)
                                  ? MAP_OVERLAY_DEFAULT : MAP_OVERLAY_ONTOP);
    colored_total = 0;
    start_timer(1); // fire timer continuously
}


/* Start effect.
 */
void FadeEffect::init_circle_fade()
{
    delete_self(); // FIXME
}


/* Called by the timer as frequently as possible. Do the appropriate
 * fade method and stop when the effect is oomplete.
 */
uint16 FadeEffect::callback(uint16 msg, CallBack *caller, void *data)
{
    bool fade_complete = false;

    // warning: msg is assumed to be CB_TIMED and data is set
    evtime = *(uint32 *)(data);

    // do effect
    if(fade_type == FADE_PIXELATED || fade_type == FADE_PIXELATED_ONTOP)
        fade_complete = (fade_dir == FADE_OUT) ? pixelated_fade_out() : pixelated_fade_in();
    else /* CIRCLE */
        fade_complete = (fade_dir == FADE_OUT) ? circle_fade_out() : circle_fade_in();

    // done
    if(fade_complete == true)
    {
        delete_self();
        return(1);
    }
    return(0);
}


/* Scan the overlay, starting at pixel rnum, for a transparent pixel if fading
 * out, and a colored pixel if fading in.
 * Returns true if a free pixel was found and set as rnum.
 */
inline bool FadeEffect::find_free_pixel(uint32 &rnum, uint32 pixel_count)
{
    uint8 scan_color = (fade_dir == FADE_OUT) ? TRANSPARENT_COLOR
                                              : pixelated_color;
    uint8 *pixels = (uint8 *)(overlay->pixels);

    for(uint32 p = rnum; p < pixel_count; p++) // check all pixels after rnum
        if(pixels[p] == scan_color)
        {
            rnum = p;
            return(true);
        }
    for(uint32 q = 0; q < rnum; q++) // check all pixels before rnum
        if(pixels[q] == scan_color)
        {
            rnum = q;
            return(true);
        }
    return(false);
}


/* Returns the next pixel to check/colorize. */
#if 0
#warning this crashes if x,y is near boundary
#warning make sure center_thresh doesnt go over boundary
inline uint32 FadeEffect::get_random_pixel(uint16 center_thresh)
{
    if(center_x == -1 || center_y == -1)
        return(NUVIE_RAND()%pixel_count);

    uint16 x = center_x, y = center_y;
    if(center_thresh == 0)
        center_thresh = overlay->w / 2;
    x += (NUVIE_RAND()%(center_thresh * 2)) - center_thresh,
    y += (NUVIE_RAND()%(center_thresh * 2)) - center_thresh;
    return((y * overlay->w) + x);
}
#endif

/* Randomly add pixels of the appropriate color to the overlay. If the color
 * is -1, it will be taken from the "fade_from" surface.
 * Returns true when the overlay is completely colored.
 */
bool FadeEffect::pixelated_fade_core(uint32 pixels_to_check, sint16 fade_to)
{
    uint8 *pixels = (uint8 *)(overlay->pixels);
    uint8 *from_pixels = fade_from ? (uint8 *)(fade_from->pixels) : NULL;
    uint32 p = 0; // scan counter
    uint32 rnum = 0; // pixel index
    uint32 colored = 0; // number of pixels that get colored
    uint16 fade_width = fade_from ? fade_from->w : overlay->w;
    uint16 fade_start = fade_from ? fade_y*overlay->w + fade_x : 0;

    while(p < pixels_to_check)
    {
        rnum = NUVIE_RAND()%pixel_count + fade_start;
        if(fade_to == -1) // get color from "fade_from"
            fade_to == from_pixels[rnum - fade_start];
        if(pixels[rnum] != fade_to)
        {
            pixels[rnum] = fade_to;
            ++colored, ++colored_total; // another pixel was set
        }
        ++p;
    }
    // all but two lines colored
    if(colored_total >= (pixel_count - fade_width*2)) // fill the rest
    {
        if(fade_to >= 0)
            SDL_FillRect(overlay, NULL, (uint32)fade_to);
        else
        {
            SDL_Rect fade_from_rect = { 0, 0, fade_from->w, fade_from->h };
            SDL_Rect overlay_rect = { fade_x, fade_y, fade_from->w, fade_from->h };
            SDL_BlitSurface(fade_from, &fade_from_rect, overlay, &overlay_rect);
        }
        return(true);
    }
    else return(false);
}


/* Color some of the mapwindow.
 * Returns true when all pixels have been filled, and nothing is visible.
 */
bool FadeEffect::pixelated_fade_out()
{
    if(fade_from)
        return(pixelated_fade_core(pixels_to_check(), TRANSPARENT_COLOR));
    return(pixelated_fade_core(pixels_to_check(), pixelated_color));
}


/* Clear some of the mapwindow.
 * Returns true when all colored pixels have been removed, and the MapWindow
 * is visible.
 */
bool FadeEffect::pixelated_fade_in()
{
    if(fade_from)
        return(pixelated_fade_core(pixels_to_check(), -1));
    return(pixelated_fade_core(pixels_to_check(), TRANSPARENT_COLOR));
}


/* Returns the number of pixels that should be checked/colored (based on speed)
 * since the previous call.
 */
uint32 FadeEffect::pixels_to_check()
{
    uint32 time_passed = (prev_evtime == 0) ? 0 : evtime - prev_evtime;
    uint32 fraction = 1000 / (time_passed > 0 ? time_passed : 1); // % of second passed, in milliseconds
    uint32 pixels_per_fraction = fade_speed / (fraction > 0 ? fraction : 1);
    prev_evtime = evtime;
    return(pixels_per_fraction);
}


/* Reduce the MapWindow's ambient light level, according to the set speed.
 * Returns true when nothing is visible.
 */
bool FadeEffect::circle_fade_out()
{
// FIXME
    return(false);
}


/* Add to the MapWindow's ambient light level, according to the set speed.
 * Returns true when the light level has returned to normal.
 */
bool FadeEffect::circle_fade_in()
{
// FIXME
    return(false);
}


/* Pause game and do FadeEffect.
 */
GameFadeInEffect::GameFadeInEffect(uint32 color)
                              : FadeEffect(FADE_PIXELATED_ONTOP, FADE_IN, color)
{
    game->pause_user();
}


GameFadeInEffect::~GameFadeInEffect()
{
}


/* Identical to FadeEffect, but unpause game when finished.
 */
uint16 GameFadeInEffect::callback(uint16 msg, CallBack *caller, void *data)
{
    // done
    if(FadeEffect::callback(msg, caller, data) != 0)
        game->unpause_user();
    return(0);
}
