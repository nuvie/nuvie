/*
 *  Look.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Mon Mar 24 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
#include <ctype.h>

#include "U6def.h"
#include "Configuration.h"

#include "U6Lzw.h"

#include "Look.h"

Look::Look(Configuration *cfg)
:look_data(NULL),desc_buf(NULL)
{
 config = cfg;
 
 max_len = 0;
}

Look::~Look()
{
 free(look_data);
 free(desc_buf);
}

bool Look::init()
{
 std::string filename;
 U6Lzw lzw;
 uint32 decomp_size;
 unsigned char *ptr;
 const char *s;
 uint16 i,j;
 unsigned int len;
 
 config->pathFromValue("config/ultima6/gamedir","look.lzd",filename);
 
 look_data = lzw.decompress_file(filename,decomp_size);
 if(look_data == NULL)
    return false;

 ptr = look_data;

 // i: current string pos, j: last string pos
 for(i=0,j=0;i < 2048;)
   {
    // get number of string
    i = ptr[0] + (ptr[1] << 8);

    if(i >= 2048)
       break;

    // store pointer to look_data buffer
    look_tbl[i] = s = reinterpret_cast<char*>(&ptr[2]);

    // update max_len
    len = strlen(s);
    if(max_len < len)
     max_len = len;

    ptr += len + 3;

    // fill all empty strings between current and last one
    for(;j<=i;j++)
      look_tbl[j] = s;
   }

 // fill remaining strings with "Nothing"
 for(i=j;i < 2048;i++)
   {
    look_tbl[i] = look_tbl[0]; // nothing
   }

 // allocate space for description buffer
 desc_buf = (char *)malloc(max_len+1);
 if(desc_buf == NULL)
   return false;

 return true;
}

const char *Look::get_description(uint16 tile_num, bool plural)
{
 const char *desc;
 char c;
 uint16 i, j;
 uint16 len;
 
 if(tile_num >= 2048)
   return NULL;

 desc = look_tbl[tile_num];
 
 len = strlen(desc);
 
 for(i=0,j=0;i < len;)
   {
    if(desc[i] == '\\' || desc[i] == '/')
      {
       c = desc[i];
       for(i++;isalpha(desc[i]) && i < len;i++)
        {
         
         if((plural && c == '\\' ) || ( !plural && c == '/' ))
           { desc_buf[j] = desc[i]; j++; }
        }
      }
    else
      {
       desc_buf[j] = desc[i];
       i++;
       j++;
      }
   }

 desc_buf[j] = desc[i];

 return desc_buf;
}

uint16 Look::get_max_len()
{
 return max_len;
}
 
void Look::print()
{
 uint16 i;
 
 for(i=0;i<2048;i++)
  {
   printf("%04d :: %s\n",i,look_tbl[i]);
  }
  
 return;
}
