#ifndef __U6def_h__
#define __U6def_h__

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

#define U6PATH_DELIMITER '/'

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

#endif /* __U6def_h__ */
