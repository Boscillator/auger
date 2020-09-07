//
// Created by oscil on 09/07/2020.
//

#ifndef AUGER_PLUGIN_WIN95LOOKANDFEEL_H
#define AUGER_PLUGIN_WIN95LOOKANDFEEL_H
#include <juce_gui_basics/juce_gui_basics.h>

class Win95LookAndFeel : public juce::LookAndFeel_V4 {
public:
    Win95LookAndFeel();

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, juce::Slider::SliderStyle style, juce::Slider& slider) override;



private:
    juce::Colour _colorLightBlue = juce::Colour::fromRGB(0,128, 129);
    juce::Colour _colorLightGray = juce::Colour::fromRGB(0xDB, 0xDB, 0xDB);
    juce::Colour _colorBlack = juce::Colour::fromRGB(0,0,0);
    juce::Colour _colorDarkBlue = juce::Colour::fromRGB(0x00, 0x01, 0x81);
    juce::Colour _colorWhite = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);

    static juce::Path makeThumbPath(int x, int y) ;
};


#endif //AUGER_PLUGIN_WIN95LOOKANDFEEL_H
