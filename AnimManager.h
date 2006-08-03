#ifndef __AnimManager_h__
#define __AnimManager_h__

#include <list>
#include "SDL.h"
#include "nuvieDefs.h"
#include "MapWindow.h"
#include "TimedEvent.h"
#include "CallBack.h"
#include "MapEntity.h"

using std::list;
using std::string;
using std::vector;

class Actor;
class CallBack;
class AnimManager;
class NuvieAnim;
class Screen;

#define MESG_TIMED CB_TIMED

typedef std::list<NuvieAnim *>::iterator AnimIterator;

/* Each viewable area has it's own AnimManager. (but I can only think of
 * animations in the MapWindow using this, so that could very well change)
 */
class AnimManager
{
    MapWindow *map_window;
    Screen *viewsurf;
    SDL_Rect viewport; // clip anims to location
    std::list<NuvieAnim *> anim_list; // in paint order
    uint32 next_id;

    uint8 tile_pitch;

    AnimIterator get_anim_iterator(uint32 anim_id);

public:
    AnimManager(Screen *screen = NULL, SDL_Rect *clipto = NULL);
    ~AnimManager() { destroy_all(); }

    void update();
    void display();

    Screen *get_surface()            { return(viewsurf); }
    void set_surface(Screen *screen) { viewsurf = screen; }
    void set_area(SDL_Rect *clipto)  { viewport = *clipto; }
    void set_tile_pitch(uint8 p)     { tile_pitch = p; }
    uint8 get_tile_pitch()           { return(tile_pitch); }

//new_anim(new ExplosiveAnim(speed));
    sint32 new_anim(NuvieAnim *new_anim);
    void destroy_all();
    bool destroy_anim(uint32 anim_id);
    bool destroy_anim(NuvieAnim *anim_pt);

    NuvieAnim *get_anim(uint32 anim_id);

    void drawTile(Tile *tile, uint16 x, uint16 y);
    void drawTileAtWorldCoords(Tile *tile, uint16 wx, uint16 wy, uint16 add_x = 0, uint16 add_y = 0);

};


/* Contains methods to support management, continuous display, and movement of
 * animation across viewport.
 */
/* FIXME: The return of update() is not very useful. If an anim isn't
 * redrawn then it just disappears on next MapWindow::Display(). If you don't
 * want it to appear just delete it.*/
class NuvieAnim: public CallBack
{
protected:
    friend class AnimManager;
    AnimManager *anim_manager; // set by anim_manager when adding to list

    uint32 id_n; // unique

    sint32 vel_x, vel_y; // movement across viewport (pixels/second; min=10)
    uint32 px, py; // location on surface
    uint32 last_move_time; // last time when update_position() moved (ticks)

    bool safe_to_delete; // can animmgr delete me?
    bool updated; // call display
    bool running;

    // return false if animation doesn't need redraw
    virtual bool update() { return(true); }
    virtual void display() = 0;

    void update_position();

public:
    NuvieAnim();
    ~NuvieAnim();

    virtual MapCoord get_location() { return(MapCoord(px, py, 0)); }
    uint32 get_id()                 { return(id_n); }

    void set_safe_to_delete(bool val)       { safe_to_delete = val; }
    void set_velocity(sint32 sx, sint32 sy) { vel_x = sx; vel_y = sy; }
    void set_velocity_for_speed(sint16 xdir, sint16 ydir, uint32 spd);

    virtual void stop()                     { updated = running = false; }
    virtual void start()                    { }
    uint16 message(uint16 msg, void *msg_data = NULL, void *my_data = NULL)
                                    { if(callback_target) return(CallBack::message(msg,msg_data,my_data));
                                      else return(0); }

    virtual void move(uint32 x, uint32 y)    { px = x; py = y; }
    virtual void shift(sint32 sx, sint32 sy) { px += sx; py += sy; }

//    void set_flags();
// ANIM_ONTOP
// ANIM_ONBOTTOM
};


/* Tile placement & data for TileAnim
 */
typedef struct
{
    sint16 pos_x, pos_y; // map position relative to Anim tx,ty
    uint16 px, py; // pixel offset from pos_x,pos_y
    Tile *tile;
} PositionedTile;


/* Animation using game tiles
 */
class TileAnim : public NuvieAnim
{
protected:
    MapWindow *map_window;
    uint32 tx, ty, // location on surface: in increments of "tile_pitch"
           px, py; // location on surface: pixel offset from tx,ty

    vector<PositionedTile *> tiles;

    virtual void display();

public:
    TileAnim();
    ~TileAnim();

    MapCoord get_location() { return(MapCoord(tx, ty, 0)); }
    void get_offset(uint32 &x_add, uint32 &y_add) { x_add = px; y_add = py; }
    sint32 get_tile_id(PositionedTile *find_tile);

    void move(uint32 x, uint32 y, uint32 add_x = 0, uint32 add_y = 0)
                                     { tx = x; ty = y; px = add_x; py = add_y; }
    void shift(sint32 sx, sint32 sy);
    void shift_tile(uint32 ptile_num, sint32 sx, sint32 sy);

    PositionedTile *add_tile(Tile *tile, sint16 x, sint16 y, uint16 add_x = 0, uint16 add_y = 0);
    void remove_tile(uint32 i = 0);
};


/* TileAnim using a timed event.
 */
class TimedAnim: public TileAnim
{
protected:
    TimedCallback *timer;
public:
    TimedAnim()  { timer = NULL; }
    ~TimedAnim() { stop_timer(); }
    void start_timer(uint32 delay) { if(!timer) timer = new TimedCallback(this, NULL, delay, true); }
    void stop_timer()              { if(timer) { timer->clear_target(); timer = NULL; } }

    void stop() { stop_timer(); NuvieAnim::stop(); }
};


// OR these together to tell a TossAnim what to intercept
#define TOSS_TO_BLOCKING 0x01
#define TOSS_TO_ACTOR    0x02
#define TOSS_TO_OBJECT   0x04

/* A TileAnim that can intercept objects in the world. Start selected tile at
 * source, and move across viewport to target. The tile is rotated by the
 * degrees argument passed to the constructor.
 */
class TossAnim : public TileAnim
{
protected:
    ActorManager *actor_manager;
    ObjManager *obj_manager;
    Map *map;

    MapCoord *src, *target;
    uint32 start_px, start_py, target_px, target_py;
    uint8 mapwindow_level; // level of map being viewed
    uint16 speed; // movement speed in pixels per second (X and Y speed can't be set independently)

    Tile *toss_tile;
    uint8 blocking; // stop_flags
    uint8 tile_center; // tile_pitch / 2
    float tanS; // Ydiff/Xdiff, between src and target (for movement velocity)
    sint16 old_relpos; // when moving diagonally, last relative position on minor axis
    float x_left, y_left; // when unable to move in a call, fractional movement values are collected here
    uint16 x_dist, y_dist; // distances from start->target on X-axis & Y-axis

    bool update();
    MapCoord get_location();

public:
    TossAnim(Tile *tile, const MapCoord &start, const MapCoord &stop, uint16 pixels_per_sec, uint8 stop_flags = 0);
    TossAnim(uint16 obj_n, uint16 degrees, const MapCoord &start, const MapCoord &stop, uint16 pixels_per_sec, uint8 stop_flags = 0);
    ~TossAnim();

    void init(Tile *tile, uint16 degrees, const MapCoord &start, const MapCoord &stop, uint16 pixels_per_sec, uint8 stop_flags);
    void start();
    void stop();
    uint32 update_position(uint32 max_move = 0);
    inline void accumulate_moves(float moves, sint32 &x_move, sint32 &y_move, sint8 xdir, sint8 ydir);

    // Virtual functions are called when the tile hits something.
    virtual void hit_target();
    virtual void hit_object(Obj *obj);
    virtual void hit_actor(Actor *actor);
    virtual void hit_blocking(MapCoord obj_loc);
};

// This is for off-center tiles. The tile will be moved down by the
// shift amount if moving right, and up if moving left. (and rotated)
struct tossanim_tile_shifts_s
{
    uint16 tile_num;
    sint8 shift; // plus or minus vertical position
};
extern struct tossanim_tile_shifts_s tossanim_tile_shifts[];

/* a line of fire */
typedef struct
{
    PositionedTile *tile; // last associated sprite
    MapCoord direction; // where the explosion sprites are going
    uint32 travelled; // distance this fire line has travelled
} ExplosiveAnimSegment;

/* SuperBomberman! Toss fireballs in multiple directions from source out.
 */
class ExplosiveAnim : public TimedAnim
{
    MapCoord center;
    uint32 radius; // num. of spaces from center
    vector<ExplosiveAnimSegment> flame; // lines of fire from the center
    uint16 exploding_tile_num; // fireball effect tile_num
    vector<MapEntity> hit_items; // things the explosion has hit

public:
    ExplosiveAnim(MapCoord *start, uint32 size);
    ~ExplosiveAnim();
    void start();
    uint16 callback(uint16 msg, CallBack *caller, void *data);
    bool update();
    bool already_hit(MapEntity ent);
    void hit_object(Obj *obj);
    void hit_actor(Actor *actor);
    void get_shifted_location(uint16 &x, uint16 &y, uint16 &px, uint16 &py,
                              uint32 sx, uint32 sy);
};


/* Display hit effect over an actor or location for a certain duration.
 */
class HitAnim : public TimedAnim
{
    Actor *hit_actor;

    bool update();

public:
    HitAnim(MapCoord *loc);
    HitAnim(Actor *actor);

    uint16 callback(uint16 msg, CallBack *caller, void *msg_data) { if(msg == MESG_TIMED) stop(); return(0); }
    void start()                    { start_timer(300); }
};


#endif /* __AnimManager_h__ */
