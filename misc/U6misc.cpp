/*
 *  misc.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sat Jun 14 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <cstdio>
#include <string>
#include <vector>
#include "nuvieDefs.h"

#include "U6misc.h"
#include "Configuration.h"

using namespace std;

bool find_casesensitive_path(std::string path, std::string filename, std::string &new_path);
bool find_path(std::string path, std::string &dir_str);

void Tokenise( const std::string& str, std::vector<std::string>& tokens, char delimiter = ' ' )
{
   std::string delimiters = string();
   delimiters = delimiter;

   // Skip delimiters at beginning.
   string::size_type lastPos = str.find_first_not_of( delimiters, 0 );

   for( string::size_type pos = str.find_first_of( delimiters, lastPos ) ;
        string::npos != pos || string::npos != lastPos ;
        pos = str.find_first_of( delimiters, lastPos ) )
   {
      // Found a token, add it to the vector.
      tokens.push_back( str.substr( lastPos, pos - lastPos ) );
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of( delimiters, pos );
   }
}

std::string config_get_game_key(Configuration *config)
{
 std::string game_key, game_name;

 config->value("config/GameName",game_name);

 game_key.assign("config/");
 game_key.append(game_name);

 return game_key;
}

const char *get_game_tag(int game_type)
{
 switch(game_type)
   {
    case NUVIE_GAME_U6 : return "U6";
    case NUVIE_GAME_MD : return "MD";
    case NUVIE_GAME_SE : return "SE";
   }

 return "";
}

void config_get_path( Configuration *config, std::string filename, std::string &path )
{
   std::string key, game_name, game_dir, tmp_path;

   config->value( "config/GameName", game_name );

   key.assign( "config/" );
   key.append( game_name );
   key.append( "/gamedir" );

   config->pathFromValue( key, "", game_dir );

   tmp_path = game_dir + filename;

   struct stat s;

   if( stat( tmp_path.c_str(), &s ) == 0 )
   {
      path = tmp_path;
      return;
   }

   find_casesensitive_path( game_dir, filename, tmp_path );

   path = tmp_path;

   return;
}

bool find_casesensitive_path( std::string path, std::string filename, std::string &new_path )
{
   vector<string> directories;
   string tmp_path = path;

   Tokenise( filename, directories, U6PATH_DELIMITER );

   std::vector<string>::iterator dir_iter;

   for( dir_iter = directories.begin(); dir_iter != directories.end(); )
   {
      string dir = *dir_iter;

      printf( "%s, ", dir.c_str() );

      if( find_path( tmp_path, dir ) == false )
         return false;

      dir_iter++;

      if( dir_iter != directories.end() )
         dir += U6PATH_DELIMITER;

      tmp_path += dir;
   }

   new_path = tmp_path;

   printf( "\nproper path = %s\n", new_path.c_str() );
   return true;
}

bool find_path( std::string path, std::string &dir_str )
{
   DIR *dir;
   struct dirent *item;

   dir = opendir( path.c_str() );
   if( dir == NULL )
      return false;

   for( item = readdir( dir ); item != NULL; item = readdir( dir ) )
   {
      printf( "trying %s, want %s\n", item->d_name, dir_str.c_str() );
      if( strlen(item->d_name) == dir_str.length() && strcasecmp( item->d_name, dir_str.c_str() ) == 0 )
      {
         dir_str = item->d_name;
         return true;
      }
   }

   return false;
}

void stringToUpper(std::string &str)
{
	for (size_t i = 0; i < str.length(); ++i)
    {
        str[i]=toupper(str[i]);
    }
}

int mkdir_recursive(std::string path, int mode)
{
   vector<string> directories;
   string tmp_path;
   
   Tokenise( path, directories, U6PATH_DELIMITER );
   
   std::vector<string>::iterator dir_iter;
   
   for( dir_iter = directories.begin(); dir_iter != directories.end(); )
   {
      string dir = *dir_iter;
      
      printf( "%s, ", dir.c_str() );
      
      tmp_path += U6PATH_DELIMITER;
      tmp_path += dir;
      
      if(directory_exists(tmp_path.c_str()) == false)
      {
#if defined(WIN32)
         int ret = mkdir(tmp_path.c_str());
#else
         int ret = mkdir(tmp_path.c_str(), mode);
#endif
         if(ret != 0)
            return ret;
      }      
      dir_iter++;
   }
   
   return 0;
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

void build_path(std::string path, std::string filename, std::string &full_path)
{
 full_path = path;

 if(full_path.length() > 0 && full_path[full_path.length()-1] != U6PATH_DELIMITER)
    full_path += U6PATH_DELIMITER + filename;
 else
    full_path += filename;

 return;
}

bool directory_exists(const char *directory)
{
 struct stat sb;

 if(stat(directory, &sb) != 0)
   return false;

 if(!(sb.st_mode | S_IFDIR))
   return false;

 return true;
}

void print_b(DebugLevelType level,uint8 num)
{
 sint8 i;

 for(i=7;i>=0;i--)
 {
  if(num & (1<<i))
    DEBUG(1,level,"1");
  else
    DEBUG(1,level,"0");
 }

 return;
}

void print_b16(DebugLevelType level,uint16 num)
{
  sint8 i;
  
  for(i=15;i>=0;i--)
  {
    if(num & (1<<i))
      DEBUG(1,level,"1");
    else
      DEBUG(1,level,"0");
  }
  
  return;
}

void print_indent(DebugLevelType level,uint8 indent)
{
 uint16 i;

 for(i=0;i < indent;i++)
  DEBUG(1,level," ");

 return;
}


void print_bool(DebugLevelType level,bool state, const char *yes, const char *no)
{
    DEBUG(1,level,"%s", state ? yes : no);
}


void print_flags(DebugLevelType level,uint8 num, const char *f[8])
{
    std::string complete_flags = "";
    print_b(level,num);
    if(num != 0)
        complete_flags += "(";
    for(uint32 i = 0; i < 8; i++)
        if((num & (1 << i)) && f[i])
            complete_flags += f[i];
    if(num != 0)
        complete_flags += ")";
    DEBUG(1,level,"%s", complete_flags.c_str());
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


/* Returns name of relative direction. 0,0 prints "nowhere".
 */
const char *get_direction_name(sint16 rel_x, sint16 rel_y)
{
    if(rel_x == 0 && rel_y < 0)
        return("North");
    else if(rel_x > 0 && rel_y < 0)
        return("Northeast");
    else if(rel_x > 0 && rel_y == 0)
        return("East");
    else if(rel_x > 0 && rel_y > 0)
        return("Southeast");
    else if(rel_x == 0 && rel_y > 0)
        return("South");
    else if(rel_x < 0 && rel_y > 0)
        return("Southwest");
    else if(rel_x < 0 && rel_y == 0)
        return("West");
    else if(rel_x < 0 && rel_y < 0)
        return("Northwest");
    else
        return("nowhere");
}

int str_bsearch( const char *str[], int max, const char *value )
{
   int position;
   int begin = 0;
   int end = max - 1;
   int cond = 0;

   while(begin <= end)
   {
      position = (begin + end) / 2;
      if( (cond = strcmp( str[position], value ) ) == 0)
         return position;
      else if(cond < 0)
         begin = position + 1;
      else
         end = position - 1;
   }

   return -1;
}
