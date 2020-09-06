#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    void configureLookAndFeel();
    void addComponents();
    void drawWin95Window(juce::Graphics& g) const;
    void drawSection(juce::Graphics& g, const juce::String& label, int x, int y, int width,
                     int height);

    //==============================================================================
    juce::Slider bitrateSlider;

    void sliderValueChanged(juce::Slider* slider) override;

    //==============================================================================
    juce::Colour _colorLightBlue = juce::Colour::fromRGB(0,128, 129);
    juce::Colour _colorLightGray = juce::Colour::fromRGB(0xDB, 0xDB, 0xDB);
    juce::Colour _colorBlack = juce::Colour::fromRGB(0,0,0);
    juce::Colour _colorDarkBlue = juce::Colour::fromRGB(0x00, 0x01, 0x81);
    juce::Colour _colorWhite = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);

    //==============================================================================
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)

};
