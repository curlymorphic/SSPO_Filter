
#pragma once
#include <float.h>
#include <math.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>



/*
 * Copyright (c) 2014-2015 Dave French <contact/dot/dave/dot/french3/at/googlemail/dot/com>
 *
 * This file is part of curlymorphic-sspo - 
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
#include "AudioProcess.h"



class FirstOrderFeedBackFilter
{
public:

	FirstOrderFeedBackFilter()
	{
		clear();
	}

	~FirstOrderFeedBackFilter()
	{

	}

	inline void clear()
	{
		m_z1 = 0;
	}
	inline float tick(float in)
	{
		float data = m_a0 * in - m_b1 * m_z1;
		m_z1 = data;
		return data;
	}

	void setCoeffs(float a0, float b1)
	{
		m_a0 = a0;
		m_b1 = b1;
	}


protected:

	float m_z1 = 0;
	float m_a0 = 0.5f;
	float m_b1 = -0.50f;
};



///
/// \brief The BiQuad class
/// Standard BiQuad implementation. base class for filters
class BiQuad
{
public:
	BiQuad()
	{
		clear();
	}
	virtual ~BiQuad() {}

	inline void setCoeffs(float a1, float a2, float b0, float b1, float b2)
	{
		m_a1 = a1;
		m_a2 = a2;
		m_b0 = b0;
		m_b1 = b1;
		m_b2 = b2;
	}
	inline void clear()
	{
		m_z1 = 0.0f;
		m_z2 = 0.0f;
	}
	virtual inline float tick(float in)
	{
		// biquad filter in transposed form
		const float out = m_z1 + m_b0 * in;
		m_z1 = m_b1 * in + m_z2 - m_a1 * out;
		m_z2 = m_b2 * in - m_a2 * out;
		return out;
	}
protected:
	float m_a1, m_a2, m_b0, m_b1, m_b2;
	float m_z1, m_z2;

};




///
/// \brief The EqFilter class.
/// A wrapper for the various filter classes, giving it freq, res, and gain controls.
/// It is designed to process periods in one pass, with recalculation of coefficents
/// upon parameter changes. The intention is to use this as a bass class, children override
/// the calcCoefficents() function, providing the coefficents a1, a2, b0, b1, b2.
///
class Filter : public AudioProcess
{
public:

	Filter() : AudioProcess()
	{


	}


	Filter(int samplerate) :
		AudioProcess() {
		sampleRate = samplerate;
	}


	virtual inline void setFrequency(float freq) {
		if (freq != m_freq)
		{
			m_freq = bound(35.0f, freq, 20000.0f);
			calcCoefficents();
		}
	}

	virtual void clear() = 0;


	virtual void setQ(float Q)
	{
		if (m_res != Q)
		{
			m_res = Q;
			calcCoefficents();
		}
	}


	virtual void setGain(float proposedGain)
	{
		if (m_gain != proposedGain)
		{
			m_gain = proposedGain;
			calcCoefficents();
		}
	}


	virtual inline void setParameters(float freq, float Q, float proposedGain = 1.0)
	{
		bool hasChanged = false;
		if (freq != m_freq)
		{
			m_freq = bound(55.0f, freq, 20000.0f);
			hasChanged = true;
		}
		if (m_res != Q)
		{
			m_res = bound(0.6f, Q, 5.0f);
			hasChanged = true;
		}
		if (m_gain != proposedGain)
		{
			m_gain = proposedGain;
			hasChanged = true;
		}

		if (hasChanged) { calcCoefficents(); }
	}

	void setSampleRate(int sr) override
	{
		sampleRate = sr;
		calcCoefficents();
	}

	///
	/// \brief calcCoefficents
	///  Override this in child classes to provide the coefficents, based on
	///  Freq, Res and Gain
	virtual void calcCoefficents() = 0;

protected:


	float m_freq = 440;
	float m_res = 0.7f;
	float m_gain = 0.0f;

};


class Lp6 : public Filter, public FirstOrderFeedBackFilter
{
public:
	Lp6() : Filter()
	{}

	Lp6(int sampleRate)
		: Filter(sampleRate)
	{}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		FirstOrderFeedBackFilter::clear();
	}

	void calcCoefficents() override
	{
		float theta = (2 * F_PI * m_freq) / sampleRate;
		auto gamma = 2 - std::cos(theta);
		m_b1 = std::sqrtf(gamma * gamma - 1) - gamma;
		m_a0 = 1 + m_b1;
	}
};


//HP6 filter not working as intended

//class Hp6 : public Filter, public FirstOrderFeedBackFilter
//{
//public:
//	Hp6() : Filter()
//	{}
//
//	Hp6(int sampleRate)
//		: Filter(sampleRate)
//	{}
//
//	float processSample(float in) override
//	{
//		return tick(in);
//	}
//
//	void clear() override
//	{
//		FirstOrderFeedBackFilter::clear();
//	}
//
//	void calcCoefficents() override
//	{
//		float theta = (2.0f * F_PI * m_freq) / sampleRate;
//		auto gamma = 2.0f + std::cosf(theta);
//		m_b1 = gamma - std::sqrtf(gamma * gamma - 1.0f);
//		m_a0 = 1.0f - m_b1;
//	}
//};


///
/// \brief The EqHp12Filter class
/// A 2 pole High Pass Filter
/// Coefficent calculations from http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
class Hp12 : public Filter , public BiQuad
{
public:

	Hp12() : Filter()
	{}


	Hp12(int samplerate) :
		Filter(samplerate)
	{
	}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		BiQuad::clear();
	}

	void calcCoefficents() override
	{

		// calc intermediate
		float w0 = f_2PI * m_freq / sampleRate;
		float c = cosf(w0);
		float s = sinf(w0);
		float alpha = s / (2 * m_res);

		float a0, a1, a2, b0, b1, b2; // coeffs to calculate

		//calc coefficents
		b0 = (1 + c) * 0.5f;
		b1 = (-(1 + c));
		b2 = (1 + c) * 0.5f;
		a0 = 1 + alpha;
		a1 = (-2 * c);
		a2 = 1 - alpha;

		//normalise
		b0 /= a0;
		b1 /= a0;
		b2 /= a0;
		a1 /= a0;
		a2 /= a0;

		a0 = 1;

		setCoeffs(a1, a2, b0, b1, b2);
	}


};




///
/// \brief The EqLp12Filter class.
/// A 2 pole low pass filter
/// Coefficent calculations from http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
///
class Lp12 : public Filter, public BiQuad
{
public:
	Lp12() :
		Filter()
	{
	}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		BiQuad::clear();
	}

	void calcCoefficents() override
	{

		// calc intermediate
		float w0 = f_2PI * m_freq / sampleRate;
		float c = cosf(w0);
		float s = sinf(w0);
		float alpha = s / (2 * m_res);

		float a0, a1, a2, b0, b1, b2; // coeffs to calculate

		//calc coefficents
		b0 = (1 - c) * 0.5f;
		b1 = 1 - c;
		b2 = (1 - c) * 0.5f;
		a0 = 1 + alpha;
		a1 = -2 * c;
		a2 = 1 - alpha;

		//normalise
		b0 /= a0;
		b1 /= a0;
		b2 /= a0;
		a1 /= a0;
		a2 /= a0;

		a0 = 1;

		setCoeffs(a1, a2, b0, b1, b2);
	}
};



///
/// \brief The EqPeakFilter class
/// A Peak Filter
/// Coefficent calculations from http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
///
class PeakFilter : public Filter, public BiQuad
{
public:
	PeakFilter() :
		Filter()
	{
	}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		BiQuad::clear();
	}


	void calcCoefficents() override
	{
		// calc intermediate
		float w0 = f_2PI * m_freq / sampleRate;
		float c = cosf(w0);
		float s = sinf(w0);
		float A = powf(10, m_gain * 0.025f);
		float alpha = s / (2 * m_res);

		float a0, a1, a2, b0, b1, b2; // coeffs to calculate

		//calc coefficents
		b0 = 1 + alpha * A;
		b1 = -2 * c;
		b2 = 1 - alpha * A;
		a0 = 1 + alpha / A;
		a1 = -2 * c;
		a2 = 1 - alpha / A;

		//normalise
		b0 /= a0;
		b1 /= a0;
		b2 /= a0;
		a1 /= a0;
		a2 /= a0;
		a0 = 1;

		setCoeffs(a1, a2, b0, b1, b2);
	}
};




class LowShelf : public Filter, public BiQuad
{
public:
	LowShelf() :
		Filter()
	{
	}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		BiQuad::clear();
	}
	void calcCoefficents() override
	{

		// calc intermediate
		float w0 = f_2PI * m_freq / sampleRate;
		float c = cosf(w0);
		float s = sinf(w0);
		float A = powf(10, m_gain * 0.025f);
		//        float alpha = s / ( 2 * m_res );
		float beta = sqrt(A) / m_res;

		float a0, a1, a2, b0, b1, b2; // coeffs to calculate

		//calc coefficents
		b0 = A * ((A + 1) - (A - 1) * c + beta * s);
		b1 = 2 * A * ((A - 1) - (A + 1) * c);
		b2 = A * ((A + 1) - (A - 1) * c - beta * s);
		a0 = (A + 1) + (A - 1) * c + beta * s;
		a1 = -2 * ((A - 1) + (A + 1) * c);
		a2 = (A + 1) + (A - 1) * c - beta * s;

		//normalise
		b0 /= a0;
		b1 /= a0;
		b2 /= a0;
		a1 /= a0;
		a2 /= a0;

		a0 = 1;

		setCoeffs(a1, a2, b0, b1, b2);
	}
};

class HighShelf : public Filter, public BiQuad
{
public:
	HighShelf() :
		Filter()
	{
	}

	float processSample(float in) override
	{
		return tick(in);
	}

	void clear() override
	{
		BiQuad::clear();
	}

	void calcCoefficents() override
	{

		// calc intermediate
		float w0 = f_2PI * m_freq / sampleRate;
		float c = cosf(w0);
		float s = sinf(w0);
		float A = powf(10, m_gain * 0.025f);
		float beta = sqrt(A) / m_res;

		float a0, a1, a2, b0, b1, b2; // coeffs to calculate

		//calc coefficents
		b0 = A * ((A + 1) + (A - 1) * c + beta * s);
		b1 = -2 * A * ((A - 1) + (A + 1) * c);
		b2 = A * ((A + 1) + (A - 1) * c - beta * s);
		a0 = (A + 1) - (A - 1) * c + beta * s;
		a1 = 2 * ((A - 1) - (A + 1) * c);
		a2 = (A + 1) - (A - 1) * c - beta * s;
		//normalise
		b0 /= a0;
		b1 /= a0;
		b2 /= a0;
		a1 /= a0;
		a2 /= a0;
		a0 = 1;

		setCoeffs(a1, a2, b0, b1, b2);
	}
};

/**
Wrapper for chaining filters in series, passing function calls
*/
class FilterChain : public Filter
{
public:
	FilterChain()
		: Filter() {}

	~FilterChain() { for (auto f : m_filters) { delete f; } }

	void push_back(Filter* newFilter) { m_filters.push_back(newFilter); }

	inline void setFrequency(float freq) override { for (auto f : m_filters) { f->setFrequency(freq); } }

	void setQ(float Q) override { for (auto f : m_filters) { f->setQ(Q); } }

	void setGain(float proposedGain) override { for (auto f : m_filters) { f->setGain(proposedGain); } }

	inline void setParameters(float freq, float Q, float proposedGain = 1.0) override
	{
		for (auto f : m_filters) { f->setParameters(freq, Q, proposedGain); }
	}

	inline float processSample(float in) override
	{
		float val = in;
		for (auto f : m_filters) { val = f->processSample(val); }
		return val;
	}

	inline void clear() override { for (auto f : m_filters) { f->clear(); } }

	void calcCoefficents() override
	{
		for (auto f : m_filters) { f->calcCoefficents(); }
	}

	void setSampleRate(int sr)
	{
		for (auto f : m_filters) { f->setSampleRate(sr); }
	}

private:
	std::vector<Filter*> m_filters;


};

class Lp24 : public FilterChain
{
public:
	Lp24() : FilterChain()
	{
		push_back(new Lp12());
		push_back(new Lp12());
	}
};

class Hp24 : public FilterChain
{
public:
	Hp24() : FilterChain()
	{
		push_back(new Hp12(sampleRate));
		push_back(new Hp12(sampleRate));
	}
};


class MultiFilter : public Filter
{
public:

	static std::vector<std::string> TYPES()
	{
		return { "LP6", "LP12", "LP24", "HP12", "HP24", "Low Shelf", "High Shelf", "Peak" };
	}

	MultiFilter()
	{
		m_filters.push_back(new Lp6());
		m_filters.push_back(new Lp12());
		m_filters.push_back(new Lp24());
		m_filters.push_back(new Hp12());
		m_filters.push_back(new Hp24());
		m_filters.push_back(new LowShelf());
		m_filters.push_back(new HighShelf());
		m_filters.push_back(new PeakFilter());

		setType(TYPES()[0]);
	}

	~MultiFilter()
	{
		for (auto f : m_filters) { delete f; }
	}

	bool setType(std::string type)
	{
		for (auto i = 0; i < TYPES().size(); ++i)
		{
			if (type.compare(TYPES()[i]) == 0)
			{
				m_currentFilter = m_filters[i];
				m_currentFilter->setSampleRate(sampleRate);
				m_currentFilter->calcCoefficents();
				m_currentFilter->clear();
				return true;
			}
		}
		return false;
	}

	inline void setFrequency(float freq) override { m_currentFilter->setFrequency(freq); }

	void setQ(float Q) override { m_currentFilter->setQ(Q); }

	void setGain(float proposedGain) override { m_currentFilter->setGain(proposedGain); }

	inline void setParameters(float freq, float Q, float proposedGain = 1.0) override
	{
		 m_currentFilter->setParameters(freq, Q, proposedGain); 
	}

	inline float processSample(float in) override
	{
		return m_currentFilter->processSample(in);
	}

	inline void clear() override { m_currentFilter->clear(); }

	void calcCoefficents() override
	{
		m_currentFilter->calcCoefficents();
	}



private:
	std::vector<Filter*> m_filters;
	Filter* m_currentFilter;
};



