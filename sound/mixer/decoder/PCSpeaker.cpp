/*
 *  PCSpeaker.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Feb 13 2011.
 *  Copyright (c) 2011. All rights reserved.
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
#include "SDL.h"
#include "nuvieDefs.h"
#include "mixer.h"
#include "decoder/pcspeaker.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif


#define SPKR_VOLUME 5000
//#define SPKR_SHIFT 8
#define SPKR_SPEED (float)((SPKR_VOLUME*2)/0.070f)

#define PIT_TICK_RATE 1193182

void PCSpeaker::SetOn()
{
	//PCSPEAKER_SetType(3);
}

void PCSpeaker::SetOff()
{
	//PCSPEAKER_SetType(0);
}

void PCSpeaker::SetFrequency(uint16 freq)
{
	//PCSPEAKER_SetCounter(PIT_TICK_RATE/freq, PIT_MODE_3_SQUAREWAVE);
	//DEBUG(0, LEVEL_DEBUGGING, "osc_samples = %d osc_length = %d new_freq = %d", osc_samples, osc_length, freq);
	frequency = freq;
	osc_length = rate / frequency;

	osc_samples = 0;
}

PCSpeaker::PCSpeaker(uint32 mixer_rate)
{
	rate = mixer_rate;
}


void PCSpeaker::PCSPEAKER_CallBack(sint16 *stream, const uint32 len)
{
	uint32 i;

	for(i = 0;i< len;i++)
	{
		stream[i] = ((osc_samples < (osc_length / 2)) ? 127 : -128) * 64;
		if (osc_samples++ >= osc_length-1)
			osc_samples = 0;
	}

	return;
}


