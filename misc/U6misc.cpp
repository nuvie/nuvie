/*
 *  misc.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jun 14 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */
#include <cstdio>
#include <string>
#include "nuvieDefs.h"

#include "U6misc.h"
#include "Configuration.h"

std::string config_get_game_key(Configuration *config)
{
 std::string game_key, game_name;
 
 config->value("config/GameName",game_name);
 
 game_key.assign("config/");
 game_key.append(game_name);
 
 return game_key;
}

void config_get_path(Configuration *config, std::string filename, std::string &path)
{
 std::string key, game_name;
 
 config->value("config/GameName",game_name);
 
 key.assign("config/");
 key.append(game_name);
 key.append("/gamedir");
    
 config->pathFromValue(key, filename, path);
 
 return;
}

//return the uint8 game_type from a char string
uint8 get_game_type(const char *string)
{
 if(string != NULL && strlen(string) >= 2)
   {
    if(strcmp("md",string) == 0 || strcmp("martian",string) == 0)
      return NUVIE_GAME_MD;
    if(strcmp("se",string) == 0 || strcmp("savage",string) == 0)
      return NUVIE_GAME_SE;
    if(strcmp("u6",string) == 0 || strcmp("ultima6",string) == 0)
      return NUVIE_GAME_U6;
   } 

 return NUVIE_GAME_NONE;
}

void print_b(uint8 num)
{
 sint8 i;
 
 for(i=7;i>=0;i--)
 {
  if(num & (1<<i))
    printf("1");
  else
    printf("0");
 }
 
 return;
}

void print_indent(uint8 indent)
{
 uint16 i;
 
 for(i=0;i < indent;i++)
  printf(" ");

 return;
}


void print_bool(bool state, const char *yes, const char *no)
{
    printf("%s", state ? yes : no);
}


void print_flags(uint8 num, const char *f[8])
{
    std::string complete_flags = "";
    print_b(num);
    if(num != 0)
        complete_flags += "(";
    for(uint32 i = 0; i < 8; i++)
        if((num & (1 << i)) && f[i])
            complete_flags += f[i];
    if(num != 0)
        complete_flags += ")";
    printf("%s", complete_flags.c_str());
}


/* Is point x,y within rect?
 */
inline bool point_in_rect(uint16 x, uint16 y, SDL_Rect *rect)
{
    uint16 rx2 = rect->x + rect->w,
           ry2 = rect->y + rect->h;
    return((x >= rect->x && x <= rx2 && y >= rect->y && y <= ry2));
}


/* Does line xy->x2y2 cross rect, to any extent?
 */
inline bool line_in_rect(uint16 x1, uint16 y1, uint16 x2, uint16 y2, SDL_Rect *rect)
{
    uint16 rx2 = rect->x + rect->w,
           ry2 = rect->y + rect->h;
    return(((y1 >= rect->y && y1 <= ry2 && x1 <= rx2 && x2 >= rect->x)
            || (x1 >= rect->x && x1 <= rx2 && y1 <= ry2 && y2 >= rect->y)));
}


/* Where rect1 and rect2 merge, subtract and copy that rect to sub_rect.
 * Returns false if the rectangles don't intersect.
 */
bool subtract_rect(SDL_Rect *rect1, SDL_Rect *rect2, SDL_Rect *sub_rect)
{
    uint16 rect1_x2 = rect1->x + rect1->w, rect1_y2 = rect1->y + rect1->h;
    uint16 rect2_x2 = rect2->x + rect2->w, rect2_y2 = rect2->y + rect2->h;
    uint16 x1, x2, y1, y2;

    if(line_in_rect(rect1->x, rect1->y, rect1_x2, rect1->y, rect2)
       || line_in_rect(rect1_x2, rect1->y, rect1_x2, rect1_y2, rect2)
       || line_in_rect(rect1->x, rect1->y, rect1->x, rect1_y2, rect2)
       || line_in_rect(rect1->x, rect1_y2, rect1_x2, rect1_y2, rect2))
    {
        x1 = rect2->x >= rect1->x ? rect2->x : rect1->x;
        y1 = rect2->y >= rect1->y ? rect2->y : rect1->y;
        x2 = rect2_x2 <= rect1_x2 ? rect2_x2 : rect1_x2;
        y2 = rect2_y2 <= rect1_y2 ? rect2_y2 : rect1_y2;
    }
    else
        return(false);
    if(sub_rect) // you can perform test without returning a subtraction
    {
        sub_rect->x = x1;
        sub_rect->y = y1;
        sub_rect->w = x2 - x1;
        sub_rect->h = y2 - y1;
    }
    return(true);
}
