/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AutoEffectsAudioProcessor::AutoEffectsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), processGraph(new juce::AudioProcessorGraph()),
#endif
                        Thread("AutoEffectThread")
{
    const char* data = BinaryData::classifier_pt;
    const int length = BinaryData::classifier_ptSize;

    std::istringstream is(std::string(data, length));
    classifier = torch::jit::load(is);
    
    startThread();
}

AutoEffectsAudioProcessor::~AutoEffectsAudioProcessor()
{
    stopThread(1000);
}

//==============================================================================
const juce::String AutoEffectsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoEffectsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutoEffectsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AutoEffectsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AutoEffectsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutoEffectsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutoEffectsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoEffectsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AutoEffectsAudioProcessor::getProgramName (int index)
{
    return {};
}

void AutoEffectsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AutoEffectsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    processGraph->setPlayConfigDetails (getMainBusNumInputChannels(),
                                         getMainBusNumOutputChannels(),
                                         sampleRate, samplesPerBlock);

    processGraph->prepareToPlay (sampleRate, samplesPerBlock);

    initialiseGraph();
}

void AutoEffectsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    processGraph->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoEffectsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AutoEffectsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    updateGraph();


    processGraph->processBlock(buffer, midiMessages);
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool AutoEffectsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AutoEffectsAudioProcessor::createEditor()
{
    return new AutoEffectsAudioProcessorEditor (*this);
}

//==============================================================================
void AutoEffectsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutoEffectsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void AutoEffectsAudioProcessor::processAudioFile()
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
        
    // Check that the file exists and that we can have a reader
    if (!targetFile.existsAsFile())
    {
        Logger::writeToLog("Could not find track file " + targetFile.getFileName());
        processState = processState::Fail;
        UIupdate_processing = true;
        return;
    }
        
    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(targetFile));
    if (!reader)
    {
        Logger::writeToLog("Could not load track from file " + targetFile.getFileName());
        processState = processState::Fail;
        UIupdate_processing = true;
        return;
    }
        
    // Basic properties of the audio buffer
    int numSamples    = (int)reader->lengthInSamples;
    double sampleRate = reader->sampleRate;
    unsigned int numChannels   = reader->numChannels;
    int targetLength  = 44100;
        
    AudioSampleBuffer inputBuffer(numChannels,numSamples);
    inputBuffer.clear();
        
    reader->read(&inputBuffer, 0, numSamples, 0, true, true);
        
    std::cout << "DONE" << std::endl;
        
    // 1b) Resample
        
    std::cout << "Resampling from " << sampleRate << " to " << modelSampleRate  << std::endl;
        
    double ratio =  sampleRate / modelSampleRate;
    int newNumSamples = (int)(((double)numSamples) / ratio);
        
    AudioSampleBuffer resampledInputBuffer;
        
    if (newNumSamples > targetLength)
        newNumSamples = targetLength;
        
    resampledInputBuffer.setSize(numChannels, targetLength);
    resampledInputBuffer.clear();
        
    ScopedPointer<LagrangeInterpolator> resampler = new LagrangeInterpolator();
        
    const float *inputPtr = inputBuffer.getReadPointer(0);
    float *outputPtr      = resampledInputBuffer.getWritePointer(0);
        
    resampler->reset();
    resampler->process(ratio, inputPtr, outputPtr, newNumSamples);
        
    std::cout << "DONE" << std::endl;;
            
        //----------------------
        // 2) Normalize input file
        
        //float mag = resampledInputBuffer.getMagnitude(0, newNumSamples);
        //resampledInputBuffer.applyGain(0, newNumSamples, 1./mag);
        
    const float *bufferPtr = resampledInputBuffer.getReadPointer(0);
        
    Array<float> bufferData = Array<float>(bufferPtr, targetLength);
        
    torch::Tensor input = torch::from_blob(bufferData.data(), {1,targetLength});
        
        
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input);

        
    auto output = classifier.forward(inputs).toTensor();
        
    ///Adding result in array of Effects enum and update graph
    
    int result = output.item<int>();
    DBG(result);
    
    
    effectsChain.add(static_cast<EffectEnum>(result));
    NeedToUpdateGraph = true;
    
    processState = processState::Success;
    UIupdate_processing = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoEffectsAudioProcessor();
}
