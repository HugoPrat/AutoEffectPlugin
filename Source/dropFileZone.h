/*
  ==============================================================================

    dropFileZone.h
    Created: 20 Apr 2022 3:40:13pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#include "CustomJuceHeader.h"

class dropFileZone   : public juce::Component, public FileDragAndDropTarget
{
public:
    dropFileZone() = default;
    ~dropFileZone() = default;
    
    void paint (juce::Graphics& g) override
    {
        if (somethingIsBeingDraggedOver)
        {
            g.setColour(outlineColor);
            g.drawRoundedRectangle(getLocalBounds().toFloat(), cornerSize, 5.f);
        }
    }
    
    void resized() override
    {
        repaint();
    }
    
    //==============================================================================
    // FileDragAndDropTarget interface
    //==============================================================================
    
    bool isInterestedInFileDrag (const StringArray& files) override
    {
        if (formats.isEmpty())
            return true;
        bool isformat = false;
        
        for (auto fileName : files) {
            for (auto format : formats)
                if (fileName.endsWith (format))
                    isformat = true;
            
            ///Make sure that each files is in right format
            if (!isformat)
                return isformat;
            else
                isformat = false;
        }
        
        return isformat;
        
    }
    
    void fileDragEnter (const StringArray& /*files*/, int /*x*/, int /*y*/) override
    {
        somethingIsBeingDraggedOver = true;
        repaint();
    }
    
    void fileDragMove (const StringArray& /*files*/, int /*x*/, int /*y*/) override
    {
    }
    
    void fileDragExit (const StringArray& /*files*/) override
    {
        somethingIsBeingDraggedOver = false;
        repaint();
    }
    
    void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override
    {
        somethingIsBeingDraggedOver = false;
        if (listener != nullptr)
            listener->fileBeingDropInZone(files);
        repaint();
    }
    
    //===================================================
    // MARK: - Listener
    //===================================================
    
    class JUCE_API  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;
        
        virtual void fileBeingDropInZone(const StringArray& /*files*/) {}
    };
    
    /** The listener that will reply to callbacks */
    Listener *listener;
    
    void setColorOutline(Colour x) { outlineColor = x; }
    
    void setCornerSize(float value) { cornerSize = value; }
    
    void setFormats(StringArray value) { formats = value; }
    
private:
    
    /** Whether something is being dragged over */
    bool somethingIsBeingDraggedOver = false;
    
    juce::Colour outlineColor = Colours::red;
    
    float cornerSize = 0;
    
    ///format of files wanted to be drop ex {.wav, .mp3}
    StringArray formats;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (dropFileZone)
};
