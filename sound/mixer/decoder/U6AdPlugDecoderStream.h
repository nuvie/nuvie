#ifndef __U6AdPlugDecoderStream_h__
#define __U6AdPlugDecoderStream_h__
/* Created by Eric Fry 
 * Copyright (C) 2011 The Nuvie Team
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
#include "audiostream.h"
#include "emuopl.h"
#include "u6m.h"
#include "mid.h"

class U6Lib_n;
class U6Lzw;
class NuvieIOBuffer;


using std::string;

class U6AdPlugDecoderStream : public Audio::RewindableAudioStream
{
public:
	U6AdPlugDecoderStream()
	{
	opl = NULL; player = NULL; player_refresh_count = 0;
	}

	U6AdPlugDecoderStream(CEmuopl *o, std::string filename, uint16 song_num);
	~U6AdPlugDecoderStream();

	int readBuffer(sint16 *buffer, const int numSamples);

	/** Is this a stereo stream? */
	bool isStereo() const { return true; }

	/** Sample rate of the stream. */
	int getRate() const { return opl->getRate(); }

	bool rewind() { if(player) { player->rewind(); return true; } return false; } //FIXME this would need to be locked if called outside mixer thread.

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	bool endOfData() const { return false; }
private:
	void update_opl(short *data, int num_samples);
protected:

	uint16 samples_left;
	CEmuopl *opl;
	CPlayer *player;
	int player_refresh_count;
	int interrupt_rate;
	int interrupt_samples_left;
	bool is_midi_track;
};

#endif /* __U6AdPlugDecoderStream_h__ */
