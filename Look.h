#ifndef __Look_h__
#define __Look_h__
/*
 *  Look.h
 *  Nuive
 *
 *  Created by Eric Fry on Mon Mar 24 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include "U6def.h"
#include "Configuration.h"

class Look
{
 Configuration *config;
 char *look_tbl[2048];
 uint16 max_len;
 char *desc_buf;
 
 public:
 
 Look(Configuration *cfg);
 
 bool init();
 
 char *get_description(uint16 tile_num, bool plural);
 uint16 get_max_len();
 
 void print();
 
 protected:
 
 char *readDescription(unsigned char *ptr);
};

#endif /* __Look_h__ */
