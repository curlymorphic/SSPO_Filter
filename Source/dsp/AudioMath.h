/*
 * Copyright (c) 2019 Dave French <contact/dot/dave/dot/french3/at/googlemail/dot/com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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

#include <algorithm>



constexpr long double LD_PI = 3.14159265358979323846264338327950288419716939937510;
constexpr auto k_pi = static_cast<float>(LD_PI);
constexpr auto k_2pi = k_pi + k_pi;
constexpr auto base_a4 = 440.0f;


inline float midiNoteFreq(int note) noexcept
{
	return 440.0f * powf(2.0f, (note - 69.0f) / 12.0f);
}

inline float fraction(float val) noexcept
{
	return val - static_cast<int>(val);
}

inline float bound(float minval, float val, float maxval) noexcept
{
	return fmax(minval, fmin(val, maxval));
}

//inline int freqNeariestMidiNote( float freq )
//{
//	return round(12*log( freq / base_a4))+57;
//}