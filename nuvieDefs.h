#ifndef __nuvieDefs_h__
#define __nuvieDefs_h__

#ifdef _MSC_VER

#if _MSC_VER < 1300
#include "msvc/msvc_kludges.h"
#elif _MSC_VER >= 1300
#include "visualc7/vc7inc.h"
#endif

#endif

#if defined(__GNUC__) && (__GNUC__ < 3)
# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif
#endif

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

typedef uint8 nuvie_game_t; // Game type (1=u6,2=md,4=se)

#define NUVIE_GAME_NONE  0
#define NUVIE_GAME_U6    1
#define NUVIE_GAME_MD    2
#define NUVIE_GAME_SE    4

#define NUVIE_CONFIG_NAME_U6 "ultima6"
#define NUVIE_CONFIG_NAME_MD "martian"
#define NUVIE_CONFIG_NAME_SE "savage"

#define NUVIE_STYLE_ORIG 0
#define NUVIE_STYLE_NEW  1

#define MAX(x, y)      ((x) > (y) ? (x) : (y))
#define MIN(x, y)      ((x) < (y) ? (x) : (y))
#define clamp_min(v, c)  (((v) < (c)) ? (c) : (v))
#define clamp_max(v, c)  (((v) > (c)) ? (c) : (v))
#define clamp(v, c1, c2) ( ((v) < (c1)) ? (c1) : (((v) > (c2)) ? (c2) : (v)) )

#define WRAPPED_COORD(c,level) ((c)&((level)?255:1023))
#define WRAP_COORD(c,level) ((c)&=((level)?255:1023))
/* 
 * on all levels, except level 0 (conveniently 'false') the map pitch is 256.
 * to properly wrap, mask the coordinate with the relevant bit-mask. 
 * Another way to write this would be:
 
const uint16 map_pitch[2] = { 1024, 256 }; // width of 0:surface plane, and 1:all other planes 
#define WRAPPED_COORD(c,level) ((c)&(map_pitch[(level==0)?0:1]-1)) // mask high bit, wrap C to map_pitch
#define WRAP_COORD(c,level) ((c)&=(map_pitch[(level==0)?0:1]-1)) // modifies C
*/

#define NUVIE_DIR_N    0
#define NUVIE_DIR_E    1
#define NUVIE_DIR_S    2
#define NUVIE_DIR_W    3

#define NUVIE_DIR_NE   4
#define NUVIE_DIR_SE   5
#define NUVIE_DIR_SW   6
#define NUVIE_DIR_NW   7

#define NUVIE_DIR_NONE 8

#define TRAMMEL_PHASE 1.75
#define FELUCCA_PHASE 1.1666666666666667

#ifdef WITHOUT_DEBUG
#define DEBUG(x...)
#else
#define DEBUG(x...) debug(__func__,__FILE__,__LINE__,x)
typedef enum { LEVEL_EMERGENCY=0, LEVEL_ALERT, LEVEL_CRITICAL, LEVEL_ERROR, LEVEL_WARNING, LEVEL_NOTIFICATION, LEVEL_INFORMATIONAL, LEVEL_DEBUGGING } DebugLevelType;
DebugLevelType debug(const char * func, const char * file, const int line, const bool no_header,const DebugLevelType level, const char *format, ...);
#endif /* WITHOUT_DEBUG */

/*
#ifndef BOOL
typedef unsigned char BOOL;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif
*/

#ifndef WIN32
  #define U6PATH_DELIMITER '/'
#else
  #define U6PATH_DELIMITER '\\'
  //#define U6PATH_DELIMITER '/' // win32 file functions work fine with /
#endif


#ifdef MACOSX
#define NUVIE_RAND random
#else
#define NUVIE_RAND rand
#endif

#define MAXPATHLEN 256

#define nuprint(T...) Game::get_game()->get_scroll()->printf(T)
/*#define nuprint(T) Game::get_game()->get_scroll()->display_string(T)*/
/*#define nuprint(T) Game::get_game()->get_scroll()->display_string(T); \
                     Game::get_game()->get_scroll()->display_string("\n");*/

#endif /* __nuvieDefs_h__ */
