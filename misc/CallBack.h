/* Created by Joseph Applegate
 * Copyright (C) 2003 The Nuvie Team
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
#ifndef __CallBack_h__
#define __CallBack_h__
#include <cstdio>

class CallBack;

// general messages
typedef enum
{
    CB_TIMED,               /* timer fired (data=uint32:time) */

    ANIM_CB_DONE,           /* animation has finished */
    ANIM_CB_HIT,            /* animation hit a MapEntity (data=MapEntity) */
    ANIM_CB_HIT_WORLD,      /* animation hit some other tile on the map (data=MapCoord) */

    EFFECT_CB_COMPLETE,     /* effect has finished */

    CB_DATA_READY,          /* some data is ready to be retrieved (data=char)*/
    MSGSCROLL_CB_TEXT_READY /* text is ready to be retrieved (data=std::string)*/
} CallbackMessage;


/* Classes inheriting this can send & receive callback messages.
 */
class CallBack
{
protected:
    char *callback_user_data; // data set by Caller
    CallBack *callback_target; // Caller: default CallBack that message() sends to

public:
    CallBack()  { callback_user_data = NULL; callback_target = NULL; }
    virtual ~CallBack() { }

    // receive message
    virtual uint16 callback(uint16 msg, CallBack *caller, void *data = NULL) {     PERR("Warning! Unhandled callback. msg (%x)\n", msg); return 0; }
    // send message
    uint16 message(uint16 msg, void *msg_data = NULL, void *my_data = NULL)
    {
        if(my_data)
            set_user_data(my_data);
        callback_target->set_user_data(callback_user_data);
        
        return(callback_target->callback(msg, this, msg_data));
    }

    void set_user_data(void *user_data) { callback_user_data = (char*)user_data; }
    void set_target(CallBack *t) { callback_target = t; }
};

#endif /* __CallBack_h__ */
