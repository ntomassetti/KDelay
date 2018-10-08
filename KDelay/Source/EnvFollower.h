/*
  ==============================================================================

    EnvFollower.h
    Created: 8 Oct 2018 10:46:41am
    Author:  Nick Tomassetti

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


template <typename Type>
class EnvFollower {
	/*
		Envelope Follower
		
		Make sure to call prepare before processing any data 
		otherwise you might have the incorrect sample rate.
	*/

public:
	EnvFollower() {
		setAttack(0.1);
		setRelease(1.0);
	}

	void prepare(const juce::dsp::ProcessSpec& spec) {
		sampleRate = (Type)spec.sampleRate;

		UpdateCoefficients();
	}

	void setAttack (Type newValue){
		jassert(newValue >= Type(0));
		attackTime = 1000 * newValue;
	}

	void setRelease(Type newValue) {
		jassert(newValue >= Type(0));
		releaseTime = 1000 * newValue;
	}

	void UpdateCoefficients() {
		attackCoef = (Type)exp((Type)log(0.01) / (attackTime * sampleRate * (Type)0.001));
		releaseCoef = (Type)exp((Type)log(0.01) / (releaseTime * sampleRate * (Type)0.001));
	}

	template <typename ProcessContext>
	void process(const ProcessContext& context) noexcept
	{
		auto& inputBlock = context.getInputBlock();
		auto& outputBlock = context.getOutputBlock();
		auto numSamples = outputBlock.getNumSamples();
		auto numChannels = outputBlock.getNumChannels();

		jassert(inputBlock.getNumSamples() == numSamples);
		jassert(inputBlock.getNumChannels() == numChannels);

		for (size_t ch = 0; ch < numChannels; ++ch)
		{
			auto* input = inputBlock.getChannelPointer(ch);

			for (size_t i = 0; i < numSamples; ++i) {
				auto inputSample = input[i];
				auto tmp = fabsf(inputSample);
				if (tmp > envelope) {
					envelope = attackCoef * (envelope - tmp) + tmp;
				}
				else {
					envelope = releaseCoef * (envelope - tmp) + tmp;
				}
			}
		}
	}

	/*Returns the envelope from 0 to infinity (if you are red lining it'll be 1+)*/
	Type GetEnvelope(){
		return envelope;
	}

private:

	Type attackTime{ Type(0.05) };
	Type releaseTime{ Type(2.0) };

	Type attackCoef;
	Type releaseCoef;

	Type envelope{ Type(0.1) };

	Type sampleRate{ Type(44.1e3) };
};