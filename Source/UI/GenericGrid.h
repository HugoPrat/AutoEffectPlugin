/*
  ==============================================================================

    GenericGrid.h
    Created: 4 Mar 2022 3:47:54pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#include "ResponsiveLabel.h"
#include "IconButton.h"


struct ExceptionGrid {
    int index;
    GridItem item;
};

class GenericGrid   :   public juce::Component, public iconButton::Listener
{
public:
    GenericGrid(Array<Grid::TrackInfo> row, Array<Grid::TrackInfo> col, Array<Component*> c, Array<ExceptionGrid> e, Array<int> labels = {}, Array<int> icons = {})
    :   templateRows(row), templateColumns(col), content(c), exceptions(e), backgroundColour(Colours::transparentBlack)
    {
        //bound.reset(new juce::GroupComponent ("z", "bounds"));
        //addAndMakeVisible(bound.get());
        
        if (labels.size() > 0) {
            for (int i = 0; i< labels.size(); i++) {
                customContent.add( new ResponsiveLabel(content[labels[i]]));
                addAndMakeVisible(customContent[i]);
                if (labels[i] < content.size())
                    content.set(labels[i], customContent.getLast());
            }
        }
        
        if (icons.size() > 0) {
            for (int i = 0; i< icons.size(); i++) {
                customContent.add( new iconButton(content[icons[i]]));
                addAndMakeVisible(customContent[i]);
                dynamic_cast<iconButton*>(customContent[i])->listener = this;
                if (icons[i] < content.size())
                    content.set(icons[i], customContent.getLast());
            }
        }
    }
    
    ~GenericGrid() {
        for (int i = 0; i < customContent.size(); i++)
            delete customContent[i];
    };

    void paint (juce::Graphics& g) override
    {
//        g.fillAll (backgroundColour);
        g.setColour(backgroundColour);
        g.fillRoundedRectangle (getLocalBounds().toFloat(), cornerSize);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        Array<ExceptionGrid> exceptionsCopy = exceptions;
        
        //bound->setBounds(getLocalBounds());
        
        juce::Grid grid;
        //using Track = juce::Grid::TrackInfo;
        
        bounds = bounds.reduced(margin);
        grid.rowGap = Grid::Px(rowGap);
        grid.columnGap = Grid::Px(columnGap);
        grid.templateRows    = templateRows;
        grid.templateColumns = templateColumns;

        for (int i =0; i < content.size(); i++) {
            addAndMakeVisible(content[i]);
            // if there are exception GridItem and its index is the same as current GridItem
            if (exceptionsCopy.size() > 0 && exceptionsCopy.begin()->index == i) {
                // add GridItem with params in exception array instead of creating a default one
                GridItem tmp = exceptionsCopy.begin()->item;
                tmp.associatedComponent = content[i];
                grid.items.add(tmp);
                //remove first Exception to check next one
                exceptionsCopy.remove(0);
            } else {
                grid.items.add(GridItem(content[i]));
            }
        }
        
        grid.performLayout(bounds);
    }
    
    static float getPercentageWidthMargin(float percentageWanted) {
        auto totalSize = Desktop::getInstance().getDisplays().getTotalBounds(true);
        
        return ((percentageWanted / 100.0f) * totalSize.getWidth());
    }
    
    static float getPercentageHeightMargin(float percentageWanted) {
        auto totalSize = Desktop::getInstance().getDisplays().getTotalBounds(true);
        
        return ((percentageWanted / 100.0f) * totalSize.getHeight());
    }
    
    void setBackground(Colour x) { backgroundColour = x; }
    
    void setRowGap(int value) { rowGap = value; }
    
    void setColumnGap(int value) { columnGap = value; }
    
    void setMargin(int value) { margin = value; }
    
    void setCornerSize(float value) { cornerSize = value; }
    
    //===================================================
    // MARK: - Listener
    //===================================================
    
    void iconButtonDidGetHolded(ImageButton *comp) override {
        if (listener != nullptr)
            listener->iconButtonDidGetHolded(comp);
    }
    
    class JUCE_API  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;
        
        virtual void iconButtonDidGetHolded(ImageButton*) {};
    };
    
    /** The listener that will reply to callbacks */
    Listener *listener;
    
private:
    
    //std::unique_ptr<juce::GroupComponent> bound;
    
    Array<Grid::TrackInfo> templateRows;
    Array<Grid::TrackInfo> templateColumns;
    Array<Component*> content;
    Array<Component*> customContent;
    Array<ExceptionGrid> exceptions;
    int rowGap = 0;
    int columnGap = 0;
    int margin = 0;
    float cornerSize = 0;
    
    juce::Colour backgroundColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericGrid)
};
