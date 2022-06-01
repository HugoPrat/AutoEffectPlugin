/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AutoEffectsAudioProcessorEditor::AutoEffectsAudioProcessorEditor (AutoEffectsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    numberOfFilter.reset (new juce::Label ("numberOfFilter",
                                        "0 filter on"));
    addAndMakeVisible (numberOfFilter.get());
    numberOfFilter->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    numberOfFilter->setJustificationType (juce::Justification::centredLeft);
    numberOfFilter->setEditable (false, false, false);
    numberOfFilter->setColour (juce::Label::textColourId, juce::Colour (0x96ffffff));
    numberOfFilter->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    numberOfFilter->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    
    numberOfFilter->setBounds (180, 100, 80, 24);
    
    add.reset (new juce::TextButton ("add"));
    addAndMakeVisible (add.get());
    add->setButtonText (TRANS("+"));
    add->addListener (this);
    add->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00a45c94));

    add->setBounds (140, 150, 40, 40);
    
    remove.reset (new juce::TextButton ("remove"));
    addAndMakeVisible (remove.get());
    remove->setButtonText (TRANS("-"));
    remove->addListener (this);
    remove->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00a45c94));

    remove->setBounds (220, 150, 40, 40);
    
    setSize (400, 300);
    
    startTimer(50.);
}

AutoEffectsAudioProcessorEditor::~AutoEffectsAudioProcessorEditor()
{
    add = nullptr;
    remove = nullptr;
    
    numberOfFilter = nullptr;
    
    stopTimer();
}

//==============================================================================
void AutoEffectsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour (0xe2000000));
}

void AutoEffectsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void AutoEffectsAudioProcessorEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == add.get()) {
        
        if (audioProcessor.NeedToUpdateGraph)
            return;
        audioProcessor.setNumberOfEffect(audioProcessor.getNumberOfEffect() + 1);
        
        numberOfFilter->setText(juce::String(audioProcessor.getNumberOfEffect()) + " filter on", juce::NotificationType::dontSendNotification);
        
    } else if (buttonThatWasClicked == remove.get()) {
        
        if (audioProcessor.NeedToUpdateGraph)
            return;
        audioProcessor.setNumberOfEffect(audioProcessor.getNumberOfEffect() - 1);
        
        numberOfFilter->setText(juce::String(audioProcessor.getNumberOfEffect()) + " filter on", juce::NotificationType::dontSendNotification);
        
    }
    
}
