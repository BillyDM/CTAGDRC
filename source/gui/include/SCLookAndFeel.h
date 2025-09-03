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

#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "../../util/include/Constants.h"

class SCLookAndFeel : public juce::LookAndFeel_V4
{
private:
    //Vars
    juce::Typeface::Ptr robotoLight, robotoRegular, robotoMedium, robotoBold;

    //Color objects
    juce::Colour backgroundDarkGrey, backgroundDarkGreyDisabled;
    juce::Colour backgroundMidGrey;
    juce::Colour statusOutline, statusOutlineDisabled;
    juce::Colour gradientDarkGrey, gradientDarkGreyDisabled;

public:

    SCLookAndFeel()
    {
        robotoLight = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoLight_ttf, BinaryData::RobotoLight_ttfSize);
        robotoRegular = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoRegular_ttf,
                                                          BinaryData::RobotoRegular_ttfSize);
        robotoMedium = juce::Typeface::
            createSystemTypefaceFor(BinaryData::RobotoMedium_ttf, BinaryData::RobotoMedium_ttfSize);
        robotoBold = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoBold_ttf, BinaryData::RobotoBold_ttfSize);

        backgroundDarkGrey = juce::Colour(Constants::Colors::bg_DarkGrey);
        backgroundDarkGreyDisabled = juce::Colour(Constants::Colors::bg_DarkGrey).withAlpha(0.35f);
        backgroundMidGrey = juce::Colour(Constants::Colors::bg_MidGrey);
        statusOutline = juce::Colour(Constants::Colors::statusOutline);
        statusOutlineDisabled = juce::Colour(statusOutline).withAlpha(0.35f);
        gradientDarkGrey = juce::Colour(Constants::Colors::gd_DarktGreyStart);
        gradientDarkGreyDisabled = juce::Colour(Constants::Colors::gd_DarktGreyStart).withAlpha(0.35f);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        const float radius = static_cast<float>(juce::jmin(width / 2, height / 2));
        const float centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
        const float centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
        const float rx = centreX - radius;
        const float ry = centreY - radius;
        const float rw = radius * 2.0f;

        const float min = static_cast<float>(slider.getMinimum());
        const float max = static_cast<float>(slider.getMaximum());
        const float zeroPos = -min / (max - min);
        const float zeroAngle = rotaryStartAngle + zeroPos * (rotaryEndAngle - rotaryStartAngle);
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        const float bedThickness = 2.0f;
        const float bedOutline = 1.4f;
        const float statusOutlineThickness = 2.5f;
        const float extraMargin = 2.0f;

        const float pointerThickness = 3.0f;
        const float pointerLength = (radius - extraMargin - statusOutlineThickness - bedOutline - bedThickness - 1.0f) *
            0.8f;

        juce::Path p, q, a, h;
        juce::Rectangle<float> rect = juce::Rectangle<float>(rx, ry, rw, rw);

        //status bg
        slider.isEnabled() ? g.setColour(backgroundDarkGrey) : g.setColour(backgroundDarkGreyDisabled);
        a.addCentredArc(centreX, centreY, radius - extraMargin, radius - extraMargin, 0.0f, rotaryStartAngle,
                        rotaryEndAngle, true);
        g.strokePath(a, juce::PathStrokeType(3.5f));

        //status ring
        slider.isEnabled() ? g.setColour(statusOutline) : g.setColour(statusOutlineDisabled);
        h.addCentredArc(centreX, centreY, radius - extraMargin, radius - extraMargin, 0.0f, zeroAngle, angle, true);
        g.strokePath(h, juce::PathStrokeType(3.5f));

        //main knob
        slider.isEnabled()
            ? g.setGradientFill(juce::ColourGradient(gradientDarkGrey, 0, 0, gradientDarkGrey, 0, rw, false))
            : g.setColour(gradientDarkGreyDisabled);
        g.fillEllipse(rect.reduced(extraMargin + statusOutlineThickness + bedOutline + bedThickness));

        //knob pointer
        p.addRectangle(-pointerThickness * 0.5f, -radius + 6.0f, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(backgroundDarkGrey);
        g.fillPath(p);
    }


    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const auto bounds = button.getLocalBounds().toFloat();

        g.setColour(button.getToggleState()
                        ? statusOutline.withMultipliedAlpha(0.8f)
                        : backgroundMidGrey);
        g.fillRoundedRectangle(bounds, 2);
    }

    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override
    {
        juce::Font font(getTextButtonFont(button, button.getHeight()));
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState()
                                          ? juce::TextButton::textColourOnId
                                          : juce::TextButton::textColourOffId)
                          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
        const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
        const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                             leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                             juce::Justification::centred, 2);
    }

    void drawBubble(juce::Graphics& g, juce::BubbleComponent& b, const juce::Point<float>& tip, const juce::Rectangle<float>& body) override
    {
        g.setColour(backgroundMidGrey);
        g.fillRoundedRectangle(body, 2);
    }


    juce::Font getSliderPopupFont(juce::Slider&) override
    {
        return robotoBold;
    }

    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override
    {
        return robotoRegular;
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(robotoMedium);
    }

    juce::Font getTextButtonFont(juce::TextButton& button, int height) override
    {
        juce::Font font(robotoMedium);
        font.setHeight(12);
        return font;
    }

    juce::Font getAlertWindowMessageFont() override
    {
        juce::Font font(robotoRegular);
        font.setHeight(14.0f);
        return font;
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW,
                      int buttonH, juce::ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 1.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);

        g.setColour(backgroundDarkGrey);
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

        juce::Rectangle<int> arrowZone(width - 30, 0, 20, height);
        juce::Path path;
        path.startNewSubPath(arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
        path.lineTo(static_cast<float>(arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
        path.lineTo(arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);

        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, juce::PathStrokeType(2.0f));
    }


    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds(1, 1,
                        box.getWidth(),
                        box.getHeight() - 2);

        label.setFont(getComboBoxFont(box));
    }


    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(backgroundMidGrey);
        juce::ignoreUnused(width, height);

#if ! JUCE_MAC
        g.setColour(backgroundDarkGrey.withAlpha(0.6f));
        g.drawRect(0, 0, width, height);
#endif
    }

    juce::Font getPopupMenuFont() override
    {
        return juce::Font(robotoBold);
    }


    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return juce::Font(robotoMedium);
    }
};
