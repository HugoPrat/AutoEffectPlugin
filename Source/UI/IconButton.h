/*
  ==============================================================================

    IconButton.h
    Created: 20 Apr 2022 3:40:13pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#include "CustomJuceHeader.h"

class iconButton   : public juce::Timer, public juce::Component
{
public:
    iconButton(Component *c)
    : content(dynamic_cast<ImageButton*>(c))
    {
        content->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(content);
    }
    
    ~iconButton() = default;
    
    void mouseDown(const MouseEvent &event) override
    {
        startTimer(100);
        content->triggerClick();
    }
    
    void mouseUp(const MouseEvent &event) override
    {
        timeholded = 0;
        
        stopTimer();
    }
    
    void timerCallback() override
    {
        timeholded += getTimerInterval();
        
        if (timeholded >= timeToCallback) {
            if (listener != nullptr)
                listener->iconButtonDidGetHolded(this->content);
            timeholded = 0;
            stopTimer();
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        float size = bounds.getHeight();
        
        size = size * 0.2f;
        bounds = bounds.reduced(size);

        content->setBounds(bounds);
    }

    ///In Milliseconds
    void setTimeToHoldBeforeCallBack(int value) { timeToCallback = value; }

    //===================================================
    // MARK: - Listener
    //===================================================
    
    class JUCE_API  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;
        
        virtual void iconButtonDidGetHolded(ImageButton *) {};
    };
    
    /** The listener that will reply to callbacks */
    Listener *listener;
    
private:

    ImageButton *content;
    
    ///time the Icon has been hold, in milisecond
    int timeholded = 0;

    ///Time before calling callback of holding
    int timeToCallback = 500;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (iconButton)
};
