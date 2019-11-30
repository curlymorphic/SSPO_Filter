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

#include "../JuceLibraryCode/JuceHeader.h"
#include "curlymorphic_sspo.h"
#include <vector>
#include <memory>
using namespace std;

//==============================================================================
/**
*/
class Sspo_filterAudioProcessor  : public AudioProcessor , public AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    Sspo_filterAudioProcessor();
    ~Sspo_filterAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	AudioProcessorValueTreeState parameters;

	bool getFilterUseQ(int index);
	bool getFilterUseGain(int index);
	

private:
    //==============================================================================


	float* resParameter = nullptr;
	float* cutoffParameter = nullptr;
	float* typeParameter = nullptr;
	float* gainParameter = nullptr;
	

	std::vector<std::unique_ptr<MultiFilter>> m_filters;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sspo_filterAudioProcessor)

		// Inherited via Listener
		virtual void parameterChanged(const String& parameterID, float newValue) override;
};
