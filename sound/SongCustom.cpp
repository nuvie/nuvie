/*
 *  SongCustom.cpp
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
#include "decoder/wave/stdiostream.h"

#include "SongCustom.h"
#include "SoundManager.h"

SongCustom::SongCustom(Audio::Mixer *m) : mixer(m) {
 samples_left = 0;
 stream = NULL;
}

SongCustom::~SongCustom() {
 //delete player;
}

bool SongCustom::Init(const char *filename) {
    if(filename == NULL)
      return false;

    m_Filename = filename; // SB-X

	Common::SeekableReadStream *readStream = StdioStream::makeFromPath(filename);
	if(readStream == NULL)
	{
		DEBUG(0, LEVEL_ERROR, "Failed to open '%s'", filename);
		return false;
	}

	stream = Audio::makeWAVStream(readStream, DisposeAfterUse::YES);

    return true;
}

bool SongCustom::Play(bool looping) {

	if(looping)
	{
		Audio::LoopingAudioStream *looping_stream = new Audio::LoopingAudioStream(stream, 0);
		mixer->playStream(Audio::Mixer::kPlainSoundType, &handle, looping_stream, -1, Audio::Mixer::kMaxChannelVolume, 0, DisposeAfterUse::NO);
	}
	else
	{
		mixer->playStream(Audio::Mixer::kPlainSoundType, &handle, (Audio::AudioStream *) stream, -1, Audio::Mixer::kMaxChannelVolume, 0, DisposeAfterUse::NO);
	}
	return true;
}

bool SongCustom::Stop() {

	mixer->stopHandle(handle);
	stream->rewind();
    return true;
}

bool SongCustom::SetVolume(uint8 volume)
{
	mixer->setChannelVolume(handle, volume);
	return true;
}

/*
bool SongCustom::Pause() {
	if (!Mix_PlayingMusic()) return false;
	return true;
}

bool SongCustom::Resume() {
	if (Mix_PlayingMusic()) return false;
	return true;
}
*/
