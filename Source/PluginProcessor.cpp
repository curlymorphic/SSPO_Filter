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
#include <vector>
using namespace std;

//==============================================================================
Sspo_filterAudioProcessor::Sspo_filterAudioProcessor() : parameters(*this, nullptr, Identifier("SSPO_Filter"),
	{	})
#ifndef JucePlugin_PreferredChannelConfigurations
     , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif

{

	//initilise filters
	//check how many output channels
	auto channelCount = getTotalNumOutputChannels();
	for (auto i = 0; i < channelCount; ++i)
	{
		m_filters.push_back(new MultiFilter());
	}

	auto cutoffRange = NormalisableRange<float>(20.0f, 20000.0f, 1.0f);
	cutoffRange.setSkewForCentre(1000);
	auto resRange = NormalisableRange<float>(0.001f, 5.0f, 0.001f);
	auto gainRange = NormalisableRange<float>(-30.0f, +30.0f, 0.001f);
	gainRange.setSkewForCentre(1.0);

	StringArray filterTypes;
	for (auto s : MultiFilter::TYPES()) filterTypes.add(s);

	parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("cutoff", "Cutoff", cutoffRange, 20000));
	parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("res", "Resonance", resRange, 0.707));
	parameters.createAndAddParameter(std::make_unique<AudioParameterChoice>("type", "Filter Type", filterTypes, 0));
	parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("gain", "Gain", gainRange, 1.0));
	resParameter = parameters.getRawParameterValue("res");
	cutoffParameter = parameters.getRawParameterValue("cutoff");
	typeParameter = parameters.getRawParameterValue("type");
	gainParameter = parameters.getRawParameterValue("gain");
	parameters.addParameterListener("cutoff", this);
	parameters.addParameterListener("res", this);
	parameters.addParameterListener("gain", this);
	parameters.addParameterListener("type", this);


}

Sspo_filterAudioProcessor::~Sspo_filterAudioProcessor()
{
	for (auto f : m_filters) {delete f;}

}


//==============================================================================
const String Sspo_filterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Sspo_filterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Sspo_filterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Sspo_filterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Sspo_filterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Sspo_filterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Sspo_filterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Sspo_filterAudioProcessor::setCurrentProgram (int index)
{
}

const String Sspo_filterAudioProcessor::getProgramName (int index)
{
    return {};
}

void Sspo_filterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Sspo_filterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	for (auto f : m_filters)
	{ 
		f->setType(MultiFilter::TYPES()[*typeParameter]);
		f->setSampleRate(sampleRate); 
		f->setParameters(*cutoffParameter, *resParameter, *gainParameter);
	}
}

void Sspo_filterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Sspo_filterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Sspo_filterAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

	

	for (auto j = 0; j < buffer.getNumChannels(); j++)
	{
		auto channelData = buffer.getWritePointer(j);
		for (auto i = 0; i < buffer.getNumSamples(); ++i)
		{
			auto x = channelData[i];
			auto f = m_filters.at(j);
			x = f->processSample(x);
			channelData[i] = x;
		}
	}
}

//==============================================================================
bool Sspo_filterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Sspo_filterAudioProcessor::createEditor()
{
    return new Sspo_filterAudioProcessorEditor (*this);
}

//==============================================================================
void Sspo_filterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void Sspo_filterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(ValueTree::fromXml(*xmlState));
}

void Sspo_filterAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{

	if (parameterID.compare("type") == 0 )
	{
		for (auto f : m_filters)
		{
			f->setType(MultiFilter::TYPES()[newValue]);
			f->clear();
		}
	}

	for (auto f : m_filters) { f->setParameters(*cutoffParameter, *resParameter, *gainParameter); }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Sspo_filterAudioProcessor();
}
