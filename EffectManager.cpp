#include "nuvieDefs.h"
#include "Map.h"
#include "TimedEvent.h"
#include "Effect.h"
#include "EffectManager.h"


EffectManager::EffectManager()
{
}


EffectManager::~EffectManager()
{
    while(!effects.empty())
        delete_effect(effects.front());
}


#if 0
/* Remove any messages pending from effect.
 */
void EffectManager::clear_messages(Effect *eff)
{
    MessageIterator mi = pending_messages.begin();
    while(mi != pending_messages.end())
    {
        if((*mi).sender == eff)
        {
            pending_messages.erase(mi);
            mi = pending_messages.begin();
        }
        else ++mi;
    }
}
#endif


/* Delete an effect and remove it from the list.
 */
void EffectManager::delete_effect(Effect *eff)
{
    EffectIterator ei = effects.begin();
    while(ei != effects.end())
    {
        if(*ei == eff)
        {
            fprintf(stderr, "EffectManager: delete %x\n", eff);
            signal_watch(eff);
            delete eff;
            effects.erase(ei);
            return;
        }
        ++ei;
    }
}


/* Add an (already existant) effect to the list.
 */
void EffectManager::add_effect(Effect *eff)
{
    fprintf(stderr, "EffectManager: new %x\n", eff);
    effects.push_back(eff);
}


/* Delete completed effects.
 */
void EffectManager::update_effects()
{
    EffectIterator ei = effects.begin();
    while(ei != effects.end())
    {
        if((*ei)->is_defunct()/* && !has_message(*ei)*/)
        {
            fprintf(stderr, "EffectManager: delete %x\n", *ei);
            signal_watch(*ei);
            delete(*ei);
            ei = effects.erase(ei);
        }
        else ++ei;
    }
}


#if 0
/* Returns true if there are any pending messages. Optionally pass an effect to
 * only check for messages from that effect.
 */
bool EffectManager::has_message(Effect *eff)
{
    MessageIterator mi = pending_messages.begin();
    while(mi != pending_messages.end())
    {
        if(eff == NULL) // true on any message
            return(true);
        if((*mi).sender == eff) // true if effect is matched
            return(true);
        ++mi;
    }
    return(false);
}


/* Get the earliest received message. If effect pointer is passed it will be set
 * to the effect that sent the message. (which might not be valid after getting
 * the message)
 * Returns true if a message was waiting.
 */
bool EffectManager::get_message(CallbackMessage *msg, void **msg_data, Effect **eff)
{
    EffectMessage pending_msg;
    if(pending_messages.empty()) // no messages
        return(false);

    // get & remove from list
    pending_msg = pending_messages.front();
    pending_messages.erase(pending_messages.begin());

    copy_message(msg, msg_data, eff, pending_msg); // set
    return(true);
}


/* Get the earliest received message from a specific effect.
 * Returns true if a message was waiting.
 */
bool EffectManager::get_message_from_effect(Effect *eff, CallbackMessage *msg, void **msg_data)
{
    EffectMessage pending_msg;
    MessageIterator mi = pending_messages.begin();
    if(pending_messages.empty()) // no messages
        return(false);
    while(mi != pending_messages.end())
    {
        pending_msg = *mi; // get
        if(pending_msg.sender == eff)
        {
            pending_messages.erase(mi); // remove from list
            copy_message(msg, msg_data, NULL, pending_msg); // set
            return(true);
        }
        ++mi;
    }
    return(false); // no messages from effect
}


/* Just copies message data from an "EffectMessage" to data pointers.
 */
void EffectManager::copy_message(CallbackMessage *msg, void **msg_data, Effect **eff, EffectMessage pending_msg)
{
    if(msg)
        *msg = pending_msg.msg;
    if(msg_data)
        *msg_data = pending_msg.msg_data;
    if(eff)
        *eff = pending_msg.sender;
}


/* Messages can only be (should only be) sent by Effects. Add to the list.
 */
void EffectManager::message_callback(CallbackMessage msg, void *msg_data)
{
    EffectMessage new_message;
    new_message.msg = msg;
    new_message.msg_data = (char *)msg_data;
    new_message.sender = c_user_data; // FIXME?
    pending_messages.push_back(new_message);
}
#endif

/* Returns true if there are any effects still active.
 */
bool EffectManager::has_effects()
{
    if(!effects.empty())
    {
        EffectIterator i = effects.begin();
        while(i != effects.end())
            if(!(*i)->is_defunct()) // effect is still active
                return(true);
    }
    return(false); // no effects, or all effects are complete
}


/* Add a watched effect. This will send effect completion message to the
 * target when the effect is deleted.
 */
void EffectManager::watch_effect(CallBack *callback_target, Effect *watch)
{
    EffectWatch new_watch;
    new_watch.watcher = callback_target;
    new_watch.effect = watch;
    watched.push_back(new_watch);
}


/* Remove a watched effect, or all watched effects for target.
 */
void EffectManager::unwatch_effect(CallBack *callback_target, Effect *watch)
{
    if(!watched.empty())
    {
        WatchIterator i = watched.begin();
        while(i != watched.end())
            if((*i).watcher == callback_target
               && ((*i).effect == watch || watch == NULL))
            {
                watched.erase(i);
                i = watched.begin(); // not efficient, but just start from beginning of list again
            }
    }
}


/* Signal effect completion if it is being watched, and stop watching it.
 */
void EffectManager::signal_watch(Effect *effect)
{
    EffectWatch *watch = find_effect_watch(effect);
    if(watch)
    {
        if(watch->watcher)
            watch->watcher->callback(EFFECT_CB_COMPLETE, NULL, effect);
        unwatch_effect(watch->watcher, effect);
    }
}


/* Returns watch for an effect. (or NULL)
 */
EffectManager::EffectWatch *EffectManager::find_effect_watch(Effect *effect)
{
    if(!watched.empty())
    {
        WatchIterator i = watched.begin();
        while(i != watched.end())
            if((*i).effect == effect)
                return(&(*i));
    }
    return(NULL);
}
