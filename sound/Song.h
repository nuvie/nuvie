/*
 *  Song.h
 *  Nuvie
 *
 *  Created by Adrian Boeing on Wed Jan 21 2004.
 *  Copyright (c) 2003. All rights reserved.
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
#ifndef SONG_H
#define SONG_H

#include "Sound.h"

class Song : public Sound {
public:
	Song();
	~Song();
	bool Init(const char *filename);
	bool Play(bool looping = false);
	bool Stop();
	bool FadeOut(float seconds);
	bool SetVolume(float volume) {return false;}; //range 0..1
private:
	Mix_Music *m_pMusic;
};

#endif
