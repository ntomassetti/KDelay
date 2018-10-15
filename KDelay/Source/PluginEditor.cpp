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
	int colorMult = 140;
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


			if (param->name == "Filter Type") {
				aSlider->setSkewFactor(1.0f);
				aSlider->setRange(param->range.start, param->range.end, 1);
				aSlider->textFromValueFunction = [](double value) {
					String filterType;
					switch ((int)value)
					{
					case 0:
						filterType = "Lowpass Filter";
						break;
					case 1:
						filterType = "Highpass Filter";
						break;
					case 2:
						filterType = "Bandpass Filter";
						break;
					default:
						filterType = "Lowpass Filter";
						break;
					}
					return filterType;
				};
			}

			aSlider->setSliderStyle(Slider::LinearBar);

			aSlider->setColour(Slider::ColourIds::trackColourId, Colour(colorMult, colorMult, colorMult));
			aSlider->setColour(Slider::ColourIds::textBoxOutlineColourId, Colours::black);
			aSlider->setValue(*param);

			aSlider->onValueChange = [this, aSlider] { changeSliderValue(aSlider); };
			aSlider->onDragStart = [this, aSlider] { startDragChange(aSlider); };
			aSlider->onDragEnd = [this, aSlider] { endDragChange(aSlider); };

			addAndMakeVisible(aSlider);

			Label* aLabel;
			paramLabels.add(aLabel = new Label(param->name, param->name));
			aLabel->setColour(Label::ColourIds::backgroundColourId, Colour(colorMult, colorMult, colorMult));
			aLabel->setColour(Label::ColourIds::outlineColourId, Colours::black);
			addAndMakeVisible(aLabel);
			colorMult -= 10;
			
		}
		if (auto* param = dynamic_cast<AudioParameterBool*> (pa)) {
			ToggleButton* Toggle;
			paramToggles.add(Toggle = new ToggleButton(param->name));
			Toggle->onClick = [this, Toggle] { updateToggleState(Toggle, "Delay Synch"); };
			addAndMakeVisible(Toggle);
			Label* aLabel;
			paramLabels.add(aLabel = new Label(param->name, param->name));
			aLabel->setColour(Label::ColourIds::backgroundColourId, Colour(colorMult, colorMult, colorMult));
			aLabel->setColour(Label::ColourIds::outlineColourId, Colours::black);
			addAndMakeVisible(aLabel);
		}

	}
	noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
	noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));

	//Resizeable
	setResizable(true, true);
	//setResizeLimits(300, paramControlHeight * paramSliders.size(), 12000, paramControlHeight * paramSliders.size());
	//
	if (paramSliders.size() == 0)
	{
		addAndMakeVisible(noParameterLabel);
		setSize(300, 100);
	}
	else
	{
		setSize(paramSliderWidth + paramLabelWidth,
			jmax(100, paramControlHeight * paramLabels.size()));

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
    g.fillAll (Colour::fromRGB(25,25,25));
	Image KBGFace = ImageCache::getFromMemory(BinaryData::KFace_png, BinaryData::KFace_pngSize);
	g.setTiledImageFill(KBGFace, 0, 0, .25f);
	g.fillAll();

}

void KdelayAudioProcessorEditor::resized()
{
	auto r = getLocalBounds();
	noParameterLabel.setBounds(r);


	for (auto i = 0; i < paramLabels.size(); ++i)
	{
		auto paramBounds = r.removeFromTop(paramControlHeight);
		auto labelBounds = paramBounds.removeFromLeft(paramLabelWidth);
		if (i < paramSliders.size()) {
			paramLabels[i]->setBounds(labelBounds);
			paramSliders[i]->setBounds(paramBounds);
		}
		else {
			paramLabels[i]->setBounds(labelBounds);
			paramToggles[0]->setBounds(paramBounds);
		}
		
	}
	
	//paramLabels[paramSliders.size() + 1]->setBounds(labelBounds);
	//
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

void KdelayAudioProcessorEditor::updateToggleState(Button * button, String name)
{
	auto& params = getAudioProcessor()->getParameters();
	auto state = button->getToggleState();
	if (name == "Delay Synch") {
		if (auto* param = dynamic_cast<AudioParameterBool*>(params.getLast()))
		{
			*param = state;
			for (auto i = 0; i < paramSliders.size(); i++) {
				if (paramSliders[i]->getName() == "Right Channel Delay Length") {
						paramSliders[i]->setEnabled(!*param);

				}
			}
		}

	}

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
