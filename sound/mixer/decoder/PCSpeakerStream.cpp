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

#include "nuvieDefs.h"

#include "PCSpeakerStream.h"


PCSpeakerFreqStream::PCSpeakerFreqStream(uint32 freq, uint16 d)
{
	frequency = freq;

	duration = d * (SPKR_OUTPUT_RATE / 1255);

	pcspkr->SetFrequency(frequency);
	pcspkr->SetOn();

	total_samples_played = 0;
}


PCSpeakerFreqStream::~PCSpeakerFreqStream()
{

}

int PCSpeakerFreqStream::readBuffer(sint16 *buffer, const int numSamples)
{
	uint32 samples = (uint32)numSamples;

	if(total_samples_played >= duration)
		return 0;

	if(total_samples_played + samples > duration)
		samples = duration - total_samples_played;

	pcspkr->PCSPEAKER_CallBack(buffer, samples);

	total_samples_played += samples;

	if(total_samples_played >= duration)
		finished = true;

	return samples;
}

//******** PCSpeakerSweepFreqStream

PCSpeakerSweepFreqStream::PCSpeakerSweepFreqStream(uint32 start, uint32 end, uint16 d, uint16 s)
{
	start_freq = start;
	finish_freq = end;
	cur_freq = start_freq;

	num_steps = d / s;
	freq_step = ((finish_freq - start_freq) * s) / d;
	stepping = s;
	duration = d * (SPKR_OUTPUT_RATE / 1255);
	samples_per_step = (2 * (uint32)(SPKR_OUTPUT_RATE / start_freq)); //duration / num_steps;
	sample_pos = 0;
	pcspkr->SetFrequency(start_freq);
	pcspkr->SetOn();

	total_samples_played = 0;
	cur_step = 0;
	DEBUG(0, LEVEL_DEBUGGING, "num_steps = %d freq_step = %d\n", num_steps, freq_step);
}


PCSpeakerSweepFreqStream::~PCSpeakerSweepFreqStream()
{

}

int PCSpeakerSweepFreqStream::readBuffer(sint16 *buffer, const int numSamples)
{
	uint32 samples = (uint32)numSamples;
	uint32 s = 0;
	//if(total_samples_played >= duration)
	//	return 0;

	//if(total_samples_played + samples > duration)
	//	samples = duration - total_samples_played;

	for(uint32 i=0;i < samples && cur_step <= num_steps;)
	{
		uint32 n = samples_per_step - sample_pos;
		if(i + n > samples)
			n = samples - i;

		pcspkr->PCSPEAKER_CallBack(&buffer[i], n);
		sample_pos += n;
		i += n;
		DEBUG(0, LEVEL_DEBUGGING, "n = %d\n", n);
		if(sample_pos >= samples_per_step)
		{
			cur_freq += freq_step;
			uint32 period = (uint32)(SPKR_OUTPUT_RATE / (uint16)cur_freq);
			//samples_per_step = ((2+(cur_step%2)) * (uint32)(SPKR_OUTPUT_RATE / (uint16)cur_freq));
			samples_per_step = stepping * (SPKR_OUTPUT_RATE / 1255);
			samples_per_step += period - (samples_per_step % period);
			//samples_per_step -= (samples_per_step % period);
			DEBUG(0, LEVEL_DEBUGGING, "samples_per_step = %d period = %d\n", samples_per_step, period);
			pcspkr->SetFrequency(cur_freq);
			sample_pos = 0;
			cur_step++;
		}

		s += n;

	}

	total_samples_played += s;

	if(cur_step >= num_steps) //total_samples_played >= duration)
	{
		DEBUG(0, LEVEL_DEBUGGING, "total_samples_played = %d cur_freq = %d\n", total_samples_played, cur_freq);
		finished = true;
	}

	return s;
}
