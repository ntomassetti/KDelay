/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MyLookAndFeel.h"
//==============================================================================
/**
*/
class KdelayAudioProcessorEditor  : public AudioProcessorEditor, private Timer
{
public:
    KdelayAudioProcessorEditor (KdelayAudioProcessor&);
    ~KdelayAudioProcessorEditor();


    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void changeSliderValue(Slider* slider);
	void startDragChange(Slider* slider);
	void endDragChange(Slider* slider);

	enum
	{
		paramControlHeight = 40,
		paramLabelWidth = 80,
		paramSliderWidth = 300
	};

	void updateToggleState(Button* button, String name);


private:

	Label noParameterLabel{ "noparam", "No parameters available" };
	OwnedArray<Slider> paramSliders;
	OwnedArray<Label> paramLabels;
	OwnedArray<ToggleButton> paramToggles;

	MyLookAndFeel myLAF;

	AudioParameterFloat* getParameterForSlider(Slider* slider);

	void timerCallback() override;

    KdelayAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KdelayAudioProcessorEditor)
};
