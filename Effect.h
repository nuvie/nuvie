#ifndef __Effect_h__
#define __Effect_h__

#include "CallBack.h"
#include "ObjManager.h"

//class Actor;
class EffectManager;
class Game;
class MapCoord;
class MapWindow;
class NuvieAnim;
class TimedCallback;

/* Effects: * = unwritten or untested
 * Quake - earthquake from cyclops or volcanos
 * Hit - hit actor anim + sfx
 * Explosive - explosion caused by powder keg, volcanos, or cannonball hit
 * ThrowObject - any thrown object
 * Cannonball (FIX: change to UseCodeThrow)
 * Missile - throw object to *ground or actor; optionally cause damage
 * *Boomerang
 * Drop - throw obj from inventory to ground
 */


/* Control animation and sounds in the game world.
 */
class Effect : public CallBack
{
protected:
    Game *game;
    EffectManager *effect_manager;
    bool defunct;

public:
    Effect();
    ~Effect();
    void delete_self() { defunct = true; }
    void add_anim(NuvieAnim *anim);

    bool is_defunct()  { return(defunct); }
    uint16 callback(uint16, CallBack *, void *)
                       { return(0); }
};


/* Toss a cannon ball from one actor to another, or from an object towards
 * a numbered direction.
 */
class CannonballEffect : public Effect
{
    UseCode *usecode;
    NuvieAnim *anim;
//  *sfx;
    Obj *obj;
    MapCoord target_loc; // where cannonball will hit

    void start_anim();

public:
    CannonballEffect(Obj *src_obj, sint8 direction = -1);
//    CannonballEffect(Actor *src_actor, Actor *target_actor); from a ship

    uint16 callback(uint16 msg, CallBack *caller, void *data);

};


/* Use to add an effect with timed activity. Self-contained timer must be
 * stopped/started with the included methods.
 */
class TimedEffect : public Effect
{
protected:
    TimedCallback *timer;
public:
    TimedEffect()                  { timer = NULL; }
    ~TimedEffect()                 { stop_timer(); }

    void start_timer(uint32 delay) { if(!timer) timer = new TimedCallback(this, NULL, delay, true); }
    void stop_timer()              { if(timer) { timer->clear_target(); timer = NULL; } }

    void delete_self() { stop_timer(); Effect::delete_self(); }

    virtual uint16 callback(uint16 msg, CallBack *caller, void *data) = 0;
};


/* Shake the visible play area around.
 */
class QuakeEffect : public TimedEffect
{
    MapWindow *map_window;
    static QuakeEffect *current_quake; // do nothing if already active
    sint32 sx, sy; // last map_window movement amount
    MapCoord orig; // map_window location at start
    Actor *orig_actor; // center map_window on actor
    uint32 stop_time;
    uint8 strength; // magnitude

public:
    QuakeEffect(uint8 magnitude, uint32 duration, Actor *keep_on = NULL);
    ~QuakeEffect();
    uint16 callback(uint16 msg, CallBack *caller, void *data);

    void init_directions();
    void recenter_map();
    void stop_quake();
};


/* Hit target actor.
 */
class HitEffect : public Effect
{
public:
    HitEffect(Actor *target, uint32 duration = 300);
    uint16 callback(uint16 msg, CallBack *caller, void *data);
};


/* Create explosion animation and sounds from the source location out to
 * specified radius. Hit actors and objects for `dmg'.
 */
class ExplosiveEffect : public Effect
{
protected:
    NuvieAnim *anim;
//  *sfx;
    MapCoord start_at;
    uint32 radius;
    uint16 hit_damage; // hp taken off actors hit by explosion

    void start_anim();

public:
    ExplosiveEffect(uint16 x, uint16 y, uint32 size, uint16 dmg = 0);
    uint16 callback(uint16 msg, CallBack *caller, void *data);

    // children can override
    virtual void delete_self() { Effect::delete_self(); }
    virtual bool hit_object(Obj *obj) { return(false); } // explosion hit something
                                                         // true return=end effect
};


/* Explosion that sends usecode event to an object on completion.
 */
class UseCodeExplosiveEffect : public ExplosiveEffect
{
     Obj *obj; // explosion came from this object (can be NULL)
     Obj *original_obj; // don't hit this object (chain-reaction avoidance hack)
     
 public:
    UseCodeExplosiveEffect(Obj *src_obj, uint16 x, uint16 y, uint32 size, uint16 dmg = 0, Obj *dont_hit_me = NULL)
                          : ExplosiveEffect(x, y, size, dmg), obj(src_obj), original_obj(dont_hit_me)
    {
    }
    void delete_self();
    bool hit_object(Obj *hit_obj); // explosion hit something


     
};



/* Toss object tile from one location to another. The sprite is rotated towards
 * the toss direction. This utilizes a ThrowObjectAnim.
 */
class ThrowObjectEffect : public Effect
{
protected:
    ObjManager *obj_manager;
    NuvieAnim *anim; // TossAnim
//  *sfx;
    MapCoord start_at, stop_at; // start_at -> stop_at
    Obj *throw_obj; // object being thrown
    uint32 throw_speed; // used in animation

public:
    ThrowObjectEffect();

    void hit_target(); // stops effect
    void start_anim();

    virtual uint16 callback(uint16 msg, CallBack *caller, void *data) = 0;
};


/* Drop an object from an actor's inventory. Object is removed from the actor
 * after starting the effect, and added to the map when the effect is complete.
 * Effect speed is gametype-defined.
 */
class DropEffect : public ThrowObjectEffect
{
    Actor *drop_from_actor;
public:
    DropEffect(Obj *obj, uint16 qty, Actor *actor, MapCoord *drop_loc);

    void hit_target();

    void get_obj(Obj *obj, uint16 qty);
    uint16 callback(uint16 msg, CallBack *caller, void *data);
};



/* Throw an object towards an actor or location. If the target is an actor, it
 * will be hit for the requested damage. Attacking objects is not handled here.
 * If the target is an empty map location, the object will be added to the map.
 * (this isn't implemented yet, try a DropEffect)
 *
 * Decide in the attack logic, before constructing this, whether or not it was
 * successful, and use the appropriate constructor.
 */
class MissileEffect : public ThrowObjectEffect
{
    uint16 hit_damage; // hp taken off actor(s) hit by object
    Actor *hit_actor; // if targeting an actor

public:
    MissileEffect(Obj *obj, MapCoord *source, Actor *target, uint32 dmg, uint32 spd = 0);
    MissileEffect(Obj *obj, MapCoord *source, MapCoord *target, uint32 dmg, uint32 spd = 0);

    void init(Obj *obj, MapCoord *src, MapCoord *target, Actor *a, uint32 dmg, uint32 spd);

    uint16 callback(uint16 msg, CallBack *caller, void *data);
};



#if 0
/* Throw an object and bring it back.
 */
class BoomerangEffect : public ThrowObjectEffect
{
};


/* Cycle or modify the game palette in some way.
 */
class PaletteEffect : public TimedEffect
{
// palette effects are created from child classes (new BlackPotionEffect();)
// ...and these can include SFX like any other effect
// but PaletteEffect is not abstract (new PaletteEffect(timing & color params...);)
};


#endif


#endif /* __Effect_h__ */
