/*
  ==============================================================================

    AudioProcess.h
    Created: 16 Nov 2019 1:39:20am
    Author:  davef

  ==============================================================================
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