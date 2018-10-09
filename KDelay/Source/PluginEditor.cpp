/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KdelayAudioProcessorEditor::KdelayAudioProcessorEditor (KdelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
	int colorMult = 196;
	auto& params = processor.getParameters();
	for (auto pa : params)
	{

		if (auto* param = dynamic_cast<AudioParameterFloat*> (pa))
		{
			Slider* aSlider;

			paramSliders.add(aSlider = new Slider(param->name));


			aSlider->setRange(param->range.start, param->range.end);
			if (aSlider->getName() == "Threshold") {
				aSlider->setSkewFactor(1.5f);
			}
			else {
				aSlider->setSkewFactor(0.25f);
			}

			aSlider->setSliderStyle(Slider::LinearBar);

			aSlider->setColour(Slider::ColourIds::trackColourId, Colour(0, colorMult, colorMult));
			aSlider->setColour(Slider::ColourIds::textBoxOutlineColourId, Colours::black);
			aSlider->setValue(*param);

			aSlider->onValueChange = [this, aSlider] { changeSliderValue(aSlider); };
			aSlider->onDragStart = [this, aSlider] { startDragChange(aSlider); };
			aSlider->onDragEnd = [this, aSlider] { endDragChange(aSlider); };

			addAndMakeVisible(aSlider);

			Label* aLabel;
			paramLabels.add(aLabel = new Label(param->name, param->name));
			aLabel->setColour(Label::ColourIds::backgroundColourId, Colour(0, colorMult, colorMult));
			aLabel->setColour(Label::ColourIds::outlineColourId, Colours::black);
			addAndMakeVisible(aLabel);
			colorMult -= 10;
		}

	}
	noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
	noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));

	//Resizeable
	setResizable(true, true);
	setResizeLimits(300, paramControlHeight * paramSliders.size(), 12000, paramControlHeight * paramSliders.size());
	//
	if (paramSliders.size() == 0)
	{
		addAndMakeVisible(noParameterLabel);
		setSize(300, 100);
	}
	else
	{
		setSize(paramSliderWidth + paramLabelWidth,
			jmax(100, paramControlHeight * paramSliders.size()));

		startTimer(100);
	}
}


KdelayAudioProcessorEditor::~KdelayAudioProcessorEditor()
{
}

//==============================================================================
void KdelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("K-Delay", getLocalBounds(), Justification::centred, 1);
}

void KdelayAudioProcessorEditor::resized()
{
	auto r = getLocalBounds();
	noParameterLabel.setBounds(r);

	for (auto i = 0; i < paramSliders.size(); ++i)
	{
		auto paramBounds = r.removeFromTop(paramControlHeight);
		auto labelBounds = paramBounds.removeFromLeft(paramLabelWidth);

		paramLabels[i]->setBounds(labelBounds);
		paramSliders[i]->setBounds(paramBounds);
	}
}

void KdelayAudioProcessorEditor::changeSliderValue(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		*param = (float)slider->getValue();
}

void KdelayAudioProcessorEditor::startDragChange(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		param->beginChangeGesture();
}

void KdelayAudioProcessorEditor::endDragChange(Slider * slider)
{
	if (auto* param = getParameterForSlider(slider))
		param->endChangeGesture();
}

AudioParameterFloat * KdelayAudioProcessorEditor::getParameterForSlider(Slider * slider)
{
	auto& params = getAudioProcessor()->getParameters();
	return dynamic_cast<AudioParameterFloat*> (params[paramSliders.indexOf(slider)]);
}

void KdelayAudioProcessorEditor::timerCallback()
{
	auto& params = getAudioProcessor()->getParameters();

	for (auto i = 0; i < params.size(); i++) {
		if (auto* param = dynamic_cast<AudioParameterFloat*> (params[i])) {
			if (i < paramSliders.size())
				paramSliders[i]->setValue(*param);
		}
	}
}
