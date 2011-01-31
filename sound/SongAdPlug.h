/*
 *  SongAdPlug.h
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
#ifndef __SongAdPlug_h__
#define __SongAdPlug_h__

#include "mixer.h"
#include "Song.h"
#include "decoder/U6AdPlugDecoderStream.h"

class CEmuopl;

class SongAdPlug : public Song {
public:
    uint16 samples_left;

	SongAdPlug(Audio::Mixer *m, CEmuopl *o);
	~SongAdPlug();
	bool Init(const char *filename);
	bool Play(bool looping = false);
	bool Stop();
    CEmuopl *get_opl() { return opl; };

private:
    Audio::Mixer *mixer;
    CEmuopl *opl;
    U6AdPlugDecoderStream *stream;
    Audio::SoundHandle handle;


};

#endif /* __SongAdPlug_h__ */
