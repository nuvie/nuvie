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

class Configuration;

std::string config_get_game_key(Configuration *config);
void config_get_path(Configuration *config, std::string filename, std::string &path);
uint8 get_game_type(const char *string);
void print_b(uint8 num);
void print_indent(uint8 indent);

#endif /* __U6misc_h__ */
