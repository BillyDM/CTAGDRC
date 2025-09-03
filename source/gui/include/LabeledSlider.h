/*
Copyright (C) 2020 Phillip Lamp
This file is part of the CTAGDRC project <https://github.com/p-hlp/CTAGDRC>.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "ModSlider.h"

using SliderAttachment = std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>;
using ButtonAttachment = std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>;

class LabeledSlider : public juce::Component
{
public:

    LabeledSlider() : slider(&sliderLabel)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible(sliderLabel);
        sliderLabel.setJustificationType(juce::Justification::centred);
    }

    LabeledSlider(Component* parent) : slider(&sliderLabel)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider.setPopupDisplayEnabled(true, true, parent, 0);
        slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);

        addAndMakeVisible(sliderLabel);
        sliderLabel.setJustificationType(juce::Justification::centred);
    }

    void reset(juce::AudioProcessorValueTreeState& state, const juce::String& paramID)
    {
        sAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(state, paramID, slider));
    }

    void setLabelText(const juce::String& text)
    {
        sliderLabel.setText(text, juce::dontSendNotification);
    }


    void resized() override
    {
        auto bounds = getLocalBounds().reduced(2);
        const auto fontHeight = sliderLabel.getFont().getHeight();

        const auto labelBounds = bounds.removeFromBottom(fontHeight);
        slider.setBounds(bounds);
        sliderLabel.setBounds(labelBounds);
    }

    void setHasModifier(bool mod) { slider.setHasModifiers(mod); }
    bool hasModifier() const { return slider.hasModifier(); }

private:
    ModSlider slider;
    juce::Label sliderLabel;
    SliderAttachment sAttachment;
    ButtonAttachment bAttachment;
};
