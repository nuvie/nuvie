#ifndef __U6misc_h__
#define __U6misc_h__
/*
 *  misc.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jun 14 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */

#include <string>

#include "U6def.h"
#include "Configuration.h"

void config_get_path(Configuration *config, std::string filename, std::string &path);
uint8 get_game_type(const char *string);

#endif /* __U6misc_h__ */
