/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AutoEffectsAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::Timer,
                                         public juce::Button::Listener
{
public:
    AutoEffectsAudioProcessorEditor (AutoEffectsAudioProcessor&);
    ~AutoEffectsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // MARK: - Timer class methods
    //==============================================================================

    void timerCallback() override
    {
        
    }
    
    void buttonClicked (juce::Button* buttonThatWasClicked) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoEffectsAudioProcessor& audioProcessor;
    
    std::unique_ptr<juce::Label> numberOfFilter;
    
    std::unique_ptr<juce::TextButton> add;
    std::unique_ptr<juce::TextButton> remove;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoEffectsAudioProcessorEditor)
};
