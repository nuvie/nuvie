#ifndef __SEUseCode_h__
#define __SEUseCode_h__
/*
 *  SEUseCode.h
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Jun 22 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * 
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
#include "Configuration.h"
#include "ObjManager.h"
#include "MsgScroll.h"
#include "Player.h"
#include "UseCode.h"

//object numbers
#define OBJ_SE_DOOR           234

class SEUseCode: public UseCode
{
 
 public:
 
 SEUseCode(Configuration *cfg);
 ~SEUseCode();
 
 bool use_obj(Obj *obj, Obj *src_obj=NULL);

 void init_objects() { }
 sint16 get_ucobject_index(uint16 n, uint8 f = 0) { return(-1); }
 bool uc_event(sint16 uco, uint8 ev, Obj *obj) { return(false); }
 
 protected:

};

#endif /* __SEUseCode_h__ */

