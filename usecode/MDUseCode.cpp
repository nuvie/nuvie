/*
 *  MDUseCode.cpp
 *  Nuvie
 *
 *  Created by Markus Niemistö on Wed Jun 18 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "U6def.h"
#include "MsgScroll.h"
#include "MDUseCode.h"

MDUseCode::MDUseCode(Configuration *cfg) : UseCode(cfg)
{

}

MDUseCode::~MDUseCode()
{
}

bool MDUseCode::use_obj(Obj *obj, Obj *src_obj)
{
 
 if(obj == NULL)
   return false;

  switch(obj->obj_n)
   {
    case OBJ_MD_DOOR :
    case OBJ_MD_HATCH : if(obj->frame_n < 3)
                        {
                         obj->frame_n = 3;
                        }
                      else
                        {
                         obj->frame_n = 1;
                        }
                      break;
                      
    case OBJ_MD_OUTER_HATCH : if(obj->frame_n == 1) //Blast the hatch open... cutscene here FIX
                                {
                                 obj_manager->move(obj,obj->x+1,obj->y+1,obj->z);
                                 obj->frame_n = 2;
                                }
                              break;
                          
    case OBJ_MD_WOODEN_CRATE : if(obj->frame_n == 2) //we can't open nailed crates by hand.
                                 break;
    case OBJ_MD_BRASS_TRUNK :
    case OBJ_MD_STEAMER_TRUNK :
    case OBJ_MD_BARREL :
                      toggle_frame(obj); //open / close object
    case OBJ_MD_BAG : use_container(obj);
                      break;
                      
    default : scroll->display_string("\nnot usable\n");
              break;
   }


 printf("Use Obj #%d Frame #%d\n",obj->obj_n, obj->frame_n);

 return true;
}
