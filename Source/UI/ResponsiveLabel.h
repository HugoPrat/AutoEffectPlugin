/*
  ==============================================================================

    ResponsiveLabel.h
    Created: 16 Mar 2022 11:33:29am
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#include "CustomJuceHeader.h"

class ResponsiveLabel   : public juce::Component
{
public:
    ResponsiveLabel(Component *c)
    : content(dynamic_cast<juce::Label*>(c))
    {
        //bound.reset(new juce::GroupComponent ("z", "bounds"));
        //addAndMakeVisible(bound.get());
        
        text = content->getText();
        if (text.contains("\n")) {
            StringArray tokens;
            tokens.addTokens (text, "\n");
            text = tokens[0];
            for (int i = 1; i < tokens.size(); i++) {
                if (text.length() < tokens[i].length())
                    text = tokens[i];
            }
        }
        
        font = content->getFont();
        addAndMakeVisible(content);
    }
    ~ResponsiveLabel() = default;
    
    void paint(Graphics& g) override
    {
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto totalSize = Desktop::getInstance().getDisplays().getTotalBounds(true);
        
        //bound->setBounds(getLocalBounds());

        float w = bounds.getWidth();
        //bound->setText(String(w));
        
        // target a smaller width for text, taking into account Gap in GenericGrid
        w = (w - (10.0f / 100.0f) * w) - (1.0f / 100.0f) * totalSize.getWidth();
        float fontSize = font.getHeight();

        font.setHorizontalScale(1);

        float fw = font.getStringWidth(text);
        if (fw >= w) {
            while (fw + 0.5f >= w && fontSize >= 0.0f) {
                fontSize -= 0.50;
                font.setHeight(fontSize);
                fw = font.getStringWidth(text);
            }
        } else if (fw <= w) {
            while (fw <= w) {
                fontSize += 0.25;
                font.setHeight(fontSize);
                fw = font.getStringWidth(text);
            }
            fontSize -= 0.50;
            font.setHeight(fontSize);
            fw = font.getStringWidth(text);
        }
        //        font.setHeight(fontSize - 1);
//        if (fw > w)
//            scale = w/fw;
/*        if (scale < minScale)
        {
            int h2 = (int)((w/minScale)/fw * h);
            font.setHeight(h2);
            fw = font.getStringWidth(text);
            if (fw > w)
                scale = w/fw;
        }*/
//        font.setHorizontalScale(scale);
        
        content->setFont(font);
        content->setBounds(bounds);
    }

private:
    //std::unique_ptr<juce::GroupComponent> bound;
    
    Label *content;
    String text;
    Font font;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResponsiveLabel)
};
