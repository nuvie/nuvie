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

#define NUVIE_GAME_NONE  0
#define NUVIE_GAME_U6    1
#define NUVIE_GAME_MD    2
#define NUVIE_GAME_SE    4

#define NUVIE_CONFIG_NAME_U6 "ultima6"
#define NUVIE_CONFIG_NAME_MD "martian"
#define NUVIE_CONFIG_NAME_SE "savage"

#define MAX(x, y)      ((x) > (y) ? (x) : (y))
#define MIN(x, y)      ((x) < (y) ? (x) : (y))
#define clamp_min(v, c)  (((v) < (c)) ? (c) : (v))
#define clamp_max(v, c)  (((v) > (c)) ? (c) : (v))
#define clamp(v, c1, c2) ( ((v) < (c1)) ? (c1) : (((v) > (c2)) ? c2 : (v)) )

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

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;

#ifdef MACOSX
#define NUVIE_RAND random
#else
#define NUVIE_RAND rand
#endif

#endif /* __nuvieDefs_h__ */
