/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CtagdrcAudioProcessor::CtagdrcAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    //Add parameter listener
    parameters.addParameterListener("power", this);
    parameters.addParameterListener("lookahead", this);
    parameters.addParameterListener("automakeup", this);
    parameters.addParameterListener("autoattack", this);
    parameters.addParameterListener("autorelease", this);
    parameters.addParameterListener("inputgain", this);
    parameters.addParameterListener("makeup", this);
    parameters.addParameterListener("threshold", this);
    parameters.addParameterListener("ratio", this);
    parameters.addParameterListener("knee", this);
    parameters.addParameterListener("attack", this);
    parameters.addParameterListener("release", this);
    parameters.addParameterListener("mix", this);

    gainReduction.set(0.0f);
    currentInput.set(-std::numeric_limits<float>::infinity());
    currentOutput.set(-std::numeric_limits<float>::infinity());
}

CtagdrcAudioProcessor::~CtagdrcAudioProcessor()
{
}

//==============================================================================
const juce::String CtagdrcAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CtagdrcAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CtagdrcAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool CtagdrcAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double CtagdrcAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CtagdrcAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int CtagdrcAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CtagdrcAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CtagdrcAudioProcessor::getProgramName(int index)
{
    return {};
}

void CtagdrcAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void CtagdrcAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    //Prepare dsp classes
    compressor.prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2});
    inLevelFollower.prepare(sampleRate);
    outLevelFollower.prepare(sampleRate);
    inLevelFollower.setPeakDecay(0.3f);
    outLevelFollower.setPeakDecay(0.3f);
    //Notify host about latency
    if (*parameters.getRawParameterValue("lookahead") > 0.5f)
        setLatencySamples(static_cast<int>(0.005 * sampleRate));
    else
        setLatencySamples(0);
}

void CtagdrcAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CtagdrcAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void CtagdrcAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    // Clear buffer
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //Update input peak metering
    inLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), totalNumInputChannels, numSamples);
    currentInput.set(juce::Decibels::gainToDecibels(inLevelFollower.getPeak()));

    // Do compressor processing
    compressor.process(buffer);

    // Update gain reduction metering
    gainReduction.set(compressor.getMaxGainReduction());

    // Update output peak metering
    outLevelFollower.updatePeak(buffer.getArrayOfReadPointers(), totalNumInputChannels, numSamples);
    currentOutput = juce::Decibels::gainToDecibels(outLevelFollower.getPeak());
}

//==============================================================================
bool CtagdrcAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CtagdrcAudioProcessor::createEditor()
{
    return new CtagdrcAudioProcessorEditor(*this, parameters);
}

//==============================================================================
void CtagdrcAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CtagdrcAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CtagdrcAudioProcessor();
}

void CtagdrcAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "inputgain") compressor.setInput(newValue);
    else if (parameterID == "power") compressor.setPower(!static_cast<bool>(newValue));
    else if (parameterID == "threshold") compressor.setThreshold(newValue);
    else if (parameterID == "automakeup") compressor.setAutoMakeup(static_cast<bool>(newValue));
    else if (parameterID == "autoattack")
    {
        const bool newBool = static_cast<bool>(newValue);
        compressor.setAutoAttack(newBool);
        if (!newBool)compressor.setAttack(*parameters.getRawParameterValue("attack"));
    }
    else if (parameterID == "autorelease")
    {
        const bool newBool = static_cast<bool>(newValue);
        compressor.setAutoRelease(newBool);
        if (!newBool)compressor.setRelease(*parameters.getRawParameterValue("release"));
    }
    else if (parameterID == "ratio") compressor.setRatio(newValue);
    else if (parameterID == "knee") compressor.setKnee(newValue);
    else if (parameterID == "attack") compressor.setAttack(newValue);
    else if (parameterID == "release") compressor.setRelease(newValue);
    else if (parameterID == "makeup") compressor.setMakeup(newValue);
    else if (parameterID == "lookahead")
    {
        const bool newBool = static_cast<bool>(newValue);
        if (newBool) setLatencySamples(static_cast<int>(0.005 * compressor.getSampleRate()));
        else setLatencySamples(0);
        compressor.setLookahead(newBool);
    }
    else if (parameterID == "mix") compressor.setMix(newValue);
}

juce::AudioProcessorValueTreeState::ParameterLayout CtagdrcAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>("lookahead", "Lookahead", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("automakeup", "AutoMakeup", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("autoattack", "AutoAttack", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("autorelease", "AutoRelease", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("power", "Power", true));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("inputgain", "Input",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::inputStart,
                                                               Constants::Parameter::inputEnd,
                                                               Constants::Parameter::inputInterval), 0.0f,
                                                           juce::String(),
                                                           juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               return juce::String(value, 1) + " dB";
                                                           }));


    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Tresh",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::thresholdStart,
                                                               Constants::Parameter::thresholdEnd,
                                                               Constants::Parameter::thresholdInterval), -10.0f,
                                                           juce::String(), juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float maxStrLen)
                                                           {
                                                               return juce::String(value, 1) + " dB";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::ratioStart,
                                                               Constants::Parameter::ratioEnd,
                                                               Constants::Parameter::ratioInterval, 0.5f), 2.0f,
                                                           juce::String(), juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               if (value > 23.9f)return juce::String("Infinity") + ":1";
                                                               return juce::String(value, 1) + ":1";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::kneeStart,
                                                               Constants::Parameter::kneeEnd,
                                                               Constants::Parameter::kneeInterval),
                                                           6.0f, juce::String(), juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               return juce::String(value, 1) + " dB";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::attackStart,
                                                               Constants::Parameter::attackEnd,
                                                               Constants::Parameter::attackInterval, 0.5f), 2.0f,
                                                           "ms",
                                                           juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               if (value == 100.0f) return juce::String(value, 0) + " ms";
                                                               return juce::String(value, 2) + " ms";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::releaseStart,
                                                               Constants::Parameter::releaseEnd,
                                                               Constants::Parameter::releaseInterval, 0.35f),
                                                           140.0f,
                                                           juce::String(),
                                                           juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               if (value <= 100) return juce::String(value, 2) + " ms";
                                                               if (value >= 1000)
                                                                   return juce::String(value * 0.001f, 2) + " s";
                                                               return juce::String(value, 1) + " ms";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("makeup", "Makeup",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::makeupStart,
                                                               Constants::Parameter::makeupEnd,
                                                               Constants::Parameter::makeupInterval), 0.0f,
                                                           juce::String(),
                                                           juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               return juce::String(value, 1) + " dB ";
                                                           }));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix",
                                                           juce::NormalisableRange<float>(
                                                               Constants::Parameter::mixStart,
                                                               Constants::Parameter::mixEnd,
                                                               Constants::Parameter::mixInterval),
                                                           1.0f, "%", juce::AudioProcessorParameter::genericParameter,
                                                           [](float value, float)
                                                           {
                                                               return juce::String(value * 100.0f, 1) + " %";
                                                           }));

    return {params.begin(), params.end()};
}
