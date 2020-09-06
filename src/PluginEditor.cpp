#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    configureLookAndFeel();
    addComponents();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 420);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

    // Create fake win95 window
    drawWin95Window(g);

    // Create boarder for slider
    drawSection(g, bitrateSlider.getTextFromValue(bitrateSlider.getValue()), 128, 128, 343, 58);

}

void AudioPluginAudioProcessorEditor::drawWin95Window(juce::Graphics& g) const {
    g.setColour(_colorBlack);
    g.fillRect(40, 74, 520 + 2, 240 + 2);   // Drop shadow
    g.setColour(_colorLightGray);
    g.fillRect(40, 74, 520, 240);   // Main grey background
    g.setColour(_colorDarkBlue);
    g.fillRect(40 + 2, 74 + 2, 520 - 4, 26);
    g.setColour(_colorWhite);
    g.drawText("Auger.exe", 40 + 2 + 8, 72 + 2 + 2, 1000, 26, juce::Justification::left, true);
}

void AudioPluginAudioProcessorEditor::resized()
{
    bitrateSlider.setBounds(147, 141, 305, 20);
}

void AudioPluginAudioProcessorEditor::configureLookAndFeel() {
    getLookAndFeel().setColour(juce::ResizableWindow::backgroundColourId, _colorLightBlue);
}

void AudioPluginAudioProcessorEditor::addComponents() {
    bitrateSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bitrateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    bitrateSlider.setTextValueSuffix(" BPS");
    bitrateSlider.addListener(this);
    processorRef.attachSlider("bitrate", bitrateSlider);
    addAndMakeVisible(bitrateSlider);
}

void AudioPluginAudioProcessorEditor::drawSection(juce::Graphics& g, const juce::String& label, int x, int y, int width,
                                                  int height) {

    // Draw boarders
    g.setColour(_colorBlack);
    g.fillRect(x - 1,y - 1,width+2,height+2);
    g.setColour(_colorWhite);
    g.fillRect(x,y,width,height);
    g.setColour(_colorLightGray);
    g.fillRect(x+1,y+1,width-1,height-1);

    // Calculate label width
    auto labelWidth = g.getCurrentFont().getStringWidth(label);

    // Draw label
    g.setColour(_colorLightGray);
    g.fillRect(x + 10, y - 1, labelWidth + 20, 4);
    g.setColour(_colorBlack);
    g.drawText(label, x + 20, y - 4, 90, 10, juce::Justification::left, true);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    repaint();
}


