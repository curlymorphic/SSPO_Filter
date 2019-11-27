/*
 * Copyright (c) 2019 Dave French <contact/dot/dave/dot/french3/at/googlemail/dot/com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#pragma once

/**
The base AudioProcess class to be inherited by all audio processing classes.
AudioProcess classes are designed to process a single channel of audio,
*/
class AudioProcess
{
public:
	AudioProcess()
	{

	}

	AudioProcess(int sr)
	{
		sampleRate = sr;
	}

	~AudioProcess()
	{}

	virtual void setSampleRate(int sr)
	{
		if ( sr > 0 ) sampleRate = sr;
	}

	virtual float processSample(float in) = 0;

	virtual void processBlock(float* block, int blockSize)
	{
		for (auto i = 0; i < blockSize; ++i)
		{
			auto data = block[i];
			data = processSample(data);
			block[i] = data;
		}
	}


protected:
	int sampleRate = 1;
};