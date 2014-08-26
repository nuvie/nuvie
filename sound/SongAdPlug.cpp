/*
 *  SongAdPlug.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Wed Feb 11 2004.
 *  Copyright (c) 2004. All rights reserved.
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

//Mix_HookMusicFinished

#include "nuvieDefs.h"
#include "adplug/emuopl.h"
#include "adplug/u6m.h"

#include "SongAdPlug.h"
#include "SoundManager.h"

SongAdPlug::SongAdPlug(Audio::Mixer *m, CEmuopl *o) {
 mixer = m;
 opl = o;
 samples_left = 0;
 stream = NULL;
}

SongAdPlug::~SongAdPlug() {
 //delete player;
}

bool SongAdPlug::Init(const char *filename, uint16 song_num) {
    if(filename == NULL)
      return false;

    m_Filename = filename; // SB-X

    stream = new U6AdPlugDecoderStream(opl, string(filename), song_num);

    return true;
}

bool SongAdPlug::Play(bool looping) {
    if(stream)
    {
    	mixer->playStream(Audio::Mixer::kMusicSoundType, &handle, stream, -1, Audio::Mixer::kMaxChannelVolume, 0, DisposeAfterUse::NO);
    }
	return true;
}

bool SongAdPlug::Stop() {

	mixer->stopHandle(handle);
	stream->rewind();
    return true;
}

bool SongAdPlug::SetVolume(uint8 volume)
{
	mixer->setChannelVolume(handle, volume);
	return true;
}

/*
bool SongAdPlug::Pause() {
	if (!Mix_PlayingMusic()) return false;
	return true;
}

bool SongAdPlug::Resume() {
	if (Mix_PlayingMusic()) return false;
	return true;
}
*/
