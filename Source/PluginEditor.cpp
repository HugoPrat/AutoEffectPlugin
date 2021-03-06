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
    dropImage.reset(new ImageButton("dropImage"));
    addAndMakeVisible(dropImage.get());
    dropImage->addListener(this);
    dropImage->setImages(false, true, true, ImageCache::getFromMemory (BinaryData::icon_drop_png, BinaryData::icon_drop_pngSize), 1.f, Colour (77,94,251), juce::Image(), 1.000f, juce::Colour (77,94,251), ImageCache::getFromMemory (BinaryData::icon_drop_png, BinaryData::icon_drop_pngSize), 1.f, Colour (77,94,251));
    dropImage->setBounds(0, 0, 100, 54);
    
    cancelButton.reset(new ImageButton("cancelButton"));
    addAndMakeVisible(cancelButton.get());
    cancelButton->addListener(this);
    cancelButton->setImages(false, true, true, ImageCache::getFromMemory (BinaryData::icon_cancel_png, BinaryData::icon_cancel_pngSize), 1.f, Colour (77,94,251), juce::Image(), 1.000f, juce::Colour (77,94,251), ImageCache::getFromMemory (BinaryData::icon_cancel_png, BinaryData::icon_cancel_pngSize), 1.f, Colour (32,42,131));
    cancelButton->setBounds(3, 3, 25, 25);
    
    dropFileLabel.reset (new juce::Label ("dropLabel", TRANS("Drop your target sound here to process")));
    addAndMakeVisible (dropFileLabel.get());
    dropFileLabel->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    dropFileLabel->setJustificationType (juce::Justification::centred);
    dropFileLabel->setEditable (false, false, false);
    dropFileLabel->setColour (juce::Label::textColourId, Colour(113,114,123));
    
    dropFileLabel2.reset (new juce::Label ("dropLabel2", "Or"));
    addAndMakeVisible (dropFileLabel2.get());
    dropFileLabel2->setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    dropFileLabel2->setJustificationType (juce::Justification::centred);
    dropFileLabel2->setEditable (false, false, false);
    dropFileLabel2->setColour (juce::Label::textColourId, Colour(200,206,214));
    
    browseFileButton.reset (new SelectFileButton ("exportMidiButton"));
    addAndMakeVisible (browseFileButton.get());
    browseFileButton->setButtonText (TRANS("Browse files"));
    browseFileButton->listener = this;
    browseFileButton->setColour (juce::TextButton::buttonColourId, juce::Colour (77,94,251));
    browseFileButton->setColour (juce::TextButton::buttonOnColourId, juce::Colour (77,94,251));
    browseFileButton->setColour (juce::TextButton::textColourOffId, juce::Colour(245, 246, 252));
    browseFileButton->setAcceptedTypes({".wav", ".mp3"});
    
    dropZone.reset(new dropFileZone());
    dropZone->setAlwaysOnTop(true);
    dropZone->setCornerSize(10.f);
    dropZone->setColorOutline(Colour(229, 226, 233));
    dropZone->setFormats({".wav", ".mp3"});
    dropZone->listener = this;
    addAndMakeVisible(dropZone.get());
    
    loadingWaitingScreen.reset(new LoadingWaitingScreen());
    addAndMakeVisible(loadingWaitingScreen.get());
    loadingWaitingScreen->setName("loadingWaitingScreen");
    
    loadingWaitingScreen->setAlwaysOnTop(true);
    loadingWaitingScreen->setVisible(false);
    
    using Track = juce::Grid::TrackInfo;
    Array<ExceptionGrid> params;
    
    params.add({.index = 3, .item = GridItem().withMargin(20)});
    mainGrid.reset(new GenericGrid({Track (3_fr), Track (1_fr), Track (1_fr), Track (3_fr)}, {Track (1_fr)},
                                        {dropImage.get(), dropFileLabel.get(), dropFileLabel2.get(), browseFileButton.get()},
                                        params
                                        ));
    mainGrid->setBackground(juce::Colour (245, 246, 252));
    mainGrid->setCornerSize(10.f);
    addAndMakeVisible(*mainGrid);
    params.clear();
    
    setSize (400, 300);
    
    getLookAndFeel().setUsingNativeAlertWindows(true);
    
    startTimer(50.);
}

AutoEffectsAudioProcessorEditor::~AutoEffectsAudioProcessorEditor()
{
    dropFileLabel = nullptr;
    dropFileLabel2 = nullptr;
    dropImage = nullptr;
    browseFileButton = nullptr;
    cancelButton = nullptr;
    
    dropZone = nullptr;
    loadingWaitingScreen = nullptr;
    chorusBlock = nullptr;
    
    stopTimer();
}

//==============================================================================
void AutoEffectsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);
}

void AutoEffectsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    
    loadingWaitingScreen->setBounds(bounds);
    
    auto reducedBound = bounds.reduced(30);
    mainGrid->setBounds(reducedBound);
    dropZone->setBounds(reducedBound);
    if (!showEffects) {
        mainGrid->setVisible(true);
        dropZone->setVisible(true);
        cancelButton->setVisible(false);
        if (chorusBlock)
            chorusBlock->setVisible(false);
    } else {
        chorusBlock->setBounds(reducedBound);
        cancelButton->setVisible(true);
        mainGrid->setVisible(false);
        dropZone->setVisible(false);
    }
}

void AutoEffectsAudioProcessorEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == cancelButton.get()) {
        audioProcessor.resetPlugin();
    }
}

void AutoEffectsAudioProcessorEditor::selectFileButtonDidSelectNewFiles(SelectFileButton* button, StringArray files, Array<URL> /*urls*/)
{
    if (button == browseFileButton.get()) {
        audioProcessor.setTargetToProcess(File(files[0]));
    }
}
