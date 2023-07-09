/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbAudioProcessor::ReverbAudioProcessor()

: myValueTreeState(*this, nullptr, juce::Identifier("myParameter"),
        {
    std::make_unique<juce::AudioParameterFloat>("size", "Size", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("dump", "Dump", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f),
    std::make_unique<juce::AudioParameterBool>("freeze", "Freeze", false)
    
    
})
{
    sizePtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("size"));
    dampPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("dump"));
    widthPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("width"));
    mixPtr = dynamic_cast<juce::AudioParameterFloat*>(myValueTreeState.getParameter("mix"));
    freezePtr = dynamic_cast<juce::AudioParameterBool*>(myValueTreeState.getParameter("freeze"));
        
}

ReverbAudioProcessor::~ReverbAudioProcessor()
{
}

//==============================================================================
const juce::String ReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbAudioProcessor::getNumPrograms()
{
    return 1;
    
}

int ReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mySpec.sampleRate = sampleRate;
    mySpec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    mySpec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    
    
    myReverb.prepare(mySpec);
    //myReverb.reset();
    
}

void ReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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


void ReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

   // get blocksize via the dsp::AudioBlock:
    auto myBlock = juce::dsp::AudioBlock<float>(buffer);
    
    //replace buffer context:
    auto myContext = juce::dsp::ProcessContextReplacing<float>(myBlock);
    
    //Reverb Process for context replace:
    myReverb.process(myContext);
    
    
    //Set Reverb Params though the juce::Reverb::Parameters class:
    params.roomSize   = sizePtr->get(); //* 0.01f;
    params.damping    = dampPtr->get(); // * 0.01f;
    params.width      = widthPtr->get(); //* 0.01f;
    params.wetLevel   = mixPtr->get(); //* 0.01f;
    params.dryLevel   = 1.0f - mixPtr->get(); // * 0.01f;
    params.freezeMode = freezePtr->get();
    //Update param state:
    myReverb.setParameters (params);
}

//==============================================================================
bool ReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbAudioProcessor::createEditor()
{
   // return new ReverbAudioProcessorEditor (*this);
    
    //Setup a generic GUI:
    return new juce::GenericAudioProcessorEditor (*this);
    
}

//==============================================================================
void ReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbAudioProcessor();
}
