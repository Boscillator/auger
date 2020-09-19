//
// Created by oscil on 09/07/2020.
//

#include "Win95LookAndFeel.h"

Win95LookAndFeel::Win95LookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, _colorLightBlue);
    setColour(juce::Slider::trackColourId, _colorBlack);
    setColour(juce::ComboBox::textColourId, _colorBlack);
    setColour(juce::PopupMenu::textColourId, _colorBlack);
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

int Win95LookAndFeel::getMenuWindowFlags() {
    return 0;
}

juce::Path Win95LookAndFeel::makeThumbPath(int x, int y) {
    juce::Path p;
    p.addRectangle(x - 4, y, 8, 18);
    p.addTriangle(x - 4, y + 18, x, y + 24, x + 4, y + 18);
    return p;
}

void Win95LookAndFeel::drawComboBox(juce::Graphics& graphics, int width, int height, bool isButtonDown, int buttonX,
                                    int buttonY, int buttonW, int buttonH, juce::ComboBox& box) {
    // Draw inner shadow
    graphics.setColour(_colorBlack);
    graphics.fillRect(0,0, width, height);

    // Draw background
    graphics.setColour(_colorWhite);
    graphics.fillRect(1,1, width - 1, height - 1);

    // Make buttons square
    buttonH -= 6;
    buttonY += 3;
    buttonX = width - buttonH - 3;
    buttonW = buttonH;

    // Draw button

    graphics.setColour(_colorWhite);
    graphics.fillRect(buttonX, buttonY, buttonW, buttonH);

    graphics.setColour(_colorBlack);
    graphics.fillRect(buttonX + 1, buttonY + 1, buttonW, buttonH);

    graphics.setColour(_colorLightGray);
    graphics.fillRect(buttonX + 1, buttonY + 1, buttonW-1, buttonH-1);

    // Draw the arrow on the button
    juce::Path p;
    p.addPolygon(juce::Point<float>(buttonX + buttonW/2,  buttonY + buttonH/2), 3, 4, juce::float_Pi);
    graphics.setColour(_colorBlack);
    graphics.fillPath(p);
}

void Win95LookAndFeel::drawPopupMenuBackground(juce::Graphics& graphics, int width, int height) {
    graphics.setColour(_colorBlack);
    graphics.fillRect(0,0,width,height);
    graphics.setColour(_colorLightGray);
    graphics.fillRect(0,0,width - 2, height - 2);
}

