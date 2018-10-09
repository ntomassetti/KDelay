/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 8 Oct 2018 4:54:15pm
    Author:  Studio 5

  ==============================================================================
*/

#include "MyLookAndFeel.h"





MyLookAndFeel::MyLookAndFeel()
{
	knobNormal = ImageCache::getFromMemory(BinaryData::KDelayKnob_norm_png, BinaryData::KDelayKnob_norm_pngSize);
	knobOver = ImageCache::getFromMemory(BinaryData::KDelayKnobHover_png, BinaryData::KDelayKnobHover_pngSize);
	knobMask = ImageCache::getFromMemory(BinaryData::KDelayKnobAlpha_png, BinaryData::KDelayKnobAlpha_pngSize);
}

void MyLookAndFeel::drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional,
										const float rotaryStartAngle, const float rotaryEndAngle, Slider & slider)
{
	AffineTransform rotator;
	g.drawImageTransformed(knobNormal,rotator.rotated((float)sliderPosProportional*rotaryEndAngle,
			(float)(knobMask.getWidth() / 2), (float)(knobMask.getHeight() / 2)), false);
}
