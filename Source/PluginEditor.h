/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "dropFileZone.h"
#include "UI/LoadingWaitingScreen.h"
#include "UI/EffectBlocks.h"
#include "UIElements/SelectFileButton.h"

//==============================================================================
/**
*/
class AutoEffectsAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::Timer,
                                         public juce::Button::Listener,
                                         public SelectFileButton::Listener,
                                         public dropFileZone::Listener
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
        if (audioProcessor.UIupdate_processing) {
            audioProcessor.UIupdate_processing = false;
            
            int state = audioProcessor.getProcessState();

            if (state == 1) { ///Loading
                loadingWaitingScreen->setVisible(true);
                loadingWaitingScreen->startAnimation();
            } else if (state == 2) { ///Success
                loadingWaitingScreen->setVisible(false);
                loadingWaitingScreen->stopAnimation();
            } else { ///Fail
                AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon, TRANS("Error while loading"), TRANS("Couldn't read from the specified file!"));

            }
        }
        
        if (audioProcessor.UIupdate_EffectBlocks) {
            audioProcessor.UIupdate_EffectBlocks = false;
            
            if (audioProcessor.getNumberOfEffect() == 0) {
                showEffects = false;
                chorusBlock = nullptr;
            } else {
                AudioProcessor* first = audioProcessor.getaudioProcessFromIndex(0);
                if (first != nullptr) {
                    chorusBlock.reset(new ChorusUiBlock(first));
                    addAndMakeVisible(*chorusBlock);
                    showEffects = true;
                }
            }
            resized();
        }
    }
    
    //===================================================
    // MARK: - dropFileZone Listener
    //===================================================
    
    void fileBeingDropInZone(const StringArray& files) override
    {
        if (files.size() != 1) {
            //AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Only one audio file should be drop", "OK", this, ModalCallbackFunction::create ([] () {}));
            return;
        }
        audioProcessor.processAudioFile(File(files[0]));
    }
    
    void clickDownOnZone(const MouseEvent &event) override
    {
        browseFileButton->mouseDown(event);
    }
    
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void selectFileButtonDidSelectNewFiles(SelectFileButton* button, StringArray files, Array<URL> urls) override;

    
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoEffectsAudioProcessor& audioProcessor;
    
    std::unique_ptr<GenericGrid> mainGrid;
    
    std::unique_ptr<juce::Label> dropFileLabel;
    std::unique_ptr<juce::Label> dropFileLabel2;
    std::unique_ptr<ImageButton> dropImage;
    std::unique_ptr<SelectFileButton> browseFileButton;

    std::unique_ptr<ImageButton> cancelButton;
    
    std::unique_ptr<dropFileZone> dropZone;
    std::unique_ptr<LoadingWaitingScreen> loadingWaitingScreen;

    std::unique_ptr<ChorusUiBlock> chorusBlock;
    
    bool showEffects = false;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoEffectsAudioProcessorEditor)
};
