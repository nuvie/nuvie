#ifndef __MDUseCode_h__
#define __MDUseCode_h__
/*
 *  MDUseCode.h
 *  Nuvie
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

#include "ObjManager.h"
#include "UseCode.h"

//object numbers
#define OBJ_MD_WOODEN_CRATE    86
#define OBJ_MD_STEAMER_TRUNK   87
#define OBJ_MD_BARREL         104
#define OBJ_MD_DOOR           152
#define OBJ_MD_CLOSED_DOOR    179 /* Half-closed-door frame = 5 */
#define OBJ_MD_LEVER          196
#define OBJ_MD_BAG            285
#define OBJ_MD_BRASS_TRUNK    304
#define OBJ_MD_HATCH          421
#define OBJ_MD_OUTER_HATCH    427

class Configuration;
class Game;
class MsgScroll;

class MDUseCode: public UseCode
{

 public:

 MDUseCode(Game *g, Configuration *cfg);
 ~MDUseCode();

 bool use_obj(Obj *obj, Obj *src_obj=NULL);

 void init_objects() { }
 sint16 get_ucobject_index(uint16 n, uint8 f, uint8 ev) { return(-1); }
 bool uc_event(sint16 uco, uint8 ev, Obj *obj) { return(false); }
 bool cannot_unready(Obj *obj, bool cancel_event = false) { return false; }

 protected:

};

#endif /* __MDUseCode_h__ */
