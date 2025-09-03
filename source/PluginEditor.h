/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "gui/include/SCLookAndFeel.h"
#include "gui/include/LabeledSlider.h"
#include "gui/include/Meter.h"

//==============================================================================
/**
*/
class CtagdrcAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Button::Listener, juce::Timer
{
public:
    CtagdrcAudioProcessorEditor(CtagdrcAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~CtagdrcAudioProcessorEditor();

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button*) override;
    void timerCallback() override;

private:
    void initWidgets();
    void setGUIState(bool);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CtagdrcAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    //LookAndFeel
    SCLookAndFeel scLaF;
    juce::Colour backgroundApp;

    //Widgets
    MeterBackground meterbg;
    Meter meter;

    LabeledSlider inGainLSlider;
    LabeledSlider makeupGainLSlider;
    LabeledSlider treshLSlider;
    LabeledSlider ratioLSlider;
    LabeledSlider kneeLSlider;
    LabeledSlider attackLSlider;
    LabeledSlider releaseLSlider;
    LabeledSlider mixLSlider;

    juce::TextButton lahButton;
    juce::TextButton autoAttackButton;
    juce::TextButton autoReleaseButton;
    juce::TextButton autoMakeupButton;
    juce::DrawableButton powerButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lahAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoMakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CtagdrcAudioProcessorEditor)
};
