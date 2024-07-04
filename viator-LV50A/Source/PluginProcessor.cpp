#include "PluginProcessor.h"
#include "PluginEditor.h"

ViatorLV50AAudioProcessor::ViatorLV50AAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
, _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getButtonParams()[i].paramID, this);
    }
    
    // menus
    for (int i = 0; i < _parameterMap.getMenuParams().size(); i++)
    {
        _treeState.addParameterListener(_parameterMap.getMenuParams()[i].paramID, this);
    }
    
    _filters.clear();
    for (int i = 0; i < 4; ++i)
    {
        _filters.add(std::make_unique<viator_dsp::SVFilter<float>>());
    }
    
    _passFilters.clear();
    for (int i = 0; i < 2; ++i)
    {
        _passFilters.add(std::make_unique<viator_dsp::SVFilter<float>>());
    }
}

ViatorLV50AAudioProcessor::~ViatorLV50AAudioProcessor()
{
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getSliderParams()[i].paramID, this);
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getButtonParams()[i].paramID, this);
    }
    
    // menus
    for (int i = 0; i < _parameterMap.getMenuParams().size(); i++)
    {
        _treeState.removeParameterListener(_parameterMap.getMenuParams()[i].paramID, this);
    }
}

//==============================================================================
const juce::String ViatorLV50AAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ViatorLV50AAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ViatorLV50AAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ViatorLV50AAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ViatorLV50AAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ViatorLV50AAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ViatorLV50AAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ViatorLV50AAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ViatorLV50AAudioProcessor::getProgramName (int index)
{
    return {};
}

void ViatorLV50AAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ViatorLV50AAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // sliders
    for (int i = 0; i < _parameterMap.getSliderParams().size(); i++)
    {
        auto param = _parameterMap.getSliderParams()[i];
        auto range = juce::NormalisableRange<float>(param.min, param.max);
        
        if (param.isSkew == ViatorParameters::SliderParameterData::SkewType::kSkew)
        {
            range.setSkewForCentre(param.center);
        }

        if (param.isInt == ViatorParameters::SliderParameterData::NumericType::kInt)
        {
            params.push_back (std::make_unique<juce::AudioProcessorValueTreeState::Parameter>(juce::ParameterID { param.paramID, _versionNumber }, param.paramName, param.paramName, range, param.initial, valueToTextFunction, textToValueFunction));
        }
        
        else
        {
            params.push_back (std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { param.paramID, _versionNumber }, param.paramName, range, param.initial));
        }
    }
    
    // buttons
    for (int i = 0; i < _parameterMap.getButtonParams().size(); i++)
    {
        auto param = _parameterMap.getButtonParams()[i];
        params.push_back (std::make_unique<juce::AudioParameterBool>(juce::ParameterID { param.paramID, _versionNumber }, param.paramName, _parameterMap.getButtonParams()[i].initial));
    }
    
    // menus
    for (int i = 0; i < _parameterMap.getMenuParams().size(); i++)
    {
        auto param = _parameterMap.getMenuParams()[i];
        params.push_back (std::make_unique<juce::AudioParameterChoice>(juce::ParameterID { param.paramID, _versionNumber }, param.paramName, param.choices, param.defaultIndex));
    }
    
    return { params.begin(), params.end() };
}

void ViatorLV50AAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)

{
}

void ViatorLV50AAudioProcessor::updateParameters()
{
    // filters
    auto lowShelf = _treeState.getRawParameterValue(ViatorParameters::lowShelfToggleID)->load();
    auto highShelf = _treeState.getRawParameterValue(ViatorParameters::highShelfToggleID)->load();
    
    _filters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, lowShelf ? viator_dsp::SVFilter<float>::FilterType::kLowShelf : viator_dsp::SVFilter<float>::FilterType::kBandShelf);
    _filters[3]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, highShelf ? viator_dsp::SVFilter<float>::FilterType::kHighShelf : viator_dsp::SVFilter<float>::FilterType::kBandShelf);
    _filters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kGain, _treeState.getRawParameterValue(ViatorParameters::filter1GainID)->load());
    _filters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQ, _treeState.getRawParameterValue(ViatorParameters::filter1QID)->load());
    _filters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::filter1CutoffID)->load());
    
    _filters[1]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kGain, _treeState.getRawParameterValue(ViatorParameters::filter2GainID)->load());
    _filters[1]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQ, _treeState.getRawParameterValue(ViatorParameters::filter2QID)->load());
    _filters[1]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::filter2CutoffID)->load());
    
    _filters[2]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kGain, _treeState.getRawParameterValue(ViatorParameters::filter3GainID)->load());
    _filters[2]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQ, _treeState.getRawParameterValue(ViatorParameters::filter3QID)->load());
    _filters[2]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::filter3CutoffID)->load());
    
    _filters[3]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kGain, _treeState.getRawParameterValue(ViatorParameters::filter4GainID)->load());
    _filters[3]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQ, _treeState.getRawParameterValue(ViatorParameters::filter4QID)->load());
    _filters[3]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::filter4CutoffID)->load());
    
    _passFilters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::hpCutoffID)->load());
    _passFilters[1]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kCutoff, _treeState.getRawParameterValue(ViatorParameters::lpCutoffID)->load());
    
    _volumeModule.setGainDecibels(_treeState.getRawParameterValue(ViatorParameters::volumeID)->load());
}


//==============================================================================
void ViatorLV50AAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    _spec.sampleRate = sampleRate;
    _spec.maximumBlockSize = samplesPerBlock;
    _spec.numChannels = getTotalNumInputChannels();
    
    for (auto& filter : _filters)
    {
        filter->prepare(_spec);
        filter->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, viator_dsp::SVFilter<float>::FilterType::kBandShelf);
        filter->setParameter(viator_dsp::SVFilter<float>::ParameterId::kQType, viator_dsp::SVFilter<float>::QType::kParametric);
    }
    
    _passFilters[0]->prepare(_spec);
    _passFilters[0]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, viator_dsp::SVFilter<float>::FilterType::kHighPass);
    _passFilters[1]->prepare(_spec);
    _passFilters[1]->setParameter(viator_dsp::SVFilter<float>::ParameterId::kType, viator_dsp::SVFilter<float>::FilterType::kLowPass);
    
    _volumeModule.prepare(_spec);
    _volumeModule.setRampDurationSeconds(0.02);
    
    levelGain.reset(sampleRate, 0.5);
    
    updateParameters();
}

void ViatorLV50AAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ViatorLV50AAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
    || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void ViatorLV50AAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    updateParameters();
    
    juce::dsp::AudioBlock<float> block {buffer};
    
    for (auto& filter : _filters)
    {
        filter->process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    
    for (auto& filter : _passFilters)
    {
        filter->process(juce::dsp::ProcessContextReplacing<float>(block));
    }
    
    _volumeModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    // get meter value
    calculatePeakSignal(buffer);
}

void ViatorLV50AAudioProcessor::calculatePeakSignal(juce::AudioBuffer<float> &buffer)
{
    levelGain.skip(buffer.getNumSamples());
    peakDB = buffer.getMagnitude(0, 0, buffer.getNumSamples());
    
    if (peakDB < levelGain.getCurrentValue())
    {
        levelGain.setTargetValue(peakDB);
    }

    else
    {
        levelGain.setCurrentAndTargetValue(peakDB);
    }
}

float ViatorLV50AAudioProcessor::getCurrentPeakSignal()
{
    return juce::Decibels::gainToDecibels(levelGain.getNextValue());
}

//==============================================================================
bool ViatorLV50AAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ViatorLV50AAudioProcessor::createEditor()
{
    return new ViatorLV50AAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void ViatorLV50AAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    _treeState.state.appendChild(variableTree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    _treeState.state.writeToStream (stream);
}

void ViatorLV50AAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, size_t(sizeInBytes));
    variableTree = tree.getChildWithName("Variables");
    
    if (tree.isValid())
    {
        _treeState.state = tree;
        _width = variableTree.getProperty("width");
        _height = variableTree.getProperty("height");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ViatorLV50AAudioProcessor();
}
