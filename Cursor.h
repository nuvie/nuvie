#ifndef __Cursor_h__
#define __Cursor_h__

#include <string>
#include <vector>
#include "SDL.h"

class Configuration;
class Screen;
class U6Shape;

typedef struct
{
    uint16 point_x, point_y; // hotspot
    unsigned char *shapedat;
    uint16 w, h;
} MousePointer;


/* Contains all mouse pointers, with hotspot and draw methods that work on the
 * active cursor.
 */
class Cursor
{
    friend class Screen;
    Screen *screen;
    Configuration *config;
    sint32 cur_x, cur_y; // location on screen, unused normally
    std::vector<MousePointer *> cursors; // pointer list
    uint8 cursor_id; // which pointer is active

    unsigned char *cleanup; // restore image behind cursor
    SDL_Rect cleanup_area;
    SDL_Rect update_area; // clear & display are updated at once (avoid flicker)

    bool hidden;

    void add_update(uint16 x, uint16 y, uint16 w, uint16 h);
    inline void fix_position(MousePointer *ptr, sint32 &px, sint32 &py);
    void save_backing(uint32 px, uint32 py, uint32 w, uint32 h);

public:
    Cursor();
    ~Cursor()                              { unload_all(); } 
    bool init(Configuration *c, Screen *s);
    uint32 load_all(std::string filename);
    void unload_all();
    bool set_pointer(uint8 ptr_num);

    void reset_position()           { cur_x = -1; cur_y = -1; }
    void move(uint32 px, uint32 py) { cur_x = px; cur_y = py; }
    void hide()                     { hidden = true; clear(); update(); }
    void show()                     { hidden = false; }

    void get_hotspot(uint16 &x, uint16 &y) { x = cursors[cursor_id]->point_x;
                                             y = cursors[cursor_id]->point_y; }
    bool display()                         { return(display(cur_x, cur_y)); }
    bool display(sint32 px, sint32 py);
    void clear();
    void update();

};

#endif /* __Cursor_h__ */
