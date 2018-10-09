/*
  ==============================================================================

    MyLookAndFeel.h
    Created: 8 Oct 2018 4:54:15pm
    Author:  Studio 5

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class MyLookAndFeel : public LookAndFeel_V4
{
public:
	MyLookAndFeel();


	void drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPosProportional,
							const float rotaryStartAngle, const float rotaryEndAngle, Slider &slider) override;

	Image knobNormal, knobOver, knobMask;
};