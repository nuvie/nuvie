//
// Created by efry on 29/10/2015.
//

#ifndef NUVIE_SDL_COMPAT_H
#define NUVIE_SDL_COMPAT_H
#if !SDL_VERSION_ATLEAST(2,0,0)

#define SDLK_UNKNOWN SDLK_LAST
#define SDL_Keycode SDL_keycode
#define SDL_JoystickNameForIndex SDL_JoystickName
#endif
#endif //NUVIE_SDL_COMPAT_H
