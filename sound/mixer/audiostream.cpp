/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: https://scummvm.svn.sourceforge.net/svnroot/scummvm/scummvm/trunk/sound/audiostream.cpp $
 * $Id: audiostream.cpp 51094 2010-07-21 18:17:51Z lordhoto $
 *
 */

//#include "common/debug.h"
//#include "common/endian.h"
//#include "common/file.h"
//#include "common/queue.h"
//#include "common/util.h"
#include <assert.h>
#include "audiostream.h"
//#include "sound/decoders/flac.h"
#include "mixer.h"
//#include "sound/decoders/mp3.h"
//#include "sound/decoders/raw.h"
//#include "sound/decoders/vorbis.h"


namespace Audio {
/*
struct StreamFileFormat {
	// Decodername
	const char *decoderName;
	const char *fileExtension;
	//
	// Pointer to a function which tries to open a file of type StreamFormat.
	// Return NULL in case of an error (invalid/nonexisting file).
	//
	SeekableAudioStream *(*openStreamFile)(Common::SeekableReadStream *stream, DisposeAfterUse::Flag disposeAfterUse);
};

static const StreamFileFormat STREAM_FILEFORMATS[] = {
	// decoderName,  fileExt, openStreamFuntion
#ifdef USE_FLAC
	{ "FLAC",         ".flac", makeFLACStream },
	{ "FLAC",         ".fla",  makeFLACStream },
#endif
#ifdef USE_VORBIS
	{ "Ogg Vorbis",   ".ogg",  makeVorbisStream },
#endif
#ifdef USE_MAD
	{ "MPEG Layer 3", ".mp3",  makeMP3Stream },
#endif

	{ NULL, NULL, NULL } // Terminator
};

SeekableAudioStream *SeekableAudioStream::openStreamFile(const Common::String &basename) {
	SeekableAudioStream *stream = NULL;
	Common::File *fileHandle = new Common::File();

	for (int i = 0; i < ARRAYSIZE(STREAM_FILEFORMATS)-1 && stream == NULL; ++i) {
		Common::String filename = basename + STREAM_FILEFORMATS[i].fileExtension;
		fileHandle->open(filename);
		if (fileHandle->isOpen()) {
			// Create the stream object
			stream = STREAM_FILEFORMATS[i].openStreamFile(fileHandle, DisposeAfterUse::YES);
			fileHandle = 0;
			break;
		}
	}

	delete fileHandle;

	if (stream == NULL)
		debug(1, "SeekableAudioStream::openStreamFile: Could not open compressed AudioFile %s", basename.c_str());

	return stream;
}
*/

#pragma mark -
#pragma mark --- LoopingAudioStream ---
#pragma mark -

LoopingAudioStream::LoopingAudioStream(RewindableAudioStream *stream, uint32 loops, DisposeAfterUse::Flag disposeAfterUse)
    : _parent(stream), _disposeAfterUse(disposeAfterUse), _loops(loops), _completeIterations(0) {
	assert(stream);

	if (!stream->rewind()) {
		// TODO: Properly indicate error
		_loops = _completeIterations = 1;
	}
}

LoopingAudioStream::~LoopingAudioStream() {
	if (_disposeAfterUse == DisposeAfterUse::YES)
		delete _parent;
}

int LoopingAudioStream::readBuffer(sint16 *buffer, const int numSamples) {
	if ((_loops && _completeIterations == _loops) || !numSamples)
		return 0;

	int samplesRead = _parent->readBuffer(buffer, numSamples);

	if (_parent->endOfStream()) {
		++_completeIterations;
		if (_completeIterations == _loops)
			return samplesRead;

		const int remainingSamples = numSamples - samplesRead;

		if (!_parent->rewind()) {
			// TODO: Properly indicate error
			_loops = _completeIterations = 1;
			return samplesRead;
		}

		return samplesRead + readBuffer(buffer + samplesRead, remainingSamples);
	}

	return samplesRead;
}

bool LoopingAudioStream::endOfData() const {
	return (_loops != 0 && (_completeIterations == _loops));
}

AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, uint32 loops) {
	if (loops != 1)
		return new LoopingAudioStream(stream, loops);
	else
		return stream;
}


} // End of namespace Audio
