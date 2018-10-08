/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KdelayAudioProcessor::KdelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

	//Params
	addParameter(Threshold = new AudioParameterFloat("threshold", "Threshold", -96.0f, 6.0f, -16.0f));

}

KdelayAudioProcessor::~KdelayAudioProcessor()
{
}

//==============================================================================
const String KdelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KdelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KdelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KdelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KdelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KdelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KdelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KdelayAudioProcessor::setCurrentProgram (int index)
{
}

const String KdelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void KdelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void KdelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	auto channels = static_cast<uint32> (jmin(getMainBusNumInputChannels(), getMainBusNumOutputChannels()));
	dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), channels };

	envFollower.prepare(spec);
	delay.prepare(spec);
	// Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void KdelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KdelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void KdelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	dsp::AudioBlock<float> block(buffer);
	dsp::ProcessContextReplacing<float> context(block);

	envFollower.process(context);
	auto env = envFollower.GetEnvelope();
	//delay "gate" driven by envelope follower
	if(env > dBToGain(*Threshold)){
		delay.setWetLevel(env);
		delay.process(context);
	}
	else {
		//smoothly goes from threshold to 0
		float newWet = lint(dBToGain(*Threshold), 0.f, .33f);
		delay.setWetLevel(0);
		delay.process(context);
	}
	DBG(dBToGain(*Threshold));
}

//==============================================================================
bool KdelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* KdelayAudioProcessor::createEditor()
{
    return new KdelayAudioProcessorEditor (*this);
}

//==============================================================================
void KdelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	MemoryOutputStream(destData, true).writeFloat(*Threshold);
}

void KdelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	*Threshold = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KdelayAudioProcessor();
}
