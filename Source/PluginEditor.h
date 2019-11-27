/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class SspoLookAndFeel : public LookAndFeel_V4
{
public:
	SspoLookAndFeel()
	{
		setColour(Slider::thumbColourId, Colours::red);
	}
};


//==============================================================================
/**
*/
class Sspo_filterAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Sspo_filterAudioProcessorEditor (Sspo_filterAudioProcessor&);
    ~Sspo_filterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

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
	std::unique_ptr<SliderAttachment> cutoffAttachement;
	std::unique_ptr<SliderAttachment> resAttachment;
	std::unique_ptr<SliderAttachment> gainAttachment;
	std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> typeAttachment;

	SspoLookAndFeel sspoLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sspo_filterAudioProcessorEditor)
};


