#ifndef __EffectManager_h__
#define __EffectManager_h__

#include <vector>

class Effect;

/* This just keeps a list of Effect pointers, and deletes them when requested.
 */
class EffectManager
{
friend class Effect;
    typedef std::vector<Effect *>::iterator EffectIterator;
    /* For each EffectWatch, a message will be sent to "watcher" when
       "effect" is deleted. */
    typedef struct
    {
        CallBack *watcher;
        Effect *effect;
    } EffectWatch;
    typedef std::vector<EffectWatch>::iterator WatchIterator;

    std::vector<Effect *> effects; // the simple list
    std::vector<EffectWatch> watched;

    void add_effect(Effect *eff); // only effects can add themselves
    void signal_watch(Effect *effect);
    EffectWatch *find_effect_watch(Effect *effect);

public:
    EffectManager();
    ~EffectManager();

    void delete_effect(Effect *eff); // anyone may delete an effect
    void update_effects(); // check and delete

    bool has_effects();
    void watch_effect(CallBack *callback_target, Effect *watch);
    void unwatch_effect(CallBack *callback_target, Effect *watch = NULL);
};

#endif /* __EffectManager_h__ */
