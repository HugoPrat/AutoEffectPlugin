/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "EffectProcessors.h"

#include <BinaryData.h>
#include <torch/script.h>

//==============================================================================
/**
*/

class AutoEffectsAudioProcessor  : public juce::AudioProcessor, public Thread
{
public:
    
    //==============================================================================
    using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = juce::AudioProcessorGraph::Node;
    //==============================================================================
    AutoEffectsAudioProcessor();
    ~AutoEffectsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    enum processState {
        Fail = 0,
        Process,
        Success
    };
    
    processState getProcessState() { return processState; }
    
    void setTargetToProcess(const File audioFile)
    {
        processState = processState::Process;
        UIupdate_processing = true;
        
        targetFile = audioFile;
        
        ///Set flag for 'run' fonction in Thread
        hasTargetToProcess = true;
    }
    
    void processAudioFile();
    
    Array<EffectEnum> &getEffectChain() { return effectsChain; }

    int getNumberOfEffect() { return effectsChain.size(); }
    
    AudioProcessor* getaudioProcessFromIndex(int i) {
        if (i < 0 || i > nodes.size())
            return nullptr;
        return nodes[i]->getProcessor();
    }
    
    void resetPlugin() {
        effectsChain.clear();
        
        for (auto node : nodes)
            processGraph->removeNode(node.get());
        nodes.clear();
        
        NeedToUpdateGraph = true;
        UIupdate_EffectBlocks = true;
    }
    
    void initialiseGraph() {
        processGraph->clear();

        audioInputNode  = processGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = processGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::audioOutputNode));
        midiInputNode   = processGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiInputNode));
        midiOutputNode  = processGraph->addNode (std::make_unique<AudioGraphIOProcessor> (AudioGraphIOProcessor::midiOutputNode));

        connectAudioNodes();
        connectMidiNodes();
    }
    
    void connectAudioNodes()
    {
        for (int channel = 0; channel < 2; ++channel)
            processGraph->addConnection ({ { audioInputNode->nodeID,  channel },
                                            { audioOutputNode->nodeID, channel } });
    }

    void connectMidiNodes()
    {
        processGraph->addConnection ({ { midiInputNode->nodeID,  juce::AudioProcessorGraph::midiChannelIndex },
                                        { midiOutputNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex } });
    }
    
    void updateGraph()
    {
        if (!NeedToUpdateGraph)
            return;
        
        NeedToUpdateGraph = false;
        
        ///Clear current graph to rebuilt it complely
        for (auto node : nodes)
            processGraph->removeNode(node.get());
        nodes.clear();
        
        ///adding in array from a dataset
        for (int i = 0 ; i < effectsChain.size(); i++) {
            nodes.add(processGraph->addNode (std::make_unique<ChorusProcessor>()));
        }
        
        ///Remove all connections
        for (auto connection : processGraph->getConnections())
            processGraph->removeConnection (connection);
        
        for (auto node : nodes)
            node->getProcessor()->setPlayConfigDetails (getMainBusNumInputChannels(),
                                                        getMainBusNumOutputChannels(),
                                                        getSampleRate(), getBlockSize());
        
        ///If no effect to chain, connect input to output
        if (nodes.isEmpty()) {
            connectAudioNodes();
        } else {
        
        ///Connecting nodes beetween them in order of the array
        for (int i = 0; i < nodes.size() - 1; ++i) {
            for (int channel = 0; channel < 2; ++channel)
                processGraph->addConnection ({ { nodes[i]->nodeID, channel },
                                                { nodes[i + 1]->nodeID, channel } });
         }

        ///Connecting input to first node and last node to output
         for (int channel = 0; channel < 2; ++channel) {
             processGraph->addConnection ({ { audioInputNode->nodeID, channel },
                                             { nodes.getFirst()->nodeID, channel } });
             processGraph->addConnection ({ { nodes.getLast()->nodeID, channel },
                                             { audioOutputNode->nodeID, channel } });
         }
        
        for (auto node : processGraph->getNodes())
            node->getProcessor()->enableAllBuses();

        ///Mute some node ?
        /*for (int i = 0; i < 3; ++i) {
            auto  slot   = slots   .getUnchecked (i);
            auto& bypass = bypasses.getReference (i);

            if (slot != nullptr)
                slot->setBypassed (bypass->get());
        }*/

        }
        
        UIupdate_EffectBlocks = true;
        
    }
    
    bool NeedToUpdateGraph = true;
    bool processing = false;

    bool UIupdate_processing = false;
    bool UIupdate_EffectBlocks = false;
    
protected:
    
    void run() override
    {
        while (!threadShouldExit())
        {
            if (hasTargetToProcess) {
                hasTargetToProcess = false;
                
                processAudioFile();
                
            }
            wait (100);
        }
    }
    
private:
    
    torch::jit::script::Module classifier;
    
    std::unique_ptr<juce::AudioProcessorGraph> processGraph;
    
    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    juce::Array<Node::Ptr> nodes;
    
    File targetFile;
    
    bool hasTargetToProcess = false;
    
    //int numberOfEffect = 0;
    
    Array<EffectEnum> effectsChain;
    
    float modelSampleRate = 22050.f;
    
    processState processState = processState::Fail;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoEffectsAudioProcessor)
};
