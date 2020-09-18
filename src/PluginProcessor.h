#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <opus.h>
#include "BlockSizeAdapter.h"
#define PACKET_SIZE 1024

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor, public BlockSizeAdapter::AdapterProcessor, public juce::AudioProcessorValueTreeState::Listener
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

    //==============================================================================
    void processChunk(std::span<float> chunk) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    //==============================================================================
    void attachSlider(const juce::String& parameterId, juce::Slider& slider);
    void unattachAllSliders();

private:
    //==============================================================================
    void updateFrameSize(size_t size);

    // Processors ==================================================================

    BlockSizeAdapter _blockSizeAdapter;
    std::vector<float> _interlacedBuffer;
    OpusEncoder* _encoder = nullptr;
    OpusDecoder* _decoder = nullptr;
    juce::dsp::DryWetMixer<float> _dryWetMixer;
    juce::IIRFilter _lowpassFilterLeft;
    juce::IIRFilter _lowpassFilterRight;

    // Parameters ==================================================================
    juce::AudioProcessorValueTreeState _parameters;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> _sliderAttachments;

    std::atomic<float>* _drywet = nullptr;
    std::atomic<bool> _useFilter = true;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
