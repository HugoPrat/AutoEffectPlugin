/*
  ==============================================================================

    EffectBlock.h
    Created: 9 June 2022 8:03:58pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#ifdef WITH_CMAKE
    #include "CustomJuceHeader.h"
#else
    #include "../JuceLibraryCode/JuceHeader.h"
#endif

#include "../EffectProcessors.h"
#include "GenericGrid.h"

class RotaryLookAndFeel : public LookAndFeel_V4
{
public:
    
    RotaryLookAndFeel()
    {        
        setColour(Slider::thumbColourId, Colour (77,94,251));
        setColour(Slider::rotarySliderFillColourId, Colour (77,94,251));
        setColour(Slider::textBoxTextColourId, Colour (113,114,123));
        setColour(Slider::textBoxBackgroundColourId, Colours::transparentWhite);
        setColour(Slider::textBoxHighlightColourId, Colour (77,94,251));
        setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
    }
    
    Slider::SliderLayout getSliderLayout (Slider& slider) override
    {
        juce::Slider::SliderLayout layout = juce::LookAndFeel_V2::getSliderLayout(slider);

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth = layout.textBoxBounds.getWidth();
        auto textBoxHeight = layout.textBoxBounds.getHeight();
        layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);
        layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2 - 5);


        return layout;
    }
    
    Label* createSliderTextBox (Slider& slider) override
    {
        auto* l = LookAndFeel_V4::createSliderTextBox (slider);

        l->setFont(Font (10.00f, juce::Font::bold));
        
        return l;
    }
};

class ChorusUiBlock : public Component,
                      public juce::Slider::Listener
{
public:
    ChorusUiBlock(AudioProcessor* p) {
        if (p != nullptr && p->getName() == "Chorus") {
            effectProcess = dynamic_cast<ChorusProcessor *>(p);
        } else
            throw "BAD Association effect Block UI";
        setComponents();
    }
    ~ChorusUiBlock() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    
    void setComponents();
    
private:

    ///Ptr to effect Node to change his propriety
    ChorusProcessor *effectProcess;
    
    std::unique_ptr<GenericGrid> mainGrid;
    
    std::unique_ptr<Label> mainLabel;
    
    std::unique_ptr<Label> rateLabel;
    std::unique_ptr<Label> depthLabel;
    std::unique_ptr<Label> delayLabel;
    std::unique_ptr<Label> feedbackLabel;
    std::unique_ptr<Label> wetLabel;
    
    std::unique_ptr<Slider> rateSlider;
    std::unique_ptr<Slider> depthSlider;
    std::unique_ptr<Slider> delaySlider;
    std::unique_ptr<Slider> feedbackSlider;
    std::unique_ptr<Slider> wetSlider;
    
    RotaryLookAndFeel lf;
    
    juce::Colour backgroundColour;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChorusUiBlock)
};
