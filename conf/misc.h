/*
 *  misc.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Mar 11 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

/*
#ifdef _MSC_VER
#include "../msvc/msvc_kludges.h"
#endif
*/

#include <cstdio>
#include <string>
// moved to nuvieDefs.h and superseded by DEBUG(x...) 
//#define PERR(x) fprintf(stderr,x)

extern bool string_i_compare(const std::string &s1, const std::string &s2);
