/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2008 Simon Peter, <dn.tlp@gmx.net>, et al.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * MIDI & MIDI-like file player - Last Update: 10/15/2005
 *                  by Phil Hassey - www.imitationpickles.org
 *                                   philhassey@hotmail.com
 *
 * Can play the following
 *      .LAA - a raw save of a Lucas Arts Adlib music
 *             or
 *             a raw save of a LucasFilm Adlib music
 *      .MID - a "midi" save of a Lucas Arts Adlib music
 *           - or general MIDI files
 *      .CMF - Creative Music Format
 *      .SCI - the sierra "midi" format.
 *             Files must be in the form
 *             xxxNAME.sci
 *             So that the loader can load the right patch file:
 *             xxxPATCH.003  (patch.003 must be saved from the
 *                            sierra resource from each game.)
 *
 * 6/2/2000:  v1.0 relased by phil hassey
 *      Status:  LAA is almost perfect
 *                      - some volumes are a bit off (intrument too quiet)
 *               MID is fine (who wants to listen to MIDI vid adlib anyway)
 *               CMF is okay (still needs the adlib rythm mode implemented
 *                            for real)
 * 6/6/2000:
 *      Status:  SCI:  there are two SCI formats, orginal and advanced.
 *                    original:  (Found in SCI/EGA Sierra Adventures)
 *                               played almost perfectly, I believe
 *                               there is one mistake in the instrument
 *                               loader that causes some sounds to
 *                               not be quite right.  Most sounds are fine.
 *                    advanced:  (Found in SCI/VGA Sierra Adventures)
 *                               These are multi-track files.  (Thus the
 *                               player had to be modified to work with
 *                               them.)  This works fine.
 *                               There are also multiple tunes in each file.
 *                               I think some of them are supposed to be
 *                               played at the same time, but I'm not sure
 *                               when.
 * 8/16/2000:
 *      Status:  LAA: now EGA and VGA lucas games work pretty well
 *
 * 10/15/2005: Changes by Simon Peter
 *	Added rhythm mode support for CMF format.
 *
 * 09/13/2008: Changes by Adam Nielsen (malvineous@shikadi.net)
 *      Fixed a couple of CMF rhythm mode bugs
 *      Disabled note velocity for CMF files
 *      Added support for nonstandard CMF AM+VIB controller (for VGFM CMFs)
 *
 * Other acknowledgements:
 *  Allegro - for the midi instruments and the midi volume table
 *  SCUMM Revisited - for getting the .LAA / .MIDs out of those
 *                    LucasArts files.
 *  FreeSCI - for some information on the sci music files
 *  SD - the SCI Decoder (to get all .sci out of the Sierra files)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "nuvieDefs.h"
#include "U6misc.h"
#include "U6Lib_n.h"
#include "Game.h"
#include "mid.h"
#include "mididata.h"

//#define TESTING
#ifdef TESTING
#define midiprintf printf
#else
void CmidPlayer::midiprintf(const char *format, ...)
    {
    }
#endif

#define LUCAS_STYLE   1
#define CMF_STYLE     2
#define MIDI_STYLE    4
#define SIERRA_STYLE  8

// AdLib melodic and rhythm mode defines
#define ADLIB_MELODIC	1
#define ADLIB_RYTHM	0

// File types
#define FILE_LUCAS      1
#define FILE_MIDI       2
#define FILE_CMF        3
#define FILE_SIERRA     4
#define FILE_ADVSIERRA  5
#define FILE_OLDLUCAS   6

// AdLib standard operator table
const unsigned char CmidPlayer::adlib_opadd[] = {0x00  ,0x01 ,0x02  ,0x08  ,0x09  ,0x0A  ,0x10 ,0x11  ,0x12};

// map CMF drum channels 12 - 15 to corresponding AdLib drum operators
// bass drum (channel 11) not mapped, cause it's handled like a normal instrument
const int CmidPlayer::map_chan[] = { 0x14, 0x12, 0x15, 0x11 };

// Standard AdLib frequency table
const int CmidPlayer::fnums[] = { 0x16b,0x181,0x198,0x1b0,0x1ca,0x1e5,0x202,0x220,0x241,0x263,0x287,0x2ae };

// Map CMF drum channels 11 - 15 to corresponding AdLib drum channels
const int CmidPlayer::percussion_map[] = { 6, 7, 8, 8, 7 };

const uint8 adlib_BD_cmd_tbl[] ={ 0, 1, 0, 1, 0, 1, 16, 8, 4, 2, 1 };

CPlayer *CmidPlayer::factory(Copl *newopl)
{
  return new CmidPlayer(newopl);
}

CmidPlayer::CmidPlayer(Copl *newopl)
  : CPlayer(newopl), author(&emptystr), title(&emptystr), remarks(&emptystr),
    emptystr('\0'), flen(0), data(0)
{
	const uint8 byte_73_init[] = {1, 2, 3, 4, 5, 6, 7, 8, 0xB, 0xFF, 0xFF, 0, 0xC};

	adlib_tim_data = NULL;
	adlib_num_active_channels = 9;
	memset(midi_chan_tim_ptr, 0, sizeof(midi_chan_tim_ptr));
	memset(midi_chan_pitch, 0, sizeof(midi_chan_pitch));
	memset(midi_chan_tim_off_10, 0, sizeof(midi_chan_tim_off_10));
	memset(midi_chan_tim_off_11, 0, sizeof(midi_chan_tim_off_11));

	adlib_bd_status = 0;
	memcpy(byte_73, byte_73_init, sizeof(byte_73_init));

	for(int i=0;i<29;i++)
	{
		midi_chan_volume[i] = 0x100;
	}

	memset(adlib_ins, 0, sizeof(adlib_ins));

	for(int i=0;i<11;i++)
	{
		adlib_ins[i].note = -1;
		adlib_ins[i].channel = -1;
		adlib_ins[i].byte_68 = 1;
	}

}

CmidPlayer::~CmidPlayer()
{
	if(data)
		delete [] data;

	if(adlib_tim_data)
		delete [] adlib_tim_data;
}

unsigned char CmidPlayer::datalook(long pos)
{
    if (pos<0 || pos >= flen) return(0);
    return(data[pos]);
}

unsigned long CmidPlayer::getnexti(unsigned long num)
{
	unsigned long v=0;
	unsigned long i;

    for (i=0; i<num; i++)
        {
        v+=(datalook(pos)<<(8*i)); pos++;
        }
    return(v);
}

unsigned long CmidPlayer::getnext(unsigned long num)
{
	unsigned long v=0;
	unsigned long i;

    for (i=0; i<num; i++)
        {
        v<<=8;
        v+=datalook(pos); pos++;
        }
    return(v);
}

unsigned long CmidPlayer::getval()
{
    int v=0;
	unsigned char b;

    b=(unsigned char)getnext(1);
	v=b&0x7f;
	while ((b&0x80) !=0)
		{
        b=(unsigned char)getnext(1);
        v = (v << 7) + (b & 0x7F);
		}
	return(v);
}

void CmidPlayer::sierra_next_section()
{
    int i,j;

    for (i=0; i<16; i++)
        track[i].on=0;

    midiprintf("\n\nnext adv sierra section:\n");

    pos=sierra_pos;
    i=0;j=0;
    while (i!=0xff)
       {
       getnext(1);
       curtrack=j; j++;
       track[curtrack].on=1;
	   track[curtrack].spos = getnext(1);
	   track[curtrack].spos += (getnext(1) << 8) + 4;	//4 best usually +3? not 0,1,2 or 5
//       track[curtrack].spos=getnext(1)+(getnext(1)<<8)+4;		// dynamite!: doesn't optimize correctly!!
       track[curtrack].tend=flen; //0xFC will kill it
       track[curtrack].iwait=0;
       track[curtrack].pv=0;
       midiprintf ("track %d starts at %lx\n",curtrack,track[curtrack].spos);

       getnext(2);
       i=getnext(1);
       }
    getnext(2);
    deltas=0x20;
    sierra_pos=pos;
    //getch();

    fwait=0;
    doing=1;
}

bool CmidPlayer::load(const std::string &filename)
{
	return false;
}

bool CmidPlayer::load(std::string &filename, int song_index)
{
	U6Lib_n f;
	f.open(filename, 4, NUVIE_GAME_MD);
    //binistream *f = fp.open(filename); if(!f) return false;
    int good;

    flen = f.get_item_size(song_index);
    data = new unsigned char [flen];
    f.get_item(song_index, data);
    //f->readString((char *)data, flen);

    //f->readString((char *)s, 6);

    good=0;
    subsongs=0;
    switch(data[0])
        {
        case 'A':
            if (data[1]=='D' && data[2]=='L') good=FILE_LUCAS;
            break;
        case 'M':
            if (data[1]=='T' && data[2]=='h' && data[3]=='d') good=FILE_MIDI;
            break;
        case 'C':
            if (data[1]=='T' && data[2]=='M' && data[3]=='F') good=FILE_CMF;
            break;
            /*
        case 0x84:
	  if (s[1]==0x00 && load_sierra_ins(filename, fp)) {
	    if (s[2]==0xf0)
	      good=FILE_ADVSIERRA;
	    else
	      good=FILE_SIERRA;

	  }*/
	  break;
        default:
            if (data[4]=='A' && data[5]=='D') good=FILE_OLDLUCAS;
            break;
        }

    if (good!=0)
		subsongs=1;
    else {
      delete data;
      data = NULL;
      return false;
    }

    type=good;
    //f->seek(0);

    load_tim_file();

    rewind(0);
    return true;
}

void CmidPlayer::load_tim_file()
{
	U6Lib_n f;
	std::string filename;
	Configuration *config = Game::get_game()->get_config();
	nuvie_game_t game_type = get_game_type(config);
	if(game_type == NUVIE_GAME_SE)
	{
		config_get_path(config,"savage.tim",filename);
	}
	else // game_type == NUVIE_GAME_MD
	{
		config_get_path(config,"md.tim",filename);
	}

	f.open(filename, 4, game_type);
	unsigned char *buf = f.get_item(1);
	adlib_tim_data = new unsigned char [f.get_item_size(1)-1];
	num_tim_records = buf[0];
	memcpy(adlib_tim_data, &buf[1], f.get_item_size(1)-1);
	free(buf);

	for(int i=0;i<32;i++)
	{
		midi_chan_tim_ptr[i] = adlib_tim_data;
	}

	program_change(0x9 ,0x80);
	program_change(0xa ,0x72);
	program_change(0xb ,0x83);
	program_change(0xc ,0x71);
	program_change(0xd ,0x86);
	program_change(0xe ,0x87);
	program_change(0xf ,0x85);
	program_change(0x10 ,0x84);
	program_change(0x11 ,0x81);
	program_change(0x12 ,0x88);
	program_change(0x13 ,0x8D);
	program_change(0x14 ,0x8F);
	program_change(0x15 ,0x90);
	program_change(0x16 ,0x91);
	program_change(0x17 ,0x93);
	program_change(0x18 ,0x8C);
	program_change(0x19 ,0x8B);
}

unsigned char *CmidPlayer::get_tim_data(uint8 program_number)
{
	for(int i=0;i<num_tim_records;i++)
	{
		if(adlib_tim_data[i*48+0x2f] == program_number)
		{
			return &adlib_tim_data[i*48];
		}
	}

	return NULL;
}

void CmidPlayer::program_change(sint8 channel, uint8 program_number)
{
	unsigned char *tim_data = get_tim_data(program_number);
	int i,j;

	for(i=0;i<11;i++)
	{
		if(adlib_ins[i].channel == channel)
		{
			play_note(channel, adlib_ins[i].note, 0); //note off.
			adlib_ins[i].channel = -1;
			adlib_ins[i].tim_data = NULL;

		}
	}

	midi_chan_tim_ptr[channel] = tim_data;
	midi_chan_tim_off_10[channel] = tim_data[0x10];
	midi_chan_tim_off_11[channel] = tim_data[0x11];

	if(tim_data[0xb] != 0 && adlib_num_active_channels == 9)
	{
		midi_write_adlib(0xa6, 0);
		midi_write_adlib(0xb6, 0);
		midi_write_adlib(0xa7, 0);
		midi_write_adlib(0xb7, 0xa);
		midi_write_adlib(0xa8, 0x54);
		midi_write_adlib(0xb8, 0x9);

		adlib_num_active_channels = 6;
		for(i=6;i<9;i++)
		{
			for(j=0;j<13;j++)
			{
				if(byte_73[j] == i)
				{
					byte_73[j] = byte_73[i];
					byte_73[i] = -1;
					break;
				}
			}
		}
		adlib_bd_status = 0x20;
		midi_write_adlib(0xbd, adlib_bd_status);
	}
}

void CmidPlayer::pitch_bend(uint8 channel, uint8 pitch_lsb, uint8 pitch_msb)
{
	unsigned char *cur_tim_ptr = midi_chan_tim_ptr[channel];

	midi_chan_pitch[channel] = ((sint16)((pitch_msb << 7) + pitch_lsb + 0xe000) * cur_tim_ptr[0xe]) / 256;
	printf("pitch_bend: c=%d, pitch=%d\n", channel, midi_chan_pitch[channel]);
	for(int i=0;i<adlib_num_active_channels;i++)
	{
		if(adlib_ins[i].byte_68 > 1 && adlib_ins[i].channel == channel)
		{
			sint16 var_4 = 0;

			if(adlib_ins[i].tim_data != NULL)
			{
				var_4 = (sint16)adlib_ins[i].tim_data[0x24] + ((sint16)adlib_ins[i].tim_data[0x25] * 256);
			}

			uint16 var_2 = sub_60D(adlib_ins[i].word_3c + midi_chan_pitch[channel] + adlib_ins[i].word_cb + adlib_ins[i].word_121 + var_4);
			var_2 += 0x2000;
			midi_write_adlib(0xa0 + i, var_2 & 0xff);
			midi_write_adlib(0xb0 + i, var_2 >> 8);
		}

	}
}

void CmidPlayer::control_mode_change(uint8 channel, uint8 function, uint8 value)
{
	uint8 c = channel;
	printf("control_mode_change: c=%d, func=%2x, value=%d\n",channel, function, value);
	if(c == 9)
	{
		c++;
		do
		{
			control_mode_change(c, function, value);
			c++;
		} while(c<=25);
		c = 9;
	}

	if(function == 1)
	{
		 midi_chan_tim_off_11[channel] = ((((sint16)midi_chan_tim_ptr[channel][0xf]) * value) / 128) + (sint16)midi_chan_tim_ptr[channel][0x11];
	}
	else if(function == 7)
	{
		midi_chan_volume[c] = value + 128;
	}
	else if(function == 0x7b)
	{
		bool var_6 = false;
		for(int i=0;i<0xb;i++)
		{
			if(adlib_ins[i].byte_68 > 1)
			{
				if(adlib_ins[i].channel == channel)
				{
					play_note(channel, adlib_ins[i].note, 0); //note off
				}
				else if(i >= adlib_num_active_channels)
				{
					var_6 = true;
				}
			}
		}

		if(var_6 && adlib_num_active_channels < 9)
		{
			midi_write_adlib(0xbd, 0);
			adlib_num_active_channels = 9;
			byte_73[6] = 7;
			byte_73[7] = 8;
			byte_73[8] = byte_73[0xb];
			byte_73[0xb] = 6;
		}

	}
	else if(function == 0x79)
	{
		control_mode_change(channel, 1, 0);
		control_mode_change(channel, 7, 0x7f);
		pitch_bend(channel, 0, 0x40);
	}
}
void CmidPlayer::play_note(uint8 channel, sint8 note, uint8 velocity)
{
	unsigned char *cur_tim_ptr = midi_chan_tim_ptr[channel];
	for(;cur_tim_ptr!=NULL;cur_tim_ptr+=48)
	{
		sint8 voice = sub_4BF(channel, note, velocity, cur_tim_ptr);
		sint16 var_4 = voice;
		if(voice > 8)
		{
			var_4 = 0x11 - voice;
		}

		if(voice >= 0)
		{
			sint16 var_a = cur_tim_ptr[0x24] + (cur_tim_ptr[0x25] << 8);
			if(velocity != 0)
			{
				adlib_ins[voice].word_121 = 0;
				adlib_ins[voice].byte_137 = 0;
				adlib_ins[voice].word_cb = cur_tim_ptr[0x12] + ((sint16)cur_tim_ptr[0x13] << 8);
			}

			sint8 cl = cur_tim_ptr[0x27];
			if(cl < 0)
			{
				adlib_ins[voice].word_3c = (-((sint16)(note - 60)*256) / (1<<-(cl+1))) + 0x3c00;
			}
			else
			{
				adlib_ins[voice].word_3c = (((sint16)(note - 60)*256) / (1<<cl)) + 0x3c00;
			}

			uint16 var_2 = sub_60D(adlib_ins[voice].word_3c + midi_chan_pitch[channel] + adlib_ins[voice].word_cb + adlib_ins[voice].word_121 + var_a);
			if(velocity == 0)
			{
				if(voice < adlib_num_active_channels || voice <= 6)
				{
					midi_write_adlib(0xa0 + var_4, var_2 & 0xff);
					midi_write_adlib(0xb0 + var_4, var_2 >> 8);
				}
				else
				{
					adlib_bd_status &= ~adlib_BD_cmd_tbl[voice];
				}
			}
			else
			{
				uint16 var_6 = cur_tim_ptr[6];
				if(cur_tim_ptr[0xc] != 0 || midi_chan_volume[channel] < 0x100)
				{
					sint16 di = 0x3f - ((midi_chan_volume[channel] * (0x3f - ( ((sint16)(63 - velocity) / (sint16)(1<<(7 - cur_tim_ptr[0xc]))) + (var_6 & 0x3f)))) >> 8); //fixme this was 0x14 in dosbox var_6 = 8

					di = 63 - velocity;
					di = di / (1<<(7 - cur_tim_ptr[0xc]));
					di += var_6 & 0x3f;

					sint16 ax = (0x3f - di) * midi_chan_volume[channel];
					ax = ax / 256;
					di = 0x3f - ax;

					if(di > 0x3f)
					{
						di = 0x3f;
					}
					if(di < 0)
					{
						di = 0;
					}

					midi_write_adlib(0x40 + adlib_voice_op1(voice), (var_6 & 0xc0) + di);
				}

				var_6 = cur_tim_ptr[1];
				if(cur_tim_ptr[0xd] != 0)
				{
					sint16 di = (0x3f - velocity) / (sint16)(1<<(7 - cur_tim_ptr[0xd])) + (var_6 & 0x3f);
					if(di > 0x3f)
					{
						di = 0x3f;
					}
					if(di < 0)
					{
						di = 0;
					}

					midi_write_adlib(0x40 + adlib_voice_op(voice), (var_6 & 0xc0) + di);
				}

				if(cur_tim_ptr[0xb] == 0 || voice == 6)
				{
					if(cur_tim_ptr[0xb] == 0)
					{
						var_2 += 0x2000;
					}
					midi_write_adlib(0xa0 + var_4, var_2 & 0xff);
					midi_write_adlib(0xb0 + var_4, var_2 >> 8);
				}

				if(cur_tim_ptr[0xb] != 0)
				{
					adlib_bd_status |= adlib_BD_cmd_tbl[voice];
				}
			}

			if(cur_tim_ptr[0xb] != 0)
			{
				midi_write_adlib(0xbd, adlib_bd_status);
			}
		}

		if(cur_tim_ptr[0x26]==0)
			break;
	}
}

uint16 CmidPlayer::sub_60D(sint16 val)
{
	const uint16 word_20f[] = {0x1E5, 0x202, 0x220, 0x241, 0x263, 0x287, 0x2AE, 0x2D7, 0x302, 0x330, 0x360, 0x393, 0x3CA};

	sint16 var_2 = val / 256;

	sint16 si = ((var_2 + 6) / 0xc) - 2;
	if(si > 7)
	{
		si = 7;
	}
	if(si < 0)
	{
		si = 0;
	}

	uint16 di = word_20f[(var_2 + 6) % 0xc];
	if((val & 0xff) != 0)
	{
		di += ((word_20f[((var_2 - 18) % 0xc) + 1] - di) * (val & 0xff)) / 256;
	}

	return (si << 10) + di;
}

uint16 CmidPlayer::sub_4BF(uint8 channel, uint8 note, uint8 velocity, unsigned char *cur_tim_ptr)
{
	sint16 si = -1;

	if(adlib_num_active_channels >= 9 || cur_tim_ptr[0xb] == 0)
	{
		if(velocity == 0)
		{
			for(si=0;si<adlib_num_active_channels;si++)
			{
				if(adlib_ins[si].byte_68 > 1 && adlib_ins[si].note == note && adlib_ins[si].channel == channel && adlib_ins[si].tim_data == cur_tim_ptr)
				{
					adlib_ins[si].byte_68 = 0;
					sub_45E(si);
					sub_48E(si, 0xb);
					break;
				}
			}
			if(si == adlib_num_active_channels)
			{
				si = -1;
			}
		}
		else
		{
			if(byte_73[0xb] == 0xb)
			{
				if(midi_chan_tim_ptr[channel] == cur_tim_ptr)
				{
					si = byte_73[0xc];
					byte_73[0xc] = byte_73[si];
					sub_48E(si, 0xc);
					midi_write_adlib(0xa0+si, 0);
					midi_write_adlib(0xb0+si, 0);
				}
			}
			else
			{
				si = byte_73[0xb];
				byte_73[0xb] = byte_73[si];
				sub_48E(si, 0xc);
			}

			if(si >= 0)
			{
				adlib_ins[si].byte_68 = 2;
				adlib_ins[si].note = note;
			}
		}
	}
	else
	{
		si = cur_tim_ptr[0xb];
		adlib_bd_status &= ~adlib_BD_cmd_tbl[cur_tim_ptr[0xb]];
		midi_write_adlib(0xbd, adlib_bd_status);
	}

	if(si >= 0)
	{
		if(adlib_ins[si].channel != channel || adlib_ins[si].tim_data != cur_tim_ptr) //changing instruments
		{
			write_adlib_instrument(si, cur_tim_ptr);
			adlib_ins[si].channel = channel;
			adlib_ins[si].tim_data = cur_tim_ptr;
		}
	}

	return si;
}

void CmidPlayer::sub_45E(sint16 voice)
{
	for(int i=0;i<0xd;i++)
	{
		if(byte_73[i] == voice)
		{
			byte_73[i] = byte_73[voice];
			byte_73[voice] = voice;
		}
	}
}

void CmidPlayer::sub_48E(sint16 voice, uint8 val)
{
	for(int i=0;i<0xd;i++)
	{
		if(byte_73[i] == val)
		{
			byte_73[i] = voice;
			byte_73[voice] = val;
		}
	}
}

uint8 CmidPlayer::adlib_voice_op(sint8 voice)
{
	const uint8 opp_tbl[] = {0, 1, 2, 8, 9, 0xA, 0x10, 0x11, 0x12, 0, 1, 2, 8, 9, 0xA, 0x10, 0x14, 0x12, 0x15, 0x11};
	return opp_tbl[adlib_num_active_channels < 9 ? voice + 9 : voice];
}

uint8 CmidPlayer::adlib_voice_op1(sint8 voice)
{
	const uint8 opp1_tbl[] = {3, 4, 5, 0xB, 0xC, 0xD, 0x13, 0x14, 0x15, 3, 4, 5, 0xB, 0xC, 0xD, 0x13, 0x14, 0x12, 0x15, 0x11};
	return opp1_tbl[adlib_num_active_channels < 9 ? voice + 9 : voice];
}

void CmidPlayer::write_adlib_instrument(sint8 voice, unsigned char *tim_data)
{
	uint8 opadd = adlib_voice_op(voice);
	uint8 opadd1 = adlib_voice_op1(voice);
	unsigned char *cur_tim_ptr = tim_data;

	midi_write_adlib(0x20+opadd,*cur_tim_ptr++);
	midi_write_adlib(0x40+opadd,*cur_tim_ptr++);
	midi_write_adlib(0x60+opadd,*cur_tim_ptr++);
	midi_write_adlib(0x80+opadd,*cur_tim_ptr++);
	midi_write_adlib(0xe0+opadd,*cur_tim_ptr++);

	if(adlib_num_active_channels == 9 || tim_data[0xb] < 7)
	{
		midi_write_adlib(0x20+opadd1,*cur_tim_ptr++);
		midi_write_adlib(0x40+opadd1,*cur_tim_ptr++);
		midi_write_adlib(0x60+opadd1,*cur_tim_ptr++);
		midi_write_adlib(0x80+opadd1,*cur_tim_ptr++);
		midi_write_adlib(0xe0+opadd1,*cur_tim_ptr++);
		midi_write_adlib(0xc0+voice,*cur_tim_ptr++);
	}

}

void CmidPlayer::interrupt_vector()
{
	const uint8 byte_229[] = {24, 0, 18, 20, 22, 0, 0, 0};

	for(int i=0;i<adlib_num_active_channels;i++)
	{
		unsigned char *cur_tim_data = NULL;
		bool update_adlib = false;
		sint8 channel = adlib_ins[i].channel;
		uint8 var_8 = byte_229[adlib_ins[i].byte_68];
		sint16 var_10 = 0;
		if(adlib_ins[i].tim_data == NULL)
		{
			cur_tim_data = adlib_tim_data;
		}
		else
		{
			cur_tim_data = adlib_ins[i].tim_data;
			var_10 = cur_tim_data[0x24] & (cur_tim_data[0x25]<<8);
		}

		if(var_8 != 0)
		{
			sint16 var_a = cur_tim_data[var_8*2 - 16] & (cur_tim_data[var_8*2 - 16+1]<<8);
			sint16 var_c = cur_tim_data[(var_8+1)*2 - 16] & (cur_tim_data[(var_8+1)*2 - 16+1]<<8);

			sint16 tmp = (var_c > adlib_ins[i].word_cb) ? var_c - adlib_ins[i].word_cb : adlib_ins[i].word_cb - var_c;
			if(tmp >= var_a)
			{
				if(adlib_ins[i].word_cb >= var_c)
				{
					adlib_ins[i].word_cb -= var_a;
				}
				else
				{
					adlib_ins[i].word_cb += var_a;
				}
			}
			else
			{
				adlib_ins[i].word_cb = var_c;
				adlib_ins[i].byte_68++;
			}

			update_adlib = true;
		}

		if(channel < 32 && midi_chan_tim_off_10[channel] != 0)
		{
			adlib_ins[i].byte_137 += midi_chan_tim_off_10[channel];
			sint8 var_11 = adlib_ins[i].byte_137;
			if(var_11 > 63 || var_11 < -64)
			{
				var_11 = -128 - var_11;
			}

			adlib_ins[i].word_121 = (midi_chan_tim_off_11[channel] * var_11) / 16;
			update_adlib = true;
		}

		if(update_adlib || var_10 != 0)
		{
			uint16 adlib_cmd_data = sub_60D(adlib_ins[i].word_3c + midi_chan_pitch[channel] + adlib_ins[i].word_cb + adlib_ins[i].word_121 + var_10);
			if(adlib_ins[i].byte_68 > 1)
			{
				adlib_cmd_data += 0x2000;
			}
			midi_write_adlib(0xa0 + i, adlib_cmd_data & 0xff);
			midi_write_adlib(0xb0 + i, adlib_cmd_data >> 8);
		}
	}
}
void CmidPlayer::midi_write_adlib(unsigned int r, unsigned char v)
{
  opl->write(r,v);
  adlib_data[r]=v;
}

void CmidPlayer::midi_fm_instrument(int voice, unsigned char *inst)
{
    if ((adlib_style&SIERRA_STYLE)!=0)
        midi_write_adlib(0xbd,0);  //just gotta make sure this happens..
                                      //'cause who knows when it'll be
                                      //reset otherwise.


    midi_write_adlib(0x20+adlib_opadd[voice],inst[0]);
    midi_write_adlib(0x23+adlib_opadd[voice],inst[1]);

    if (adlib_style & LUCAS_STYLE) {
        midi_write_adlib(0x43+adlib_opadd[voice],0x3f);
        if ((inst[10] & 1)==0)
            midi_write_adlib(0x40+adlib_opadd[voice],inst[2]);
        else
            midi_write_adlib(0x40+adlib_opadd[voice],0x3f);

    } else if ((adlib_style & SIERRA_STYLE) || (adlib_style & CMF_STYLE)) {
        midi_write_adlib(0x40+adlib_opadd[voice],inst[2]);
        midi_write_adlib(0x43+adlib_opadd[voice],inst[3]);

    } else {
        midi_write_adlib(0x40+adlib_opadd[voice],inst[2]);
        if ((inst[10] & 1)==0)
            midi_write_adlib(0x43+adlib_opadd[voice],inst[3]);
        else
            midi_write_adlib(0x43+adlib_opadd[voice],0);
    }

    midi_write_adlib(0x60+adlib_opadd[voice],inst[4]);
    midi_write_adlib(0x63+adlib_opadd[voice],inst[5]);
    midi_write_adlib(0x80+adlib_opadd[voice],inst[6]);
    midi_write_adlib(0x83+adlib_opadd[voice],inst[7]);
    midi_write_adlib(0xe0+adlib_opadd[voice],inst[8]);
    midi_write_adlib(0xe3+adlib_opadd[voice],inst[9]);

    midi_write_adlib(0xc0+voice,inst[10]);
}

void CmidPlayer::midi_fm_percussion(int ch, unsigned char *inst)
{
  int	opadd = map_chan[ch - 12];

  midi_write_adlib(0x20 + opadd, inst[0]);
  midi_write_adlib(0x40 + opadd, inst[2]);
  midi_write_adlib(0x60 + opadd, inst[4]);
  midi_write_adlib(0x80 + opadd, inst[6]);
  midi_write_adlib(0xe0 + opadd, inst[8]);
  if (opadd < 0x13) // only output this for the modulator, not the carrier, as it affects the entire channel
      midi_write_adlib(0xc0 + percussion_map[ch - 11], inst[10]);
}

void CmidPlayer::midi_fm_volume(int voice, int volume)
{
    int vol;

    if ((adlib_style&SIERRA_STYLE)==0)  //sierra likes it loud!
    {
    vol=volume>>2;

    if ((adlib_style&LUCAS_STYLE)!=0)
        {
        if ((adlib_data[0xc0+voice]&1)==1)
            midi_write_adlib(0x40+adlib_opadd[voice], (unsigned char)((63-vol) |
            (adlib_data[0x40+adlib_opadd[voice]]&0xc0)));
        midi_write_adlib(0x43+adlib_opadd[voice], (unsigned char)((63-vol) |
            (adlib_data[0x43+adlib_opadd[voice]]&0xc0)));
        }
        else
        {
        if ((adlib_data[0xc0+voice]&1)==1)
            midi_write_adlib(0x40+adlib_opadd[voice], (unsigned char)((63-vol) |
            (adlib_data[0x40+adlib_opadd[voice]]&0xc0)));
        midi_write_adlib(0x43+adlib_opadd[voice], (unsigned char)((63-vol) |
           (adlib_data[0x43+adlib_opadd[voice]]&0xc0)));
        }
    }
}

void CmidPlayer::midi_fm_playnote(int voice, int note, int volume)
{
    int freq=fnums[note%12];
    int oct=note/12;
	int c;

    midi_fm_volume(voice,volume);
    midi_write_adlib(0xa0+voice,(unsigned char)(freq&0xff));

	c=((freq&0x300) >> 8)+((oct&7)<<2) + (adlib_mode == ADLIB_MELODIC || voice < 6 ? (1<<5) : 0);
    midi_write_adlib(0xb0+voice,(unsigned char)c);
}

void CmidPlayer::midi_fm_endnote(int voice)
{
    //midi_fm_volume(voice,0);
    //midi_write_adlib(0xb0+voice,0);

    midi_write_adlib(0xb0+voice,(unsigned char)(adlib_data[0xb0+voice]&(255-32)));
}

void CmidPlayer::midi_fm_reset()
{
    int i;

    opl->init();

    for (i=0; i<256; i++)
        midi_write_adlib(i,0);

    midi_write_adlib(0x01, 0x20);
    midi_write_adlib(0xBD,0xc0);
}

bool CmidPlayer::update()
{
	const uint8 adlib_chan_tbl[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10,
			10, 18, 11, 0, 12, 13, 17, 13, 16, 13, 14, 13, 13, 15,
			13, 19, 0, 0, 0, 0, 21, 0, 0, 0, 26, 26, 25, 20, 20,
			0, 0, 21, 21, 22, 23, 0, 0, 24, 0, 20, 0 };

	const uint8 adlib_note_tbl[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48,
			48, 48, 48, 0, 48, 42, 71, 42, 71, 47, 71, 47, 52, 79,
			52, 77, 0, 0, 0, 0, 71, 0, 0, 0, 72, 79, 79, 64, 58,
			0, 0, 89, 84, 48, 72, 0, 0, 36, 0, 96, 0 };

    //long w,v,note,vel,ctrl,nv,x,l,lnum;
    long w,v,note,vel,ctrl,x,l,lnum;
    int i=0,j,c;
    //int on,onl,numchan;
    int ret;

    int current_status[16];

    for(i=0;i<16;i++)
    	current_status[i] = 0;

    if (doing == 1)
        {
        // just get the first wait and ignore it :>
        for (curtrack=0; curtrack<16; curtrack++)
            if (track[curtrack].on)
                {
                pos=track[curtrack].pos;
                if (type != FILE_SIERRA && type !=FILE_ADVSIERRA)
                    track[curtrack].iwait+=getval();
                    else
                    track[curtrack].iwait+=getnext(1);
                track[curtrack].pos=pos;
                }
        doing=0;
        }

    iwait=0;
    ret=1;

    while (iwait==0 && ret==1)
        {
        for (curtrack=0; curtrack<16; curtrack++)
        if (track[curtrack].on && track[curtrack].iwait==0 &&
            track[curtrack].pos < track[curtrack].tend)
        {
        pos=track[curtrack].pos;

		v=getnext(1);

        //  This is to do implied MIDI events. aka 'Running Status'
        if (v<0x80)
        {
        	v=track[curtrack].pv;
        	printf ("Running status [%2X]\n",(unsigned int)v);
        	pos--;
        }
        else
        {
        	if(v >= 0xf0 && v < 0xf9)
        	{
        		track[curtrack].pv=0; //reset running status.
        	}
        	else if(v < 0xf0)
        	{
        		track[curtrack].pv=(unsigned char)v;
        	}
        	// if v > 0xf9 then current running status is maintained.
        }
		c=v&0x0f;
        midiprintf ("[%2X]",(unsigned int)v);
        if(v==0xfe)
        	midiprintf("pos=%d",(int)pos);
        current_status[curtrack] = v;
        switch(v&0xf0)
            {
			case 0x80: /*note off*/
				midiprintf("Trk%02d: Note Off\n",curtrack);
				note=getnext(1); vel=getnext(1);
				play_note(c, note, 0);
				/*
                for (i=0; i<9; i++)
                    if (chp[i][0]==c && chp[i][1]==note)
                        {
                        midi_fm_endnote(i);
                        chp[i][0]=-1;
                        }
                        */
                break;
            case 0x90: /*note on*/
              //  doing=0;
            	midiprintf("Trk%02d: Note On\n",curtrack);
                note=getnext(1); vel=getnext(1);
                if(c == 10)
                {
                	play_note(adlib_chan_tbl[note] - 1, adlib_note_tbl[note], vel);
                }
                else
                {
                	play_note(c, note, vel);
                }
                /*
		if(adlib_mode == ADLIB_RYTHM)
		  numchan = 6;
		else
		  numchan = 9;

                if (ch[c].on!=0)
                {
		  for (i=0; i<18; i++)
                    chp[i][2]++;

		  if(c < 11 || adlib_mode == ADLIB_MELODIC) {
		    j=0;
		    on=-1;onl=0;
		    for (i=0; i<numchan; i++)
		      if (chp[i][0]==-1 && chp[i][2]>onl)
			{ onl=chp[i][2]; on=i; j=1; }

		    if (on==-1)
		      {
			onl=0;
			for (i=0; i<numchan; i++)
			  if (chp[i][2]>onl)
			    { onl=chp[i][2]; on=i; }
		      }

		    if (j==0)
		      midi_fm_endnote(on);
		  } else
		    on = percussion_map[c - 11];

                  if (vel!=0 && ch[c].inum>=0 && ch[c].inum<128) {
                    if (adlib_mode == ADLIB_MELODIC || c < 12) // 11 == bass drum, handled like a normal instrument, on == channel 6 thanks to percussion_map[] above
		      midi_fm_instrument(on,ch[c].ins);
		    else
 		      midi_fm_percussion(c, ch[c].ins);

                    if (adlib_style & MIDI_STYLE) {
                        nv=((ch[c].vol*vel)/128);
                        if ((adlib_style&LUCAS_STYLE)!=0) nv*=2;
                        if (nv>127) nv=127;
                        nv=my_midi_fm_vol_table[nv];
                        if ((adlib_style&LUCAS_STYLE)!=0)
                            nv=(int)((float)sqrt((float)nv)*11);
                    } else if (adlib_style & CMF_STYLE) {
                        // CMF doesn't support note velocity (even though some files have them!)
                        nv = 127;
                    } else {
                        nv=vel;
                    }

		    midi_fm_playnote(on,note+ch[c].nshift,nv*2); // sets freq in rhythm mode
                    chp[on][0]=c;
                    chp[on][1]=note;
                    chp[on][2]=0;

		    if(adlib_mode == ADLIB_RYTHM && c >= 11) {
		      // Still need to turn off the perc instrument before playing it again,
		      // as not all songs send a noteoff.
		      midi_write_adlib(0xbd, adlib_data[0xbd] & ~(0x10 >> (c - 11)));
		      // Play the perc instrument
		      midi_write_adlib(0xbd, adlib_data[0xbd] | (0x10 >> (c - 11)));
		    }

                  } else {
                    if (vel==0) { //same code as end note
		        if (adlib_mode == ADLIB_RYTHM && c >= 11) {
		            // Turn off the percussion instrument
		            midi_write_adlib(0xbd, adlib_data[0xbd] & ~(0x10 >> (c - 11)));
                            //midi_fm_endnote(percussion_map[c]);
                            chp[percussion_map[c - 11]][0]=-1;
                        } else {
                            for (i=0; i<9; i++) {
                                if (chp[i][0]==c && chp[i][1]==note) {
                                    // midi_fm_volume(i,0);  // really end the note
                                    midi_fm_endnote(i);
                                    chp[i][0]=-1;
                                }
                            }
                        }
                    } else {
                        // i forget what this is for.
                        chp[on][0]=-1;
                        chp[on][2]=0;
                    }
                  }
                  midiprintf(" [%d:%d:%d:%d]\n",c,ch[c].inum,(int)note,(int)vel);
                }
                else
                midiprintf ("off");
*/
                break;
            case 0xa0: /*key after touch */
                note=getnext(1); vel=getnext(1);
                /*  //this might all be good
                for (i=0; i<9; i++)
                    if (chp[i][0]==c & chp[i][1]==note)
                        
midi_fm_playnote(i,note+cnote[c],my_midi_fm_vol_table[(cvols[c]*vel)/128]*2);
                */
                break;
            case 0xb0: /*control change .. pitch bend? */
                ctrl=getnext(1); vel=getnext(1);
                control_mode_change(c, ctrl, vel);
/*
                switch(ctrl)
                    {
                    case 0x07:
                        midiprintf ("(pb:%d: %d %d)",c,(int)ctrl,(int)vel);
                        ch[c].vol=vel;
                        midiprintf("vol");
                        break;
                    case 0x63:
                        if (adlib_style & CMF_STYLE) {
                            // Custom extension to allow CMF files to switch the
                            // AM+VIB depth on and off (officially this is on,
                            // and there's no way to switch it off.)  Controller
                            // values:
                            //   0 == AM+VIB off
                            //   1 == VIB on
                            //   2 == AM on
                            //   3 == AM+VIB on
                            midi_write_adlib(0xbd, (adlib_data[0xbd] & ~0xC0) | (vel << 6));
                            midiprintf(" AM+VIB depth change - AM %s, VIB %s\n",
                                (adlib_data[0xbd] & 0x80) ? "on" : "off",
                                (adlib_data[0xbd] & 0x40) ? "on" : "off"
                            );
                        }
                        break;
                    case 0x67:
                        midiprintf("Rhythm mode: %d\n", (int)vel);
                        if ((adlib_style&CMF_STYLE)!=0) {
			  adlib_mode=vel;
			  if(adlib_mode == ADLIB_RYTHM)
			    midi_write_adlib(0xbd, adlib_data[0xbd] | (1 << 5));
			  else
			    midi_write_adlib(0xbd, adlib_data[0xbd] & ~(1 << 5));
			}
                        break;
                    }
*/
                break;
            case 0xc0: /*patch change*/
            	x=getnext(1);
            	program_change(c, x);
            	/*
            	ch[c].inum=x;
            	for (j=0; j<11; j++)
            		ch[c].ins[j]=myinsbank[ch[c].inum][j];
            	program_change(c,x);
            	*/
            	break;
            case 0xd0: /*chanel touch*/
                x=getnext(1);
                break;
            case 0xe0: /*pitch wheel*/
                x=getnext(1);
                l=getnext(1);
                pitch_bend(c,x,l);
                break;
            case 0xf0:
                switch(v)
                    {
                    case 0xf0:
                    case 0xf7: /*sysex*/
		      l=getval();
		      if (datalook(pos+l)==0xf7)
			i=1;
		      midiprintf("{%d}",(int)l);
		      midiprintf("\n");

                        if (datalook(pos)==0x7d &&
                            datalook(pos+1)==0x10 &&
                            datalook(pos+2)<16)
							{
                            adlib_style=LUCAS_STYLE|MIDI_STYLE;
							for (i=0; i<l; i++)
								{
                                midiprintf ("%x ",datalook(pos+i));
                                if ((i-3)%10 == 0) midiprintf("\n");
								}
                            midiprintf ("\n");
                            getnext(1);
                            getnext(1);
							c=getnext(1);
							getnext(1);

                          //  getnext(22); //temp
                            ch[c].ins[0]=(unsigned char)((getnext(1)<<4)+getnext(1));
                            ch[c].ins[2]=(unsigned char)(0xff-(((getnext(1)<<4)+getnext(1))&0x3f));
                            ch[c].ins[4]=(unsigned char)(0xff-((getnext(1)<<4)+getnext(1)));
                            ch[c].ins[6]=(unsigned char)(0xff-((getnext(1)<<4)+getnext(1)));
                            ch[c].ins[8]=(unsigned char)((getnext(1)<<4)+getnext(1));

                            ch[c].ins[1]=(unsigned char)((getnext(1)<<4)+getnext(1));
                            ch[c].ins[3]=(unsigned char)(0xff-(((getnext(1)<<4)+getnext(1))&0x3f));
                            ch[c].ins[5]=(unsigned char)(0xff-((getnext(1)<<4)+getnext(1)));
                            ch[c].ins[7]=(unsigned char)(0xff-((getnext(1)<<4)+getnext(1)));
                            ch[c].ins[9]=(unsigned char)((getnext(1)<<4)+getnext(1));

                            i=(getnext(1)<<4)+getnext(1);
                            ch[c].ins[10]=i;

                            //if ((i&1)==1) ch[c].ins[10]=1;

                            midiprintf ("\n%d: ",c);
							for (i=0; i<11; i++)
                                midiprintf ("%2X ",ch[c].ins[i]);
                            getnext(l-26);
							}
                            else
                            {
                            midiprintf("\n");
                            for (j=0; j<l; j++)
                                midiprintf ("%2X ",(unsigned int)getnext(1));
                            }

                        midiprintf("\n");
						if(i==1)
							getnext(1);
                        break;
                    case 0xf1:
                        break;
                    case 0xf2:
                        getnext(2);
                        break;
                    case 0xf3:
                        getnext(1);
                        break;
                    case 0xf4:
                        break;
                    case 0xf5:
                        break;
                    case 0xf6: /*something*/
                    case 0xf8:
                    case 0xfa:
                    case 0xfb:
                    case 0xfc:
                        //this ends the track for sierra.
                        if (type == FILE_SIERRA ||
                            type == FILE_ADVSIERRA)
                            {
                            track[curtrack].tend=pos;
                            midiprintf ("endmark: %ld -- %lx\n",pos,pos);
                            }
                        break;
                    case 0xfe:
                    	i=getnext(1);
                    	printf("FE %02X pos=%d\n",i, (int)pos);//(unsigned int)getnext(1),(unsigned int)getnext(1));
                    	getnext(2);
                    	if(i==0)
                    	{
                    		printf(" %02X",(unsigned int)getnext(1));
                    		//getnext(1);
                    	}
                    	printf("\n");
                    	break;
                    case 0xfd:
                        break;
                    case 0xff:
                        v=getnext(1);
                        l=getval();
                        midiprintf ("\n");
                        midiprintf("{%X_%X}",(unsigned int)v,(int)l);
                        if (v==0x51)
                            {
                            lnum=getnext(l);
                            msqtr=lnum; /*set tempo*/
                            midiprintf ("Set Tempo (qtr=%ld)",msqtr);
                            }
                        else if (v==0x3)
                            {
                        		midiprintf ("Track Name: ");
                        		for (i=0; i<l; i++)
                        	       midiprintf ("%c",(unsigned char)getnext(1));
                            }
                        else if (v==0x6)
                            {
                        		printf ("Marker: ");
                        		for (i=0; i<l; i++)
                        		{
                        	       //midiprintf ("%c",(unsigned char)getnext(1));
                        	       printf ("%c",(unsigned char)getnext(1));
                        		}
                        		printf("\n");
                            }
                            else
                            {
                            for (i=0; i<l; i++)
                                midiprintf ("%2X ",(unsigned int)getnext(1));
                            }
                        break;
					}
                break;
            default: midiprintf("! v = %d",(int)v); /* if we get down here, a error occurred */
			break;
            }

        if (pos < track[curtrack].tend)
            {
            if (type != FILE_SIERRA && type !=FILE_ADVSIERRA)
                w=getval();
                else
                w=getnext(1);
            track[curtrack].iwait=w;
            /*
            if (w!=0)
                {
                midiprintf("\n<%d>",w);
                f = 
((float)w/(float)deltas)*((float)msqtr/(float)1000000);
                if (doing==1) f=0; //not playing yet. don't wait yet
                }
                */
            }
            else
            track[curtrack].iwait=0;

        track[curtrack].pos=pos;
        }

        for(i=0;i<16;i++)
        {
        	if(current_status[i] == 0)
        		printf("--");
        	else
        		printf("%02X", current_status[i]);
        	printf(" ");
        }
        printf("\n");
        ret=0; //end of song.
        iwait=0;
        for (curtrack=0; curtrack<16; curtrack++)
            if (track[curtrack].on == 1 &&
                track[curtrack].pos < track[curtrack].tend)
                ret=1;  //not yet..

        if (ret==1)
            {
            iwait=0xffffff;  // bigger than any wait can be!
            for (curtrack=0; curtrack<16; curtrack++)
               if (track[curtrack].on == 1 &&
                   track[curtrack].pos < track[curtrack].tend &&
                   track[curtrack].iwait < iwait)
                   iwait=track[curtrack].iwait;
            }
        }


    if (iwait !=0 && ret==1)
        {
        for (curtrack=0; curtrack<16; curtrack++)
            if (track[curtrack].on)
                track[curtrack].iwait-=iwait;

        
fwait=1.0f/(((float)iwait/(float)deltas)*((float)msqtr/(float)1000000));
        }
        else
        fwait=50;  // 1/50th of a second

    midiprintf ("\n");
    for (i=0; i<16; i++)
      if (track[i].on) {
	if (track[i].pos < track[i].tend)
	  ;//midiprintf ("<%d:%d>",(int)i,(int)track[i].iwait);
	else
	  midiprintf("stop");
      }

    /*
    if (ret==0 && type==FILE_ADVSIERRA)
        if (datalook(sierra_pos-2)!=0xff)
            {
            midiprintf ("next sectoin!");
            sierra_next_section(p);
            fwait=50;
            ret=1;
            }
    */

	if(ret)
		return true;
	else
		return false;
}

float CmidPlayer::getrefresh()
{
    return (fwait > 0.01f ? fwait : 0.01f);
}

void CmidPlayer::rewind(int subsong)
{
    long i,j,n,m,l;
    long o_sierra_pos;
    unsigned char ins[16];

    pos=0; tins=0;
    adlib_style=MIDI_STYLE|CMF_STYLE;
    adlib_mode=ADLIB_MELODIC;
    for (i=0; i<128; i++)
        for (j=0; j<16; j++)
            myinsbank[i][j]=midi_fm_instruments[i][j];
	for (i=0; i<16; i++)
        {
        ch[i].inum=0;
        for (j=0; j<11; j++)
            ch[i].ins[j]=myinsbank[ch[i].inum][j];
        ch[i].vol=127;
        ch[i].nshift=-25;
        ch[i].on=1;
        }

    /* General init */
    for (i=0; i<9; i++)
        {
        chp[i][0]=-1;
        chp[i][2]=0;
        }

    deltas=250;  // just a number,  not a standard
    msqtr=500000;
    fwait=123; // gotta be a small thing.. sorta like nothing
    iwait=0;

    subsongs=1;

    for (i=0; i<16; i++)
        {
        track[i].tend=0;
        track[i].spos=0;
        track[i].pos=0;
        track[i].iwait=0;
        track[i].on=0;
        track[i].pv=0;
        }
    curtrack=0;

    /* specific to file-type init */

        pos=0;
        i=getnext(1);
        switch(type)
            {
            case FILE_LUCAS:
                getnext(24);  //skip junk and get to the midi.
                adlib_style=LUCAS_STYLE|MIDI_STYLE;
                //note: no break, we go right into midi headers...
            case FILE_MIDI:
                if (type != FILE_LUCAS)
                    tins=128;
                getnext(9);  /*skip header*/
                track_count = getnext(2); //total number of tracks.
                deltas=getnext(2);
                midiprintf ("deltas:%ld\n",deltas);


                load_ultima_midi_tracks();

                break;
            case FILE_CMF:
                getnext(3);  // ctmf
                getnexti(2); //version
                n=getnexti(2); // instrument offset
                m=getnexti(2); // music offset
                deltas=getnexti(2); //ticks/qtr note
                msqtr=1000000/getnexti(2)*deltas;
                   //the stuff in the cmf is click ticks per second..

                i=getnexti(2);
				if(i) title = (char *)data+i;
                i=getnexti(2);
				if(i) author = (char *)data+i;
                i=getnexti(2);
				if(i) remarks = (char *)data+i;

                getnext(16); // channel in use table ..
                i=getnexti(2); // num instr
                if (i>128) i=128; // to ward of bad numbers...
                getnexti(2); //basic tempo

                midiprintf("\nioff:%d\nmoff%d\ndeltas:%ld\nmsqtr:%ld\nnumi:%d\n",
                    (int)n,(int)m,deltas,msqtr,(int)i);
                pos=n;  // jump to instruments
                tins=i;
                for (j=0; j<i; j++)
                    {
                    midiprintf ("\n%d: ",(int)j);
                    for (l=0; l<16; l++)
                        {
                        myinsbank[j][l]=(unsigned char)getnext(1);
                        midiprintf ("%2X ",myinsbank[j][l]);
                        }
                    }

                for (i=0; i<16; i++)
                    ch[i].nshift=-13;

                adlib_style=CMF_STYLE;

                curtrack=0;
                track[curtrack].on=1;
                track[curtrack].tend=flen;  // music until the end of the file
                track[curtrack].spos=m;  //jump to midi music
                break;
            case FILE_OLDLUCAS:
                msqtr=250000;
                pos=9;
                deltas=getnext(1);

                i=8;
                pos=0x19;  // jump to instruments
                tins=i;
                for (j=0; j<i; j++)
                    {
                    midiprintf ("\n%d: ",(int)j);
                    for (l=0; l<16; l++)
                        ins[l]=(unsigned char)getnext(1);

                    myinsbank[j][10]=ins[2];
                    myinsbank[j][0]=ins[3];
                    myinsbank[j][2]=ins[4];
                    myinsbank[j][4]=ins[5];
                    myinsbank[j][6]=ins[6];
                    myinsbank[j][8]=ins[7];
                    myinsbank[j][1]=ins[8];
                    myinsbank[j][3]=ins[9];
                    myinsbank[j][5]=ins[10];
                    myinsbank[j][7]=ins[11];
                    myinsbank[j][9]=ins[12];

                    for (l=0; l<11; l++)
                        midiprintf ("%2X ",myinsbank[j][l]);
                    }

                for (i=0; i<16; i++)
                    {
                    if (i<tins)
                        {
                        ch[i].inum=i;
                        for (j=0; j<11; j++)
                            ch[i].ins[j]=myinsbank[ch[i].inum][j];
                        }
                    }

                adlib_style=LUCAS_STYLE|MIDI_STYLE;

                curtrack=0;
                track[curtrack].on=1;
                track[curtrack].tend=flen;  // music until the end of the file
                track[curtrack].spos=0x98;  //jump to midi music
                break;
            case FILE_ADVSIERRA:
	      memcpy(myinsbank, smyinsbank, 128 * 16);
	      tins = stins;
                deltas=0x20;
                getnext(11); //worthless empty space and "stuff" :)

                o_sierra_pos=sierra_pos=pos;
                sierra_next_section();
                while (datalook(sierra_pos-2)!=0xff)
                    {
                    sierra_next_section();
                    subsongs++;
                    }

                if (subsong < 0 || subsong >= subsongs) subsong=0;

                sierra_pos=o_sierra_pos;
                sierra_next_section();
                i=0;
                while (i != subsong)
                    {
                    sierra_next_section();
                    i++;
                    }

                adlib_style=SIERRA_STYLE|MIDI_STYLE;  //advanced sierra tunes use volume
                break;
            case FILE_SIERRA:
	      memcpy(myinsbank, smyinsbank, 128 * 16);
	      tins = stins;
                getnext(2);
                deltas=0x20;

                curtrack=0;
                track[curtrack].on=1;
                track[curtrack].tend=flen;  // music until the end of the file

                for (i=0; i<16; i++)
                    {
                    ch[i].nshift=-13;
                    ch[i].on=getnext(1);
                    ch[i].inum=getnext(1);
                    for (j=0; j<11; j++)
                        ch[i].ins[j]=myinsbank[ch[i].inum][j];
                    }

                track[curtrack].spos=pos;
                adlib_style=SIERRA_STYLE|MIDI_STYLE;
                break;
            }


/*        sprintf(info,"%s\r\nTicks/Quarter Note: %ld\r\n",info,deltas);
        sprintf(info,"%sms/Quarter Note: %ld",info,msqtr); */

        for (i=0; i<16; i++)
            if (track[i].on)
                {
                track[i].pos=track[i].spos;
                track[i].pv=0;
                track[i].iwait=0;
                }

    doing=1;
    midi_fm_reset();
}

void CmidPlayer::load_ultima_midi_tracks()
{
    for(curtrack=0;curtrack<track_count;curtrack++)
    {
        getnext(4); //skip MTrk

    track[curtrack].on=1;
    track[curtrack].tend=getnext(4);
    track[curtrack].tend+=pos;
    track[curtrack].spos=pos;
    pos=track[curtrack].tend;
    midiprintf ("tracklen:%ld\n",track[curtrack].tend-track[curtrack].spos);
    }
}

std::string CmidPlayer::gettype()
{
	switch(type) {
	case FILE_LUCAS:
		return std::string("LucasArts AdLib MIDI");
	case FILE_MIDI:
		return std::string("General MIDI");
	case FILE_CMF:
		return std::string("Creative Music Format (CMF MIDI)");
	case FILE_OLDLUCAS:
		return std::string("Lucasfilm Adlib MIDI");
	case FILE_ADVSIERRA:
		return std::string("Sierra On-Line VGA MIDI");
	case FILE_SIERRA:
		return std::string("Sierra On-Line EGA MIDI");
	default:
		return std::string("MIDI unknown");
	}
}
