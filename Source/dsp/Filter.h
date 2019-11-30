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
#include <cmath>
#include <float.h>
#include <math.h>
#include <memory>
#include <string>
#include <vector>

#include "AudioProcess.h"
#include "..\farbot\NonRealtimeMutatable.hpp"



class FirstOrderFeedBackFilter
{
public:

	FirstOrderFeedBackFilter ()
	{
		clear ();
	}

	~FirstOrderFeedBackFilter ()
	{

	}

	inline void clear () noexcept
	{
		m_z1 = 0;
	}
	inline float tick (float in) noexcept
	{
		const float data = m_a0 * in - m_b1 * m_z1;
		m_z1 = data;
		return data;
	}

	void setCoeffs (float a0, float b1) noexcept
	{
		m_a0 = a0;
		m_b1 = b1;
	}

protected:

	float m_z1{ 0.0f };
	float m_a0{ 0.5f };
	float m_b1{ -0.5f };
};


///
/// \brief The BiQuad class
/// Transposed Canonical Form  BiQuad implementation. base class for various filters
/// based on the block diagram in Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle p249
class BiQuad
{
public:
	BiQuad ()
	{
		clear ();
	}
	virtual ~BiQuad () {}

	inline void setCoeffs (float a0, float a1, float a2, float b1, float b2, float c0, float d0)
	{
		BiquadCoeffecients newCoeffs{ a0,  a1,  a2,  b1,  b2,  c0,  d0 };
		farbot::NonRealtimeMutatable<BiquadCoeffecients>::ScopedAccess<false> coeffs (m_biquadCoeffs);
		*coeffs = newCoeffs;
	}

	inline void clear () noexcept
	{
		m_z1 = 0.0f;
		m_z2 = 0.0f;
	}

	inline float tick (float in)
	{
		farbot::NonRealtimeMutatable<BiquadCoeffecients>::ScopedAccess<true> coeffs (m_biquadCoeffs);

		float out = m_z1 + coeffs->m_a0 * in;
		//check denormal
		if (!isnormal (out)) out = 0.0f;
		m_z1 = coeffs->m_a1 * in + m_z2 - coeffs->m_b1 * out;
		m_z2 = coeffs->m_a2 * in - coeffs->m_b2 * out;
		return out * coeffs->m_c0 + in * coeffs->m_d0;
	}

protected:

	float m_z1, m_z2;

	struct BiquadCoeffecients
	{
		float m_a0, m_a1, m_a2, m_b1, m_b2;
		float m_c0, m_d0;
	};

	farbot::NonRealtimeMutatable<BiquadCoeffecients> m_biquadCoeffs;
};




///
/// \brief The Filter class.
/// A wrapper for the various filter classes, having freq, Q, and gain controls.
/// The intention is to use this as a bass class, children override
/// the calcCoefficents() and clear() functions
///
class Filter : public AudioProcess
{
public:

	Filter () : AudioProcess ()
	{


	}

	Filter (int samplerate) :
		AudioProcess () {
		m_sampleRate = samplerate;
	}

	virtual inline void setFrequency (float freq)
	{
		m_freq = bound (20.0f, freq, 20000.0f);
		calcCoefficents ();
	}

	virtual void clear () = 0;

	virtual void setQ (float Q)
	{

		m_Q = Q;
		calcCoefficents ();
	}

	virtual void setGain (float proposedGain)
	{
		m_gain = proposedGain;
		calcCoefficents ();
	}


	virtual inline void setParameters (float freq, float Q, float gain = 1.0)
	{
		m_freq = bound (20.0f, freq, 20000.0f);
		m_Q = bound (0.1f, Q, 20.0f);
		m_gain = gain;
		calcCoefficents ();
	}

	void setSampleRate (int sr) override
	{
		m_sampleRate = sr;
		calcCoefficents ();
	}

	///
	/// \brief calcCoefficents
	///  Override this in child classes to provide the coefficents, based on
	///  m_freq, m_Q and m_gain
	virtual void calcCoefficents () = 0;

	virtual bool getUseQ () = 0;
	virtual bool getUseGain () = 0;

protected:

	float m_freq{ 440.0f };
	float m_Q{ 0.707f };
	float m_gain{ 0.0f };

};

///
/// \brief The Lp6 Filter class
/// A 1 pole Low Pass Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class Lp6 : public Filter, public BiQuad
{
public:
	Lp6 () : Filter ()
	{}

	Lp6 (int sampleRate)
		: Filter (sampleRate)
	{}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float c = cosf (theta);
		const float s = sinf (theta);
		const float gamma = c / (1 + s);
		const float a0 = (1 - gamma) * 0.5f;
		const float a1 = (1 - gamma) * 0.5f;
		const float a2 = 0.0f;
		const float b1 = -gamma;
		const float b2 = 0.0f;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return false;
	}
};

///
/// \brief The Hp6 Filter class
/// A 1 pole High Pass Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class Hp6 : public Filter, public BiQuad
{
public:
	Hp6 () : Filter ()
	{}

	Hp6 (int sampleRate)
		: Filter (sampleRate)
	{}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float c = cosf (theta);
		const float s = sinf (theta);
		const float gamma = c / (1 + s);
		const float a0 = (1 + gamma) * 0.5f;
		const float a1 = (1 + gamma) * -0.5f;
		const float a2 = 0.0f;
		const float b1 = -gamma;
		const float b2 = 0.0f;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return false;
	}
};



///
/// \brief The Hp12 Filter class
/// A 2 pole High Pass Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class Hp12 : public Filter, public BiQuad
{
public:

	Hp12 () : Filter ()
	{}


	Hp12 (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float c = cosf (theta);
		const float s = sinf (theta);
		const float d = 1.0f / m_Q;
		const float beta = 0.5f * ((1 - 0.5f * d * s) / (1 + 0.5f * d * s));
		const float gamma = (0.5f + beta) * c;

		const float a0 = (0.5f + beta + gamma) * 0.5f;
		const float a1 = -(0.5f + beta + gamma);
		const float a2 = (0.5f + beta + gamma) * 0.5f;
		const float b1 = -2.0f * gamma;
		const float b2 = 2.0f * beta;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};




///
/// \brief The Lp12 Filter class.
/// A 2 pole low pass filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
///
class Lp12 : public Filter, public BiQuad
{
public:
	Lp12 () :
		Filter ()
	{
	}

	Lp12 (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float c = cosf (theta);
		const float s = sinf (theta);
		const float d = 1.0f / m_Q;
		const float beta = 0.5f * ((1 - 0.5f * d * s) / (1 + 0.5f * d * s));
		const float gamma = (0.5f + beta) * c;

		const float a0 = (0.5f + beta - gamma) * 0.5f;
		const float a1 = 0.5f + beta - gamma;
		const float a2 = (0.5f + beta - gamma) * 0.5f;
		const float b1 = -2.0f * gamma;
		const float b2 = 2.0f * beta;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};

///
/// \brief The Bp12 Filter class.
/// A 2 pole band pass filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
///
class Bp12 : public Filter, public BiQuad
{
public:
	Bp12 () :
		Filter ()
	{
	}

	Bp12 (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float K = tanf ((k_pi * m_freq) / m_sampleRate);
		const float delta = K * K * m_Q + K + m_Q;

		const float a0 = K / delta;
		const float a1 = 0.0;
		const float a2 = -K / delta;
		const float b1 = (2.0f * m_Q * (K * K - 1)) / delta;
		const float b2 = (K * K * m_Q - K + m_Q) / delta;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};

///
/// \brief The Bs12 Filter class.
/// A 2 pole band stop filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
///
class Bs12 : public Filter, public BiQuad
{
public:
	Bs12 () :
		Filter ()
	{
	}

	Bs12 (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float K = tanf ((k_pi * m_freq) / m_sampleRate);
		const float delta = K * K * m_Q + K + m_Q;

		const float a0 = (m_Q * (K * K + 1)) / delta;
		const float a1 = (2.0f * m_Q * (K * K - 1)) / delta;
		const float a2 = (m_Q * (K * K + 1)) / delta;
		const float b1 = (2.0f * m_Q * (K * K - 1)) / delta;
		const float b2 = (K * K * m_Q - K + m_Q) / delta;
		const float c0 = 1.0f;
		const float d0 = 0.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};



///
/// \brief The PeakFilter class
/// A 2 pole Peak Pass Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class PeakFilter : public Filter, public BiQuad
{
public:
	PeakFilter () :
		Filter ()
	{
	}

	PeakFilter (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}


	void calcCoefficents () override
	{
		const float Q = fmax (1.0f, m_Q);
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float mu = powf (10, m_gain / 20.0f);
		const float zeta = 4.0f / (1.0f + mu);
		const float beta = 0.5f * ((1 - zeta * tanf (theta / (2.0f * Q))) / (1 + zeta * tanf (theta / (2 * Q))));
		const float gamma = (0.5f + beta) * cosf (theta);

		const float a0 = 0.5f - beta;
		const float a1 = 0.0;
		const float a2 = -(0.5f - beta);
		const float b1 = -2.0f * gamma;
		const float b2 = 2.0f * beta;
		const float c0 = mu - 1.0f;
		const float d0 = 1.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return true;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};



///
/// \brief The LowShelf Filter class
/// A Low Shelf Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class LowShelf : public Filter, public BiQuad
{
public:
	LowShelf () :
		Filter ()
	{
	}

	LowShelf (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}
	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float mu = powf (10, m_gain / 20.0f);
		const float beta = 4.0f / (1.0f + mu);
		const float delta = beta * tanf (theta * 0.5f);
		const float gamma = (1.0f - delta) / (1.0f + delta);

		const float a0 = (1.0f - gamma) * 0.5f;
		const float a1 = (1.0f - gamma) * 0.5f;
		const float a2 = 0.0f;
		const float b1 = -gamma;
		const float b2 = 0.0f;
		const float c0 = mu - 1.0f;
		const float d0 = 1.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return true;
	}

	bool getUseQ () noexcept override
	{
		return false;
	}
};

///
/// \brief The HighShelf Filter class
/// A  High Shelf Filter
/// Coefficent calculations from Designing Audio Effects Plugins in c++ 2nd ed Will Pirkle
class HighShelf : public Filter, public BiQuad
{
public:
	HighShelf () :
		Filter ()
	{
	}

	HighShelf (int samplerate) :
		Filter (samplerate)
	{
	}

	float processSample (float in) override
	{
		return tick (in);
	}

	void clear () override
	{
		BiQuad::clear ();
	}

	void calcCoefficents () override
	{
		const float theta = k_2pi * m_freq / m_sampleRate;
		const float mu = powf (10, m_gain / 20.0f);
		const float beta = (1.0f + mu) / 4.0f;
		const float delta = beta * tanf (theta * 0.5f);
		const float gamma = (1.0f - delta) / (1.0f + delta);

		const float a0 = (1.0f + gamma) * 0.5f;
		const float a1 = (1.0f + gamma) * -0.5f;
		const float a2 = 0.0f;
		const float b1 = -gamma;
		const float b2 = 0.0f;
		const float c0 = mu - 1.0f;
		const float d0 = 1.0f;

		setCoeffs (a0, a1, a2, b1, b2, c0, d0);
	}

	bool getUseGain () noexcept override
	{
		return true;
	}

	bool getUseQ () noexcept override
	{
		return false;
	}
};

///
/// \brief Wrapper for chaining filters in series

class FilterChain : public Filter
{
public:
	FilterChain ()
		: Filter () {}

	~FilterChain () {  }

	void push_back (std::unique_ptr<Filter> newFilter) { m_filters.push_back (std::move (newFilter)); }

	inline void setFrequency (float freq) override { for (auto& f : m_filters) { f->setFrequency (freq); } }

	void setQ (float Q) override { for (auto& f : m_filters) { f->setQ (Q); } }

	void setGain (float proposedGain) override { for (auto& f : m_filters) { f->setGain (proposedGain); } }

	inline void setParameters (float freq, float Q, float gain = 0.0) override
	{
		for (auto& f : m_filters) { f->setParameters (freq, Q, gain); }
	}

	inline float processSample (float in) override
	{
		float val = in;
		for (auto& f : m_filters) { val = f->processSample (val); }
		return val;
	}

	inline void clear () override { for (auto& f : m_filters) { f->clear (); } }

	void calcCoefficents () override
	{
		for (auto& f : m_filters) { f->calcCoefficents (); }
	}

	void setSampleRate (int sr) override
	{
		for (auto& f : m_filters) { f->setSampleRate (sr); }
	}

private:
	std::vector<std::unique_ptr<Filter>> m_filters;
};

class Lp24 : public FilterChain
{
public:
	Lp24 () : FilterChain ()
	{
		push_back (std::make_unique<Lp12> ());
		push_back (std::make_unique<Lp12> ());
	};

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};

class Hp24 : public FilterChain
{
public:
	Hp24 () : FilterChain ()
	{
		push_back (std::make_unique<Hp12> ());
		push_back (std::make_unique<Hp12> ());
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return true;
	}
};


class MultiFilter : public Filter
{
public:

	static std::vector<std::string> typeStings ()
	{
		return { "LP6", "LP12", "LP24", "HP6", "HP12", "HP24", "Low Shelf", "High Shelf", "Peak", "BP12", "BS12" };
	}

	MultiFilter ()
	{
		m_filters.push_back (std::make_unique < Lp6> ());
		m_filters.push_back (std::make_unique < Lp12 > ());
		m_filters.push_back (std::make_unique < Lp24 > ());
		m_filters.push_back (std::make_unique < Hp6 > ());
		m_filters.push_back (std::make_unique < Hp12 > ());
		m_filters.push_back (std::make_unique < Hp24 > ());
		m_filters.push_back (std::make_unique < LowShelf > ());
		m_filters.push_back (std::make_unique < HighShelf > ());
		m_filters.push_back (std::make_unique < PeakFilter > ());
		m_filters.push_back (std::make_unique < Bp12 > ());
		m_filters.push_back (std::make_unique < Bs12 > ());

		setType (typeStings ().at (0));
	}

	~MultiFilter ()
	{
	}

	bool setType (std::string type)
	{
		for (auto i = 0; i < typeStings ().size (); ++i)
		{
			if (type.compare (typeStings ().at (i)) == 0)
			{
				m_filters.at (i)->setSampleRate (m_sampleRate);
				m_filters.at (i)->calcCoefficents ();
				m_filters.at (i)->clear ();
				m_currentFilterIndex.store (i);
				return true;
			}
		}
		return false;
	}

	inline void setFrequency (float freq) override { m_filters.at (m_currentFilterIndex.load ())->setFrequency (freq); }

	void setQ (float Q) override { m_filters.at (m_currentFilterIndex.load ())->setQ (Q); }

	void setGain (float proposedGain) override { m_filters.at (m_currentFilterIndex)->setGain (proposedGain); }

	inline void setParameters (float freq, float Q, float proposedGain = 1.0) override
	{
		m_filters.at (m_currentFilterIndex.load ())->setParameters (freq, Q, proposedGain);
	}

	inline float processSample (float in) override
	{
		return m_filters.at (m_currentFilterIndex.load ())->processSample (in);
	}

	inline void clear () override { m_filters.at (m_currentFilterIndex.load ())->clear (); }

	void calcCoefficents () override
	{
		m_filters.at (m_currentFilterIndex.load ())->calcCoefficents ();
	}

	bool getUseGain (int index)
	{
		return m_filters.at (index)->getUseGain ();
	}

	bool getUseQ (int index)
	{
		return m_filters.at (index)->getUseQ ();
	}

	bool getUseGain () noexcept override
	{
		return false;
	}

	bool getUseQ () noexcept override
	{
		return false;
	}

private:
	std::vector<std::unique_ptr<Filter>> m_filters;
	//Filter* m_currentFilter;
	std::atomic_int m_currentFilterIndex{ 0 };
	static_assert (std::atomic_int::is_always_lock_free);

	// Inherited via Filter

};