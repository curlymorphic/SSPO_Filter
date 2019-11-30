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
#include "PluginProcessor.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;




//==============================================================================
/**
*/
class Sspo_filterAudioProcessorEditor : public AudioProcessorEditor, public AudioProcessorValueTreeState::Listener, ImageButton::Listener
{
public:
	Sspo_filterAudioProcessorEditor (Sspo_filterAudioProcessor&);
	~Sspo_filterAudioProcessorEditor ();

	//==============================================================================
	void paint (Graphics&) override;
	void resized () override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	Sspo_filterAudioProcessor& processor;

	AudioProcessorValueTreeState& valueTreeState;
	Label cutoffLabel;
	Slider cutoffSlider;
	Label resLabel;
	Slider resSlider;
	Slider gainSlider;
	ComboBox typeCombo;
	Label gainLabel;
	ImageButton gitHubSocialButton;
	std::unique_ptr<SliderAttachment> cutoffAttachement;
	std::unique_ptr<SliderAttachment> resAttachment;
	std::unique_ptr<SliderAttachment> gainAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;

	SspoLookAndFeel sspoLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sspo_filterAudioProcessorEditor)

		// Inherited via Listener
		void parameterChanged (const String& parameterID, float newValue) override;

	// Inherited via Listener
	void buttonClicked (Button*) override;
};


