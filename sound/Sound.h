/*
 *  Sound.h
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
#ifndef SOUND_H
#define SOUND_H

//disable some annoying MSC warnings
#if defined(_MSC_VER)
#pragma warning( disable : 4503 ) // warning: decorated name length exceeded
#if _MSC_VER <= 1200
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#endif
#endif

#include "SDL.h"

#include <string>
#include <stdlib.h>
#include <list>
#include <vector>
#include <map>
#include <iterator>
using std::string;
using std::list;
using std::iterator;
using std::map;
using std::vector;

class Sound {
public:
	virtual ~Sound() {};
	virtual bool Play(bool looping = false) = 0;
	virtual bool Stop() = 0;
	virtual bool FadeOut(float seconds) = 0;
	virtual bool SetVolume(uint8 volume) = 0; //range 0..255
	string GetName() {return m_Filename;}
protected:
	string m_Filename;
//	static SoundManager *gpSM;
};

class SoundCollection {
public:
	Sound *Select() {
		int i=NUVIE_RAND()  % m_Sounds.size();
		return m_Sounds[i];
	}; //randomly select one from the list
	vector<Sound *> m_Sounds;
};

#endif //SOUND_H
