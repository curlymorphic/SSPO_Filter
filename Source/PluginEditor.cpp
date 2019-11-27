/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sspo_filterAudioProcessorEditor::Sspo_filterAudioProcessorEditor (Sspo_filterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (p.parameters)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setLookAndFeel(&sspoLookAndFeel);
	setSize (300, 160);
	cutoffLabel.setText("Cutoff", dontSendNotification);
	addAndMakeVisible(cutoffLabel);

	addAndMakeVisible(cutoffSlider);
	cutoffSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	cutoffSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow,false, 100,30);
	cutoffAttachement.reset(new SliderAttachment(valueTreeState, "cutoff", cutoffSlider));
	resLabel.setText("Q", dontSendNotification);
	addAndMakeVisible(resLabel);
	addAndMakeVisible(resSlider);
	resSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	resSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow,false,100,30);
	resAttachment.reset(new SliderAttachment(valueTreeState, "res", resSlider));

	addAndMakeVisible(gainSlider);
	gainSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 30);
	gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));

	addAndMakeVisible(typeCombo);
	StringArray filterTypes;
	for (auto s : MultiFilter::TYPES()) filterTypes.add(s);
	typeCombo.addItemList(filterTypes, 1);
	typeCombo.setSelectedId(1);
	typeAttachment.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(valueTreeState, "type", typeCombo));
}

Sspo_filterAudioProcessorEditor::~Sspo_filterAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

//==============================================================================
void Sspo_filterAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
	cutoffSlider.setBounds(0, 0, 100, 130);
	resSlider.setBounds(100, 0, 100, 130);
	cutoffLabel.setBounds(0, 130, 100, 30);
	resLabel.setBounds(100, 130, 100, 30);
	typeCombo.setBounds(200, 130, 100, 30);
	gainSlider.setBounds(200, 0, 100, 130);
}

void Sspo_filterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
