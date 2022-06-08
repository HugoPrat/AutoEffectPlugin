/*
  ==============================================================================

    LoadingWaitingScreen.h
    Created: 25 May 2022 12:03:15pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#ifdef WITH_CMAKE
    #include "CustomJuceHeader.h"
#else
    #include "../JuceLibraryCode/JuceHeader.h"
#endif

class LoadingWaitingScreen : public Component, public Timer {
public:
    LoadingWaitingScreen() = default;
    ~LoadingWaitingScreen() = default;
    
    
    void paint (juce::Graphics& g) override
    {
        g.fillAll (Colours::black.withAlpha(0.5f));
        g.setColour (juce::Colour (0xffc7c7c7));
 
        auto numberOfDots = 15;
 
        juce::Path spinePath;
 
        for (auto i = 0; i < numberOfDots; ++i)
        {
            int radius = 50;
 
            juce::Point<float> p ((float) getWidth()  / 2.0f + 1.0f * (float) radius * std::sin ((float) animationCounter * 0.04f + (float) i * 0.12f),
                                  (float) getHeight() / 2.0f + 1.0f * (float) radius * std::cos ((float) animationCounter * 0.04f + (float) i * 0.12f));
 
            if (i == 0)
                spinePath.startNewSubPath (p);
            else
                spinePath.lineTo (p);
        }
 
        g.strokePath (spinePath, juce::PathStrokeType (4.0f));
    }
        
    ///Make sure that you call stopAnimation() when animation is not needed for better performance
    void startAnimation() {
        animationCounter = 0;
        startTimer(30);
    }
    
    void stopAnimation() {
        animationCounter = 0;
        stopTimer();
    }
    
    void timerCallback() override
    {
        animationCounter += 10;
        repaint();
    }
        
    void resized () override
    {
        repaint();
    }
    
private:
    
    int animationCounter = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoadingWaitingScreen)
};
