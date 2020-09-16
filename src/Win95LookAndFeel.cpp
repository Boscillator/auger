//
// Created by oscil on 09/07/2020.
//

#include "Win95LookAndFeel.h"

Win95LookAndFeel::Win95LookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, _colorLightBlue);
    setColour(juce::Slider::trackColourId, _colorBlack);
}

void Win95LookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                        float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style,
                                        juce::Slider& slider) {
    // Draw track
    g.setColour(_colorBlack);
    g.fillRect(x, y + height / 2 - 12, width, 4);
    g.setColour(_colorWhite);
    g.fillRect(x + 2, y + height / 2 - 12 + 2, width - 2, 2);

    // Calculate thumb position
    int thumbX = (x + sliderPos) - minSliderPos;

    // Draw thumb
    g.setColour(_colorWhite);
    auto p = makeThumbPath(thumbX - 2, y);
    g.fillPath(p);

    g.setColour(_colorBlack);
    p = makeThumbPath(thumbX + 2, y + 3);
    g.fillPath(p);

    g.setColour(_colorLightGray);
    p = makeThumbPath(thumbX, y + 2);
    g.fillPath(p);
}

juce::Path Win95LookAndFeel::makeThumbPath(int x, int y) {
    juce::Path p;
    p.addRectangle(x - 4, y, 8, 18);
    p.addTriangle(x - 4, y + 18, x, y + 24, x + 4, y + 18);
    return p;
}

