/*
  ==============================================================================

    EffectBlock.cpp
    Created: 9 June 2022 8:03:58pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#include "EffectBlocks.h"

ChorusUiBlock::~ChorusUiBlock()
{
    effectProcess = nullptr;
    
    mainGrid = nullptr;
    
    mainLabel = nullptr;
    rateLabel = nullptr;
    depthLabel = nullptr;
    delayLabel = nullptr;
    feedbackLabel = nullptr;
    wetLabel = nullptr;
    rateSlider = nullptr;
    depthSlider = nullptr;
    delaySlider = nullptr;
    feedbackSlider = nullptr;
    wetSlider = nullptr;
}

void ChorusUiBlock::setComponents()
{
    mainLabel.reset (new juce::Label ("mainLabel", "Chorus"));
    addAndMakeVisible (mainLabel.get());
    mainLabel->setFont (juce::Font (35.00f, juce::Font::bold).withTypefaceStyle ("Regular"));
    mainLabel->setJustificationType (juce::Justification::centred);
    mainLabel->setEditable (false, false, false);
    mainLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    rateLabel.reset (new juce::Label ("rateLabel", "Rate"));
    addAndMakeVisible (rateLabel.get());
    rateLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    rateLabel->setJustificationType (juce::Justification::centred);
    rateLabel->setEditable (false, false, false);
    rateLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    depthLabel.reset (new juce::Label ("depthLabel", "Depth"));
    addAndMakeVisible (depthLabel.get());
    depthLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    depthLabel->setJustificationType (juce::Justification::centred);
    depthLabel->setEditable (false, false, false);
    depthLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    delayLabel.reset (new juce::Label ("delayLabel", "Delay"));
    addAndMakeVisible (delayLabel.get());
    delayLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    delayLabel->setJustificationType (juce::Justification::centred);
    delayLabel->setEditable (false, false, false);
    delayLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    feedbackLabel.reset (new juce::Label ("feedbackLabel", "Feedback"));
    addAndMakeVisible (feedbackLabel.get());
    feedbackLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    feedbackLabel->setJustificationType (juce::Justification::centred);
    feedbackLabel->setEditable (false, false, false);
    feedbackLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    wetLabel.reset (new juce::Label ("wetLabel", "Dry/Wet"));
    addAndMakeVisible (mainLabel.get());
    wetLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    wetLabel->setJustificationType (juce::Justification::centred);
    wetLabel->setEditable (false, false, false);
    wetLabel->setColour (juce::Label::textColourId, Colour(113,114,123));

    rateSlider.reset (new juce::Slider ("rateSlider"));
    addAndMakeVisible (rateSlider.get());
    rateSlider->setRange (0, 100, 1);
    rateSlider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    rateSlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 30, 10);
    rateSlider->setValue(effectProcess->getRate());
    rateSlider->addListener (this);
    rateSlider->setLookAndFeel(&lf);
    
    depthSlider.reset (new juce::Slider ("depthSlider"));
    addAndMakeVisible (depthSlider.get());
    depthSlider->setRange (0, 1, 0.01);
    depthSlider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    depthSlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 30, 10);
    depthSlider->setValue(effectProcess->getDepth());
    depthSlider->addListener (this);
    depthSlider->setLookAndFeel(&lf);
    
    delaySlider.reset (new juce::Slider ("delaySlider"));
    addAndMakeVisible (delaySlider.get());
    delaySlider->setRange (1, 100, 1);
    delaySlider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    delaySlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 30, 10);
    delaySlider->setValue(effectProcess->getCentreDelay());
    delaySlider->addListener (this);
    delaySlider->setLookAndFeel(&lf);
    
    feedbackSlider.reset (new juce::Slider ("feedbackSlider"));
    addAndMakeVisible (feedbackSlider.get());
    feedbackSlider->setRange (-1, 1, 0.02);
    feedbackSlider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    feedbackSlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 30, 10);
    feedbackSlider->setValue(effectProcess->getFeedback());
    feedbackSlider->addListener (this);
    feedbackSlider->setLookAndFeel(&lf);
    
    wetSlider.reset (new juce::Slider ("wetSlider"));
    addAndMakeVisible (wetSlider.get());
    wetSlider->setRange (0, 1, 0.01);
    wetSlider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    wetSlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 30, 10);
    wetSlider->setValue(effectProcess->getMix());
    wetSlider->addListener (this);
    wetSlider->setLookAndFeel(&lf);
    
    using Track = juce::Grid::TrackInfo;
    Array<ExceptionGrid> params;
    
    params.add({.index = 1, .item = GridItem().withArea(GridItem::Span(2), {})});
    mainGrid.reset(new GenericGrid({Track (1_fr), Track (3_fr), Track (1_fr), Track (3_fr)}, {Track (1_fr), Track (1_fr), Track (1_fr)},
                                        {rateLabel.get(),
        mainLabel.get(), depthLabel.get(),
        rateSlider.get(), depthSlider.get(), delayLabel.get(), feedbackLabel.get(), wetLabel.get(), delaySlider.get(), feedbackSlider.get(), wetSlider.get()},
                                        params
                                        ));
    mainGrid->setBackground(juce::Colour (245, 246, 252));
    mainGrid->setCornerSize(10.f);
    addAndMakeVisible(*mainGrid);
    params.clear();
}

void ChorusUiBlock::paint (juce::Graphics&)
{
    ///Nothing to do here
}

void ChorusUiBlock::resized()
{
    mainGrid->setBounds(getLocalBounds());
}

void ChorusUiBlock::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == rateSlider.get()) {
        effectProcess->setRate(float(rateSlider->getValue()));
    }
    else if (sliderThatWasMoved == depthSlider.get()) {
        effectProcess->setDepth(float(depthSlider->getValue()));
    }
    else if (sliderThatWasMoved == delaySlider.get()) {
        effectProcess->setCentreDelay(float(delaySlider->getValue()));
    }
    else if (sliderThatWasMoved == feedbackSlider.get()) {
        effectProcess->setFeedback(float(feedbackSlider->getValue()));
    }
    else if (sliderThatWasMoved == wetSlider.get()) {
        effectProcess->setMix(float(wetSlider->getValue()));
    }
}
