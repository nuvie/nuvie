//
// Created by efry on 29/10/2015.
//

#ifndef NUVIE_SDL_COMPAT_H
#define NUVIE_SDL_COMPAT_H

#include <SDL.h>

#if !SDL_VERSION_ATLEAST(1,3,0)
#define SDLK_UNKNOWN SDLK_LAST
#define SDL_Keycode SDLKey
#define SDL_Keymod SDLMod
#define SDL_Keysym SDL_keysym
#define KMOD_GUI KMOD_META
#define SDL_JoystickNameForIndex SDL_JoystickName
#define SDL_Window void
#define SDL_Renderer void
#define SDL_Texture void

#define SDLK_KP_0 SDLK_KP0
#define SDLK_KP_1 SDLK_KP1
#define SDLK_KP_2 SDLK_KP2
#define SDLK_KP_3 SDLK_KP3
#define SDLK_KP_4 SDLK_KP4
#define SDLK_KP_5 SDLK_KP5
#define SDLK_KP_6 SDLK_KP6
#define SDLK_KP_7 SDLK_KP7
#define SDLK_KP_8 SDLK_KP8
#define SDLK_KP_9 SDLK_KP9

#define SDL_CreateThread(x, y, z) SDL_CreateThread(x, z)

#define SDL_TRUE SDL_SRCCOLORKEY
#else

#endif
#endif //NUVIE_SDL_COMPAT_H
