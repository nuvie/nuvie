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
#include "SDL.h"

class Configuration;

std::string config_get_game_key(Configuration *config);
const char *get_game_tag(int game_type);
void config_get_path(Configuration *config, std::string filename, std::string &path);
uint8 get_game_type(const char *string);
void build_path(std::string path, std::string filename, std::string &full_path);
bool directory_exists(const char *directory);
void print_b(uint8 num);
void print_indent(uint8 indent);
void print_bool(bool state, const char *yes = "true", const char *no = "false");
void print_flags(uint8 num, const char *f[8]);
inline bool point_in_rect(uint16 x, uint16 y, SDL_Rect *rect);
inline bool line_in_rect(uint16 x1, uint16 y1, uint16 x2, uint16 y2, SDL_Rect *rect);
bool subtract_rect(SDL_Rect *rect1, SDL_Rect *rect2, SDL_Rect *sub_rect);

#endif /* __U6misc_h__ */
