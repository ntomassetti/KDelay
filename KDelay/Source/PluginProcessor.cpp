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
	addParameter(LeftChDelLength = new AudioParameterFloat("leftChDelLength", "Left Channel Delay Length", 0.00001f, 2.0f, 0.5f));
	addParameter(RightChDelLength = new AudioParameterFloat("rightChDelLength", "Right Channel Delay Length", 0.00001f, 2.0f, 0.5f));
	addParameter(Feedback = new AudioParameterFloat("feedback", "Feedback", 0.001f, 0.99999999f, 0.25f));
	addParameter(WetLevel = new AudioParameterFloat("wetLevel", "Wet Level", 0.0f, 1.0f, 1.0f));
	addParameter(DryLevel = new AudioParameterFloat("dryLevel", "Dry Level", 0.0f, 1.0f, 1.0f));
	addParameter(EnvFollowerAttack = new AudioParameterFloat("EnvAttack", "Gate Attack Time", 0.0001f, 5.0f, 0.1f));
	addParameter(EnvFollowerRelease = new AudioParameterFloat("EnvRelease", "Gate Release Time", 0.0001f, 5.0f, 0.1f));
	addParameter(lerpAlpha = new AudioParameterFloat("gaterelease", "Gate Release", 0.0001f, 1.0f, .001f));
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

	UpdateParameters();

	envFollower.process(context);
	auto env = envFollower.GetEnvelope();
	//delay "gate" driven by envelope follower
	if(env > ThresholdAsGain){
		auto envVal = env;
		if (envVal >= 1.0f) {
			envVal = 1.0f;
		}
		delay.setWetLevel(env * *WetLevel);
		
		wetMult = ThresholdAsGain;
	}
	else {
		//float newWet = lint(ThresholdAsGain, 0.f, *lerpAlpha);
		if (wetMult > 0.0f)
		{
			wetMult -= 1.0f * *lerpAlpha;//reduces popping
		}
		else {
			wetMult = 0.0f;
		}
		delay.setWetLevel(wetMult * *WetLevel);
	}
	delay.process(context);
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
	MemoryOutputStream(destData, true).writeFloat(*LeftChDelLength);
	MemoryOutputStream(destData, true).writeFloat(*RightChDelLength);
	MemoryOutputStream(destData, true).writeFloat(*Feedback);
	MemoryOutputStream(destData, true).writeFloat(*WetLevel);
	MemoryOutputStream(destData, true).writeFloat(*DryLevel);
	MemoryOutputStream(destData, true).writeFloat(*EnvFollowerAttack);
	MemoryOutputStream(destData, true).writeFloat(*EnvFollowerRelease);
	MemoryOutputStream(destData, true).writeFloat(*lerpAlpha);
}

void KdelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	*Threshold = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*LeftChDelLength = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*RightChDelLength = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*Feedback = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*WetLevel = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*DryLevel = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*EnvFollowerAttack = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*EnvFollowerRelease = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
	*lerpAlpha = MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
}

void KdelayAudioProcessor::UpdateParameters()
{
	delay.setDelayTime(0, *LeftChDelLength);
	delay.setDelayTime(1, *RightChDelLength);
	ThresholdAsGain = dBToGain(*Threshold);
	envFollower.setAttack(*EnvFollowerAttack);
	envFollower.setRelease(*EnvFollowerRelease);
	delay.setFeedback(*Feedback);
	delay.setDryLevel(*DryLevel);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KdelayAudioProcessor();
}
