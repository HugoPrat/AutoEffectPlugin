/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "EffectProcessors.h"

//==============================================================================
/**
*/
class AutoEffectsAudioProcessor  : public juce::AudioProcessor
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

    int getNumberOfEffect() { return numberOfEffect; }
    void setNumberOfEffect(int value) {
        if (value < 0)
            value = 0;
        numberOfEffect = value;
        NeedToUpdateGraph = true;
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
        
        ///Need to adding in array from a dataset
        ///TMP
        for (int i = 0 ; i != numberOfEffect; i++) {
            nodes.add(processGraph->addNode (std::make_unique<FilterProcessor>()));
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
        
    }
    
    bool NeedToUpdateGraph = true;
    
private:
    
    std::unique_ptr<juce::AudioProcessorGraph> processGraph;
    
    Node::Ptr audioInputNode;
    Node::Ptr audioOutputNode;
    Node::Ptr midiInputNode;
    Node::Ptr midiOutputNode;

    juce::Array<Node::Ptr> nodes;
    
    int numberOfEffect = 0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoEffectsAudioProcessor)
};
