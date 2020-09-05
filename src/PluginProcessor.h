#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <opus.h>
#include "BlockSizeAdapter.h"
#define PACKET_SIZE 512

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor, public BlockSizeAdapter::AdapterProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

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

    //===============================================================================
    void processChunk(std::span<float> chunk) override;

private:
    // Processors

    BlockSizeAdapter _blockSizeAdapter;
    std::vector<float> _interlacedBuffer;
    OpusEncoder* _encoder;
    OpusDecoder* _decoder;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
