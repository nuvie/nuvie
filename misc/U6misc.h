#ifndef __ERICmisc_h__
#define __ERICmisc_h__
/*
 *  misc.h
 *  Nuive
 *
 *  Created by Eric Fry on Sat Jun 14 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include <string>

#include "U6def.h"
#include "Configuration.h"

void config_get_path(Configuration *config, std::string filename, std::string &path);

#endif /* __ERICmisc_h__ */
