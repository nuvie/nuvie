#ifndef __EffectManager_h__
#define __EffectManager_h__

#include <vector>

class Effect;

/* This just keeps a list of Effect pointers, and deletes them when requested.
 */
class EffectManager/* : public CallbackTarget*/
{
// FIXME: why are we handling messages from Effects? unecessary
friend class Effect;
    typedef std::vector<Effect *>::iterator EffectIterator;
/*
    typedef std::vector<EffectMessage>::iterator MessageIterator;
    typedef struct
    {
        CallbackMessage msg;
        char *msg_data;
        Effect *sender;
    } EffectMessage;
*/

    std::vector<Effect *> effects; // the simple list
/*
    std::vector<EffectMessage> pending_messages; // in recv order front to back
*/

public:
    EffectManager();
    ~EffectManager();

private:
    void add_effect(Effect *eff); // only effects can add themselves
public:
    void delete_effect(Effect *eff); // anyone may delete an effect
/*
    void clear_messages(Effect *eff); // clear its messages when deleting

    bool has_message(Effect *eff = NULL); // there are pending messages
    bool get_message(CallbackMessage *msg, void **msg_data, Effect **eff = NULL);
    bool get_message_from_effect(Effect *eff, CallbackMessage *msg, void **msg_data);
    void copy_message(CallbackMessage *msg, void **msg_data, Effect **eff, EffectMessage pending_msg);
*/

    void update_effects(); // check and delete
/*
    void message_callback(CallbackMessage msg, void *msg_data); // receive msg
*/

    bool has_effects();
};

#endif /* __EffectManager_h__ */
