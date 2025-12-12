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
#include "include/MeterBackground.h"
#include "../util/include/Constants.h"
#include "juce_core/juce_core.h"
#include "juce_graphics/juce_graphics.h"

MeterBackground::MeterBackground()
{
    sAngle = 0.0f;
    eAngle = 0.0f;
    minValue = 0;
    maxValue = 0;
    step = 5;
    mode = 3;
    indicatorColour = juce::Colour(Constants::Colors::bg_LightGrey);
    backgroundApp = juce::Colour(Constants::Colors::bg_App);
    setBufferedToImage(true);
}

void MeterBackground::prepare(const float& s, const float& e)
{
    sAngle = s;
    eAngle = e;
}

void MeterBackground::paint(juce::Graphics& g)
{
    const auto bounds = meterArea.toFloat();
    const float centreX = bounds.getX() + bounds.getWidth() * 0.5f;
    const float centreY = bounds.getY() + bounds.getHeight();
    const float needleLength = juce::jmin(bounds.getWidth() * 0.7f, bounds.getHeight() * 0.7f);

    g.setColour(backgroundApp);
    g.fillRoundedRectangle(meterArea.toFloat(), 1);

    g.setColour(indicatorColour);
    drawIndicators(g, centreX, centreY, needleLength);
}

void MeterBackground::resized()
{
    meterArea = getLocalBounds().reduced(3);
    repaint();
}

void MeterBackground::drawIndicators(juce::Graphics& g, float centreX, float centreY, float length)
{
    const auto indices = (abs(maxValue - minValue) / step) + 1;
    int val = minValue;

    for (int i = 0; i < indices; ++i)
    {
        float mapped = juce::jmap(static_cast<float>(val), static_cast<float>(minValue), static_cast<float>(maxValue), sAngle,
                            eAngle);
        mapped -= mapped > 2 * juce::MathConstants<float>::pi ? juce::MathConstants<float>::twoPi : 0.0f;
        const float mappedSin = sin(mapped);
        const float mappedCos = cos(mapped);

        const float x2 = centreX + mappedSin * length;
        const float y2 = centreY - mappedCos * length;
        const float rX = centreX - x2;
        const float rY = centreY - y2;
        const float rLength = sqrt(juce::square(rX) + juce::square(rY));
        const float nX = rX / rLength;
        const float nY = rY / rLength;
        const float xCof = nX * 7;
        const float yCof = nY * 7;

        g.drawArrow({x2, y2, x2 - xCof, y2 - yCof}, 2.0f, 0, 0);

        const float textX = centreX + mappedSin * (length + 25.0);
        const float textY = centreY - mappedCos * (length + 25.0);

        g.drawText(juce::String((indices - i - 1) * -step), textX - 15.0, textY, 30.0, 20.0, juce::Justification::centredTop, false);

        val += step;
    }
}

void MeterBackground::setMode(int m)
{
    minValue = m == 3 ? -30 : -50;
    mode = m;
    repaint();
}

void MeterBackground::setIndicatorEnabled(bool state)
{
    indicatorColour = state
        ? juce::Colour(Constants::Colors::bg_LightGrey)
        : juce::Colour(Constants::Colors::bg_LightGrey).withAlpha(0.35f);
    repaint();
}
