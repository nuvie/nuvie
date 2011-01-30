/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/backends/mixer/sdl/sdl-mixer.h $
 * $Id: sdl-mixer.h 54584 2010-11-29 18:48:43Z lordhoto $
 *
 */

#ifndef BACKENDS_MIXER_SDL_H
#define BACKENDS_MIXER_SDL_H

//#include "backends/platform/sdl/sdl-sys.h"
#include "mixer_intern.h"

/**
 * SDL mixer manager. It wraps the actual implementation
 * of the Audio:Mixer used by the engine, and setups
 * the SDL audio subsystem and the callback for the
 * audio mixer implementation.
 */
class SdlMixerManager {
public:
	SdlMixerManager();
	virtual ~SdlMixerManager();

	/**
	 * Initialize and setups the mixer
	 */
	virtual void init();

	/**
	 * Get the audio mixer implementation
	 */
	Audio::Mixer *getMixer() { return (Audio::Mixer *)_mixer; }

	// Used by LinuxMoto Port

	/**
	 * Pauses the audio system
	 */
	virtual void suspendAudio();

	/**
	 * Resumes the audio system
	 */
	virtual int resumeAudio();

protected:
	/** The mixer implementation */
	Audio::MixerImpl *_mixer;

	/**
	 * The obtained audio specification after opening the
	 * audio system.
	 */
	SDL_AudioSpec _obtainedRate;

	/** State of the audio system */
	bool _audioSuspended;

	/**
	 * Returns the desired audio specification 
	 */
	virtual SDL_AudioSpec getAudioSpec(uint32 rate);

	/**
	 * Starts SDL audio
	 */
	virtual void startAudio();

	/**
	 * Handles the audio callback
	 */
	virtual void callbackHandler(uint8 *samples, int len);

	/**
	 * The mixer callback entry point. Static functions can't be overrided
	 * by subclasses, so it invokes the non-static function callbackHandler()
	 */
	static void sdlCallback(void *this_, uint8 *samples, int len);
};

#endif
