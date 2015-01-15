#ifndef __DirFinder_h__
#define __DirFinder_h__

class MapCoord;

class DirFinder
{
public:
    DirFinder() { }

    static void get_adjacent_dir(sint8 &xdir, sint8 &ydir, sint8 rotate);
    static uint8 get_nuvie_dir(sint16 xrel, sint16 yrel);
    static uint8 get_nuvie_dir(uint16 sx, uint16 sy, uint16 tx, uint16 ty, uint8 z);
    static sint8 get_turn_towards_dir(sint16 oxdir, sint16 oydir, sint8 txdir, sint8 tydir);
    static void get_normalized_dir(MapCoord from, MapCoord to, sint8 &xdir, sint8 &ydir);
};

#endif /* __DirFinder_h__ */
