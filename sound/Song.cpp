/*
 *  Song.cpp
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

//Mix_HookMusicFinished

#include "Song.h"


Song::Song() {
	m_Filename="";
	m_pMusic=NULL;
//	m_Paused=false;
}

Song::~Song() {
	if (m_pMusic!=NULL) {
		Mix_HaltMusic();
		Mix_FreeMusic(m_pMusic);
		m_pMusic=NULL;
	}
}

bool Song::Init(const char *filename) {
	if (filename==NULL) return false;
	m_Filename=filename;
	m_pMusic=Mix_LoadMUS(filename);
	if (m_pMusic == NULL) return false;
	return true;
}

bool Song::Play(bool looping) {
	int ret;
	if (m_pMusic==NULL) return false;
	ret=Mix_PlayMusic(m_pMusic, looping ? -1 : 0);	
	if (ret) return false;
	return true;
}

bool Song::Stop() {
	int ret;
	if (m_pMusic==NULL) return false;
	ret=Mix_HaltMusic();
	if (ret) return false;
	return true;
}

/*
bool Song::Pause() {
	if (m_pMusic==NULL) return false;
	Mix_PauseMusic();
	m_Paused=true;
	return true;
}

bool Song::Resume() {
	if (m_pMusic==NULL) return false;
	Mix_ResumeMusic();
	m_Paused=false;
	return true;
}
*/

bool Song::FadeOut(float seconds) {
	int ret;
	if (m_pMusic==NULL) return false;
	ret=Mix_FadeOutMusic((int)(seconds*1000.0f));
	if (ret) return false;
	return true;
}
