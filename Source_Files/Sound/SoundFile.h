#ifndef __SOUND_DEFINITION_H
#define __SOUND_DEFINITION_H

/*
SOUND_DEFINITIONS.H

	Copyright (C) 1991-2001 and beyond by Bungie Studios, Inc.
	and the "Aleph One" developers.
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This license is contained in the file "COPYING",
	which is included with this source code; it is available online at
	http://www.gnu.org/licenses/gpl.html

*/

#include "AStream.h"
#include "FileHandler.h"
#include <memory>
#include <vector>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

typedef std::vector<uint8> SoundData;

class SoundInfo 
{
public:
	SoundInfo() : sixteen_bit(false), 
		      stereo(false), 
		      signed_8bit(false), 
		      little_endian(false), 
		      bytes_per_frame(1),
		      loop_start(0),
		      loop_end(0),
		      rate(0),
		      length(0) { }
	
	bool sixteen_bit;
	bool stereo;
	bool signed_8bit;
	bool little_endian;
	int bytes_per_frame;
	int32 loop_start;
	int32 loop_end;
	uint32 rate;
	int32 length;
};

class SoundHeader : public SoundInfo
{
public:
	SoundHeader();
	virtual ~SoundHeader() { };

	bool Load(OpenedFile &SoundFile); // loads a system 7 header from file
	boost::shared_ptr<SoundData> LoadData(OpenedFile& SoundFile);
	
	// loads and copies a system 7 sound
	boost::shared_ptr<SoundData> Load(const uint8* data);

	int32 Length() const
		{ return length; };
	
	void Clear() { length = 0; }

private:
	bool UnpackStandardSystem7Header(AIStreamBE &header);
	bool UnpackExtendedSystem7Header(AIStreamBE &header);
	
	uint32 data_offset;
};

class SoundDefinition
{
public:
	bool Unpack(OpenedFile &SoundFile);
	bool Load(OpenedFile &SoundFile, bool LoadPermutations);
	boost::shared_ptr<SoundData> LoadData(OpenedFile& SoundFile, short permutation);
	void Unload() { sounds.clear(); }

	static const int MAXIMUM_PERMUTATIONS_PER_SOUND = 5;

private:
	static int HeaderSize() { return 64; }
	
public: // for now
	int16 sound_code;
	int16 behavior_index;
	uint16 flags;

	uint16 chance; // play sound if AbsRandom() >= chance
	
	/* if low_pitch==0 use FIXED_ONE; if high_pitch==0 use low pitch; else choose in [low_pitch, high_pitch] */
	_fixed low_pitch, high_pitch;

	/* filled in later */
	int16 permutations;
	uint16 permutations_played;

	int32 group_offset, single_length, total_length; // magic numbers necessary to load sounds
	std::vector<int32> sound_offsets; // zero-based from group offset

	uint32 last_played; // machine ticks

	std::vector<SoundHeader> sounds;
};

class SoundFile
{
public:
	bool Open(FileSpecifier &SoundFile);
	void Close();
	SoundDefinition* GetSoundDefinition(int source, int sound_index);
	void Load(int source, int sound_index);

public:
	int32 version;
	int32 tag;
	
	int16 source_count;
	int16 sound_count;
	int16 real_sound_count;

	static const int v1Unused = 124;

	std::vector< std::vector<SoundDefinition> > sound_definitions;

	static int HeaderSize() { return 260; }
	std::auto_ptr<OpenedFile> opened_sound_file;
};

#endif
