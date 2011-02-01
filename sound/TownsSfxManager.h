/*
 *  TownsSfxManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Feb 1 2011.
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

#ifndef TOWNSSFXMANAGER_H
#define TOWNSSFXMANAGER_H

#include "nuvieDefs.h"
#include "Configuration.h"
#include "mixer.h"
#include "decoder/FMtownsDecoderStream.h"
#include "SfxManager.h"

#define TOWNS_SFX_SOUNDS1_SIZE 13

typedef struct
{
	unsigned char *buf;
	uint32 len;
} TownsSampleData;

class TownsSfxManager : public SfxManager
{
 public:
	TownsSfxManager(Configuration *cfg, Audio::Mixer *m);
	~TownsSfxManager() {}


 bool playSfx(uint16 sfx_id);

 private:

 TownsSampleData sounds1_dat[TOWNS_SFX_SOUNDS1_SIZE];

 void loadSound1Dat();
 void playSounds1Sample(uint8 sample_num);
 void playSounds2Sample(uint8 sample_num);

};

#endif
