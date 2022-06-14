/*
  ==============================================================================

    EffectProcessors.h
    Created: 30 May 2022 8:03:58pm
    Author:  Hugo PRAT

  ==============================================================================
*/

#pragma once

#include "CustomJuceHeader.h"

enum EffectEnum {
    Dry = 0,
    FeedBackDelay,
    SlapbackDelay,
    Reverb,
    Chorus,
    Flanger,
    Phaser,
    Tremolo,
    Vibrato,
    Distortion,
    Overdrive
};

//==============================================================================
class ProcessorBase  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ProcessorBase()
        : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo())
                                           .withOutput ("Output", juce::AudioChannelSet::stereo()))
    {}

    //==============================================================================
    void prepareToPlay (double, int) override {}
    void releaseResources() override {}
    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override {}

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return nullptr; }
    bool hasEditor() const override                              { return false; }

    //==============================================================================
    const juce::String getName() const override                  { return {}; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 0; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override         {}

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorBase)
};

//==============================================================================
class FilterProcessor  : public ProcessorBase
{
public:
    FilterProcessor() {}

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate, 1000.0f);

        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        filter.prepare (spec);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        filter.process (context);
    }

    void reset() override
    {
        filter.reset();
    }

    const juce::String getName() const override { return "Filter"; }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};

//==============================================================================
class ChorusProcessor  : public ProcessorBase
{
public:
    ChorusProcessor() {
        filter.setRate(rate);
        filter.setDepth(depth);
        filter.setCentreDelay(centreDelay);
        filter.setFeedback(feedback);
        filter.setMix(mix);
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 };
        filter.prepare (spec);
    }

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> context (block);
        filter.process (context);
    }

    void reset() override
    {
        filter.reset();
    }

    void setRate(float value) {
        rate = value;
        filter.setRate(value);
    }
    void setDepth(float value) {
        depth = value;
        filter.setDepth(value);
    }
    void setCentreDelay(float value) {
        centreDelay = value;
        filter.setCentreDelay(value);
    }
    void setFeedback(float value) {
        feedback = value;
        filter.setFeedback(value);
    }
    void setMix(float value) {
        mix = value;
        filter.setMix(value);
    }
    
    float getRate() { return rate; }
    float getDepth() { return depth; }
    float getCentreDelay() { return centreDelay; }
    float getFeedback() { return feedback; }
    float getMix() { return mix; }
    
    const juce::String getName() const override { return "Chorus"; }

private:
    juce::dsp::Chorus<float> filter;
    
    float rate = 50.f;
    float depth = 0.5f;
    float centreDelay = 50.f;
    float feedback = 0.f;
    float mix = 0.75f;
};
