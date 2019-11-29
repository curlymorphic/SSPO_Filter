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
	for (auto s : MultiFilter::typeStings()) filterTypes.add(s);
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
