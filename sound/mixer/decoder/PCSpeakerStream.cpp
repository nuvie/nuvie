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

#include <math.h>
#include "nuvieDefs.h"

#include "PCSpeakerStream.h"


PCSpeakerFreqStream::PCSpeakerFreqStream(uint32 freq, uint16 d)
{
	frequency = freq;

	duration = d * (SPKR_OUTPUT_RATE / 1255);
	pcspkr->SetOn();
	pcspkr->SetFrequency(frequency);


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
	{
		finished = true;
		pcspkr->SetOff();
	}

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
	samples_per_step = (float)s * (SPKR_OUTPUT_RATE * 0.000879533f); //(2 * (uint32)(SPKR_OUTPUT_RATE / start_freq)); //duration / num_steps;
	sample_pos = 0.0f;
	pcspkr->SetOn();
	pcspkr->SetFrequency(start_freq);


	total_samples_played = 0;
	cur_step = 0;
	DEBUG(0, LEVEL_DEBUGGING, "num_steps = %d freq_step = %d samples_per_step = %f\n", num_steps, freq_step, samples_per_step);
}


PCSpeakerSweepFreqStream::~PCSpeakerSweepFreqStream()
{

}

int PCSpeakerSweepFreqStream::readBuffer(sint16 *buffer, const int numSamples)
{
	uint32 samples = (uint32)numSamples;
	uint32 i;
	//if(total_samples_played >= duration)
	//	return 0;

	//if(total_samples_played + samples > duration)
	//	samples = duration - total_samples_played;

	for(i = 0;i < samples && cur_step < num_steps;)
	{
		//DEBUG(0, LEVEL_DEBUGGING, "sample_pos = %f\n", sample_pos);
		float n = samples_per_step - sample_pos;
		if((float)i + n > (float)samples)
			n = (float)(samples - i);

		float remainder = n - floor(n);
		n = floor(n);
		pcspkr->PCSPEAKER_CallBack(&buffer[i], (uint32)n);
		sample_pos += n;

		i += (uint32)n;
		//DEBUG(0, LEVEL_DEBUGGING, "sample_pos = %f remainder = %f\n", sample_pos, remainder);
		if(sample_pos + remainder >= samples_per_step)
		{
			cur_freq += freq_step;

			pcspkr->SetFrequency(cur_freq, remainder);

			if(remainder != 0.0f)
			{
				sample_pos = 1.0f - remainder;
				pcspkr->PCSPEAKER_CallBack(&buffer[i], 1);
				i++;
			}
			else
			{
				sample_pos = 0;
			}

			cur_step++;
		}

	}

	total_samples_played += i;

	if(cur_step >= num_steps) //total_samples_played >= duration)
	{
		DEBUG(0, LEVEL_DEBUGGING, "total_samples_played = %d cur_freq = %d\n", total_samples_played, cur_freq);
		finished = true;
		pcspkr->SetOff();
	}

	return i;
}


//**************** PCSpeakerRandomStream

PCSpeakerRandomStream::PCSpeakerRandomStream(uint32 freq, uint16 d, uint16 s)
{
	rand_value = 0x7664;
	base_val = freq;
	/*
	frequency = freq;

	duration = d * (SPKR_OUTPUT_RATE / 1255);

	pcspkr->SetFrequency(frequency);
	pcspkr->SetOn();

	total_samples_played = 0;
	*/

	pcspkr->SetOn();
	pcspkr->SetFrequency(getNextFreqValue());

	cur_step = 0;
	sample_pos = 0;
	num_steps = d / s;
	samples_per_step = s * (SPKR_OUTPUT_RATE / 20 / 800); //1255);
	total_samples_played = 0;
	DEBUG(0, LEVEL_DEBUGGING, "num_steps = %d samples_per_step = %d\n", num_steps, samples_per_step);

}


PCSpeakerRandomStream::~PCSpeakerRandomStream()
{

}

uint16 PCSpeakerRandomStream::getNextFreqValue()
{
	rand_value += 0x9248;
	rand_value = rand_value & 0xffff; //clamp_max(rand_value, 65535);
	uint16 bits = rand_value & 0x7;
	rand_value = (rand_value >> 3) + (bits << 13); //rotate rand_value right (ror) by 3 bits
	rand_value = rand_value ^ 0x9248;
	rand_value += 0x11;
	rand_value = rand_value & 0xffff; //clamp_max(rand_value, 65535);

	uint16 freq = base_val - 0x64 + 1;
	uint16 tmp = rand_value;
	freq = tmp - floor(tmp / freq) * freq;
	freq += 0x64;

	return freq;
}

int PCSpeakerRandomStream::readBuffer(sint16 *buffer, const int numSamples)
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
//		DEBUG(0, LEVEL_DEBUGGING, "n = %d\n", n);
		if(sample_pos >= samples_per_step)
		{
			//DEBUG(0, LEVEL_DEBUGGING, "samples_per_step = %d period = %d\n", samples_per_step, period);
			pcspkr->SetFrequency(getNextFreqValue());
			sample_pos = 0;
			cur_step++;
		}

		s += n;

	}

	total_samples_played += s;

	if(cur_step >= num_steps) //total_samples_played >= duration)
	{
		DEBUG(0, LEVEL_DEBUGGING, "total_samples_played = %d\n", total_samples_played);
		finished = true;
		pcspkr->SetOff();
	}

	return s;
}



