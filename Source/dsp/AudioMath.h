/*
  ==============================================================================

    AudioMath.h
    Created: 20 Nov 2019 8:05:43am
    Author:  davef

  ==============================================================================
*/

#pragma once

#include <algorithm>



const long double LD_PI = 3.14159265358979323846264338327950288419716939937510;
const float F_PI = (float)LD_PI;
const float f_2PI = F_PI + F_PI;
const float base_a4 = 440.0f;


inline float midiNoteFreq(int note)
{
	return 440.0 * powf(2.0, (note - 69.0) / 12.0);
}

inline float fraction(float val) {
	return val - (int)val;
}

inline float bound(float minval, float val, float maxval)
{
	return fmax(minval, fmin(val, maxval));
}

//inline int freqNeariestMidiNote( float freq )
//{
//	return round(12*log( freq / base_a4))+57;
//}