#ifndef __MDUseCode_h__
#define __MDUseCode_h__
/*
 *  MDUseCode.h
 *  Nuive
 *
 *  Created by Markus Niemistö on Wed Jun 18 2003.
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

#define OBJ_MD_DOOR           152
#define OBJ_MD_CLOSED_DOOR    179 /* Half-closed-door frame = 5 */
#define OBJ_MD_LEVER          196
#define OBJ_MD_BAG            285
#define OBJ_MD_BRASS_TRUNK    304

class MDUseCode: public UseCode
{
 
 public:
 
 MDUseCode(Configuration *cfg);
 ~MDUseCode();
 
 bool use_obj(Obj *obj, Obj *src_obj=NULL);
 
 protected:

 bool use_ladder(Obj *obj);
 bool use_container(Obj *obj);
};

#endif /* __MDUseCode_h__ */
