#ifndef __ConverseSpeech_h__
#define __ConverseSpeech_h__
/* Created by Eric Fry 
 * Copyright (C) 2004 The Nuvie Team
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
#include <cstdio>
#include <string>
#include <list>

#include "SDL.h"
#include "SDL_mixer.h"

class Configuration;
class U6Lib_n;
class U6Lzw;
class NuvieIOBuffer;

using std::string;

class ConverseSpeech
{
    // game system objects from nuvie
    Configuration *config;
    bool audio_enabled;
    int channel_num;
    std::list<Mix_Chunk *> list;

public:
    ConverseSpeech();
    ~ConverseSpeech();
    void init(Configuration *cfg);
    void update();
    void play_speech(uint16 actor_num, uint16 sample_num);

protected:
NuvieIOBuffer *load_speech(std::string filename, uint16 sample_num);
void wav_init_header(NuvieIOBuffer *wav_buffer, uint32 audio_length);    
};

#endif /* __ConverseSpeech_h__ */
