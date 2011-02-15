#ifndef __PCSpeakerStream_h__
#define __PCSpeakerStream_h__
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

#include "audiostream.h"
#include "decoder/PCSpeaker.h"

class PCSpeakerStream : public Audio::RewindableAudioStream
{
public:
	PCSpeakerStream()
	{
		pcspkr = new PCSpeaker(SPKR_OUTPUT_RATE);
		finished = false;
	}

	~PCSpeakerStream() { }

	/** Is this a stereo stream? */
	bool isStereo() const { return false; }

	/** Sample rate of the stream. */
	int getRate() const { return SPKR_OUTPUT_RATE; }

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	bool endOfData() const { return finished; }

	bool rewind() { return false; }

protected:
	PCSpeaker *pcspkr;
	bool finished;
};


class PCSpeakerFreqStream : public PCSpeakerStream
{
public:
	PCSpeakerFreqStream()
	{

	}

	PCSpeakerFreqStream(uint32 start, uint16 d);
	~PCSpeakerFreqStream();

	int readBuffer(sint16 *buffer, const int numSamples);

protected:

	uint32 frequency;
	uint16 duration;
	uint32 total_samples_played;

};

class PCSpeakerSweepFreqStream : public PCSpeakerStream
{
public:
	PCSpeakerSweepFreqStream()
	{

	}

	PCSpeakerSweepFreqStream(uint32 start, uint32 end, uint16 d, uint16 s);
	~PCSpeakerSweepFreqStream();

	int readBuffer(sint16 *buffer, const int numSamples);

protected:

	uint32 start_freq;
	uint32 finish_freq;
	uint32 cur_freq;
	uint16 duration;
	uint16 stepping;

	uint32 freq_step;
	uint32 samples_per_step;
	uint32 sample_pos;
	uint32 total_samples_played;
	uint32 num_steps;
	uint32 cur_step;
    
};
#endif /* __PCSpeakerStream_h__ */
