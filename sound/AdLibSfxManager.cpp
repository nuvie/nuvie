/*
 *  AdLibSfxManager.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun May 26 2013.
 *  Copyright (c) 2013. All rights reserved.
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

#include <string>
#include <map>

#include "nuvieDefs.h"
#include "mixer.h"
//#include "decoder/AdLibStream.h"
#include "AdLibSfxManager.h"


AdLibSfxManager::AdLibSfxManager(Configuration *cfg, Audio::Mixer *m) : SfxManager(cfg, m)
{

}

AdLibSfxManager::~AdLibSfxManager()
{

}

bool AdLibSfxManager::playSfx(SfxIdType sfx_id, uint8 volume)
{
	return playSfxLooping(sfx_id, NULL, volume);
}


bool AdLibSfxManager::playSfxLooping(SfxIdType sfx_id, Audio::SoundHandle *handle, uint8 volume)
{
	Audio::AudioStream *stream = NULL;

	if(sfx_id == NUVIE_SFX_HIT)
		{
			//FIXME
		}
	else if(sfx_id == NUVIE_SFX_EXPLOSION)
		{
			//FIXME
		}


	if(stream)
	{
		sfx_duration = stream->getLengthInMsec();
		playSoundSample(stream, handle, volume);
		return true;
	}

	return false;
}

void AdLibSfxManager::playSoundSample(Audio::AudioStream *stream, Audio::SoundHandle *looping_handle, uint8 volume)
{
	Audio::SoundHandle handle;

	if(looping_handle)
	{
		Audio::LoopingAudioStream *looping_stream = new Audio::LoopingAudioStream((Audio::RewindableAudioStream *)stream, 0);
		mixer->playStream(Audio::Mixer::kPlainSoundType, looping_handle, looping_stream, -1, volume);
	}
	else
	{
		mixer->playStream(Audio::Mixer::kPlainSoundType, &handle, stream, -1, volume);
	}

}
