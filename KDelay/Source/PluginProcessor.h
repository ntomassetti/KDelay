/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EnvFollower.h"
#include "DelayDSP.h"

//==============================================================================
/**
*/
class KdelayAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    KdelayAudioProcessor();
    ~KdelayAudioProcessor();

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

	void UpdateParameters();



private:
	EnvFollower<float> envFollower;
	Delay<float> delay;

	float ThresholdAsGain;

	////////
	//Params
	AudioParameterFloat* Threshold;
	AudioParameterFloat* LeftChDelLength;
	AudioParameterFloat* RightChDelLength;
	AudioParameterFloat* Feedback;
	AudioParameterFloat* DryLevel;
	AudioParameterFloat* WetLevel;

	AudioParameterFloat* EnvFollowerAttack;
	AudioParameterFloat* EnvFollowerRelease;

	AudioParameterFloat* lerpAlpha;

	AudioParameterBool* linkDelayLength;

private:
	/*Linear interpolate*/
	float lint(float a, float b, float alpha) {
		return (a * (1.0f - alpha)) + (b * alpha);
	};

	float wetMult;

	/*dB to gain*/
	float dBToGain(float inDB) { return pow(10.0f, inDB / 20.0f); };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KdelayAudioProcessor)
};
