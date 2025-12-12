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
#include "LevelDetector.h"
#include "GainComputer.h"
#include <juce_dsp/juce_dsp.h>
#include "DelayLine.h"
#include "LookAhead.h"
#include "HighpassFilter.h"

/* Compressor-Class:
 * The circruit is modeled after the "ideal" VCA-Compressor
 * based on the paper "Digital Dynamic Range Compressor Design �  Tutorial and Analysis"
 * by Giannoulis, Massberg & Reiss
 */

class Compressor
{
public:

    Compressor() = default;
    ~Compressor();

    // Prepares compressor with a ProcessSpec-Object containing samplerate, blocksize and number of channels
    void prepare(const juce::dsp::ProcessSpec& ps);

    // Sets compressor to bypassed/not bypassed
    void setPower(bool);

    // Sets input in dB
    void setInput(float);

    // Sets threshold in dB
    void setThreshold(float);

    // Sets ratio in dB
    void setRatio(float);

    // Sets knee-width in dB (> 0 = soft knee)
    void setKnee(float);

    // Sets make-up gain in dB
    void setMakeup(float);

    // Sets mix 0.0f - 1.0f
    void setMix(float);

    // Sets sidechain highpass 20.0 - 20000.0
    void setScHighpass(float);

    // Sets attack time in milliseconds
    void setAttack(float);

    // Sets release time in milliseconds
    void setRelease(float);

    // Sets auto attack to enabled = true or disabled = false
    void setAutoAttack(bool);

    // Sets auto release to enabled = true or disabled = false
    void setAutoRelease(bool);

    // Sets auto makeup to enabled = true or disabled = false
    void setAutoMakeup(bool);

    // Enables lookahead
    void setLookahead(bool);

    // Gets current make-up gain value
    float getMakeup();

    // Return current sampleRate
    double getSampleRate();

    float getMaxGainReduction();

    // Processes input buffer
    void process(juce::AudioBuffer<float>& buffer);

private:
    inline void applyInputGain(juce::AudioBuffer<float>&, int);
    inline float calculateAutoMakeup(const float*, int numSamples);

    //Directly initialize process spec to avoid debugging problems
    juce::dsp::ProcessSpec procSpec{-1, 0, 0};

    juce::AudioBuffer<float> originalSignal;
    juce::AudioBuffer<float> highpassedSignal;
    std::vector<float> sidechainSignal;
    float* rawSidechainSignal{nullptr};

    LevelDetector ballistics;
    GainComputer gainComputer;
    DelayLine delay;
    LookAhead lookahead;
    SmoothingFilter smoothedAutoMakeup;
    HighpassFilter scHighpassL;
    HighpassFilter scHighpassR;

    double lookaheadDelay{0.005};
    float input{0.0f};
    float prevInput{0.0f};
    float makeup{0.0f};
    float autoMakeup{0.0f};
    bool bypassed{false};
    bool lookaheadEnabled{false};
    bool autoMakeupEnabled{false};
    float mix{1.0f};
    float maxGainReduction{0.0f};
};
