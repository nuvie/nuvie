/*
 *  Look.cpp
 *  Nuive
 *
 *  Created by Eric Fry on Mon Mar 24 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
#include "U6def.h"
#include "Configuration.h"

#include "U6Lzw.h"

#include "Look.h"

Look::Look(Configuration *cfg)
{
 config = cfg;
}
 
bool Look::init()
{
 std::string filename;
 U6Lzw lzw;
 uint32 decomp_size;
 unsigned char *look_data;
 unsigned char *ptr;
 char *s;
 uint16 i,j;
 
 config->pathFromValue("config/ultima6/gamedir","look.lzd",filename);
 
 look_data = lzw.decompress_file(filename,decomp_size);
 if(look_data == NULL)
   return false;

 ptr = look_data;
 
 for(i=0,j=0;i < 2048;)
   {
    i = ptr[0] + (ptr[1] << 8);

    if(i >= 2048)
       break;
    s = readDescription(&ptr[2]);
    
    ptr += strlen(s) + 3;
    
    for(;j<=i;j++)
      look_tbl[j] = s;
   }

 for(i=j;i < 2048;i++)
   {
    look_tbl[i] = look_tbl[0]; // nothing
   }
   
 free(look_data);
 
 return true;
}

char *Look::get_description(uint16 tile_num)
{
 if(tile_num >= 2048)
   return NULL;

 return look_tbl[tile_num];
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

char *Look::readDescription(unsigned char *ptr)
{
 uint16 i;
 char *s;
 
 for(i = 0;;i++)
  {
   if(ptr[i] == '\0')
     break;
  }
 
 s = (char *)malloc(i+1);
 
 strcpy(s,(char *)ptr);
 
 return s;
}
