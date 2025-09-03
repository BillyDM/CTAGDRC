/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "PluginEditor.h"

//==============================================================================
CtagdrcAudioProcessorEditor::CtagdrcAudioProcessorEditor(CtagdrcAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor(p), valueTreeState(vts), backgroundApp(juce::Colour(Constants::Colors::bg_App)),
      inGainLSlider(this),
      makeupGainLSlider(this), treshLSlider(this), ratioLSlider(this), kneeLSlider(this), attackLSlider(this),
      releaseLSlider(this), mixLSlider(this),
      powerButton("powerButton", juce::DrawableButton::ButtonStyle::ImageOnButtonBackground)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&scLaF);
    initWidgets();
    setSize(400, 300);
    startTimerHz(60);
}

CtagdrcAudioProcessorEditor::~CtagdrcAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void CtagdrcAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(backgroundApp);
}

void CtagdrcAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(Constants::Margins::big);

    const auto headerHeight = area.getHeight() / 10;
    const auto btnAreaWidth = area.getWidth() / 5;
    const auto btnBotHeight = area.getHeight() / 3;

    auto header = area.removeFromTop(headerHeight).reduced(Constants::Margins::small);
    auto lBtnArea = area.removeFromLeft(btnAreaWidth).reduced(Constants::Margins::small);
    auto rBtnArea = area.removeFromRight(btnAreaWidth).reduced(Constants::Margins::small);
    auto botBtnArea = area.removeFromBottom(btnBotHeight).reduced(Constants::Margins::medium);

    const juce::FlexItem::Margin knobMargin = juce::FlexItem::Margin(Constants::Margins::medium);
    const juce::FlexItem::Margin knobMarginSmall = juce::FlexItem::Margin(Constants::Margins::medium);
    const juce::FlexItem::Margin buttonMargin = juce::FlexItem::Margin(Constants::Margins::small, Constants::Margins::big,
                                                           Constants::Margins::small,
                                                           Constants::Margins::big);
    juce::FlexBox headerBox;
    headerBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    headerBox.flexDirection = juce::FlexBox::Direction::row;
    headerBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    headerBox.items.add(juce::FlexItem(lahButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(autoAttackButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(autoReleaseButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(autoMakeupButton).withFlex(2).withMargin(buttonMargin));
    headerBox.items.add(juce::FlexItem(powerButton).withFlex(1).withMargin(buttonMargin));
    headerBox.performLayout(header.toFloat());

    juce::FlexBox leftBtnBox;
    leftBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    leftBtnBox.flexDirection = juce::FlexBox::Direction::column;
    leftBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    leftBtnBox.items.add(juce::FlexItem(attackLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(juce::FlexItem(releaseLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.items.add(juce::FlexItem(inGainLSlider).withFlex(1).withMargin(knobMarginSmall));
    leftBtnBox.performLayout(lBtnArea.toFloat());

    juce::FlexBox rightBtnBox;
    rightBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    rightBtnBox.flexDirection = juce::FlexBox::Direction::column;
    rightBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    rightBtnBox.items.add(juce::FlexItem(kneeLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(juce::FlexItem(ratioLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.items.add(juce::FlexItem(mixLSlider).withFlex(1).withMargin(knobMarginSmall));
    rightBtnBox.performLayout(rBtnArea.toFloat());

    juce::FlexBox botBtnBox;
    botBtnBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    botBtnBox.flexDirection = juce::FlexBox::Direction::row;
    botBtnBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    botBtnBox.items.add(juce::FlexItem(treshLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.items.add(juce::FlexItem(makeupGainLSlider).withFlex(1).withMargin(knobMargin));
    botBtnBox.performLayout(botBtnArea.toFloat());

    juce::FlexBox meterBox;
    meterBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    meterBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    meterBox.items.add(juce::FlexItem(meter).withFlex(1).withMargin(Constants::Margins::big));
    meterBox.performLayout(area.toFloat());
}


void CtagdrcAudioProcessorEditor::buttonClicked(juce::Button* b)
{
    if (b == &autoAttackButton)attackLSlider.setEnabled(!attackLSlider.isEnabled());
    if (b == &autoReleaseButton)releaseLSlider.setEnabled(!releaseLSlider.isEnabled());
    if (b == &powerButton) setGUIState(powerButton.getToggleState());
}

void CtagdrcAudioProcessorEditor::timerCallback()
{
    int m = meter.getMode();
    switch (m)
    {
    case Meter::Mode::IN:
        meter.update(processor.currentInput.get());
        break;
    case Meter::Mode::OUT:
        meter.update(processor.currentOutput.get());
        break;
    case Meter::Mode::GR:
        meter.update(processor.gainReduction.get());
        break;
    default:
        break;
    }
}

void CtagdrcAudioProcessorEditor::initWidgets()
{
    addAndMakeVisible(inGainLSlider);
    inGainLSlider.reset(valueTreeState, "inputgain");
    inGainLSlider.setLabelText("Input");

    addAndMakeVisible(makeupGainLSlider);
    makeupGainLSlider.reset(valueTreeState, "makeup");
    makeupGainLSlider.setLabelText("Makeup");

    addAndMakeVisible(treshLSlider);
    treshLSlider.reset(valueTreeState, "threshold");
    treshLSlider.setLabelText("Threshold");

    addAndMakeVisible(ratioLSlider);
    ratioLSlider.reset(valueTreeState, "ratio");
    ratioLSlider.setLabelText("Ratio");

    addAndMakeVisible(kneeLSlider);
    kneeLSlider.reset(valueTreeState, "knee");
    kneeLSlider.setLabelText("Knee");

    addAndMakeVisible(attackLSlider);
    attackLSlider.reset(valueTreeState, "attack");
    attackLSlider.setLabelText("Attack");

    addAndMakeVisible(releaseLSlider);
    releaseLSlider.reset(valueTreeState, "release");
    releaseLSlider.setLabelText("Release");

    addAndMakeVisible(mixLSlider);
    mixLSlider.reset(valueTreeState, "mix");
    mixLSlider.setLabelText("Mix");

    addAndMakeVisible(lahButton);
    lahButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    lahButton.setButtonText("LookAhead");
    lahButton.setClickingTogglesState(true);
    lahButton.setToggleState(false, juce::dontSendNotification);
    lahAttachment.reset(new juce:: AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "lookahead", lahButton));

    addAndMakeVisible(autoAttackButton);
    autoAttackButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoAttackButton.setButtonText("AutoAttack");
    autoAttackButton.setClickingTogglesState(true);
    autoAttackButton.setToggleState(false, juce::dontSendNotification);
    autoAttackButton.addListener(this);
    autoAttackAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "autoattack", autoAttackButton));

    addAndMakeVisible(autoReleaseButton);
    autoReleaseButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoReleaseButton.setButtonText("AutoRelease");
    autoReleaseButton.setClickingTogglesState(true);
    autoReleaseButton.setToggleState(false, juce::dontSendNotification);
    autoReleaseButton.addListener(this);
    autoReleaseAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "autorelease", autoReleaseButton));

    addAndMakeVisible(autoMakeupButton);
    autoMakeupButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    autoMakeupButton.setButtonText("Makeup");
    autoMakeupButton.setClickingTogglesState(true);
    autoMakeupButton.setToggleState(false, juce::dontSendNotification);
    autoMakeupButton.addListener(this);
    autoMakeupAttachment.reset(
        new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "automakeup", autoMakeupButton));

    addAndMakeVisible(powerButton);
    powerButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colour::fromRGB(245, 124, 0));
    powerButton.setImages(
        juce::Drawable::createFromImageData(BinaryData::power_white_svg, BinaryData::power_white_svgSize).get());
    powerButton.setClickingTogglesState(true);
    powerButton.setToggleState(true, juce::dontSendNotification);
    powerButton.addListener(this);
    powerAttachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "power", powerButton));

    addAndMakeVisible(meter);
    meter.setMode(Meter::Mode::GR);
}

void CtagdrcAudioProcessorEditor::setGUIState(bool powerState)
{
    inGainLSlider.setEnabled(powerState);
    treshLSlider.setEnabled(powerState);
    ratioLSlider.setEnabled(powerState);
    kneeLSlider.setEnabled(powerState);
    makeupGainLSlider.setEnabled(powerState);
    mixLSlider.setEnabled(powerState);
    meter.setEnabled(powerState);
    meter.setGUIEnabled(powerState);
    lahButton.setEnabled(powerState);
    autoMakeupButton.setEnabled(powerState);

    autoAttackButton.setEnabled(powerState);
    autoReleaseButton.setEnabled(powerState);

    if (!powerState)
    {
        attackLSlider.setEnabled(powerState);
        releaseLSlider.setEnabled(powerState);
    }
    else
    {
        attackLSlider.setEnabled(!autoAttackButton.getToggleState());
        releaseLSlider.setEnabled(!autoReleaseButton.getToggleState());
    }
}
