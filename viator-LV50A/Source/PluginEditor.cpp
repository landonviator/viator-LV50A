#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ViatorLV50AAudioProcessorEditor::ViatorLV50AAudioProcessorEditor (ViatorLV50AAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    startTimerHz(20);
    
    // header
    addAndMakeVisible(_headerComp);
    
    // pass filter knobs
    setPassFilterKnobProps();
    
    // eq filter knobs
    setEQFilterKnobProps();
    
    // volume fader
    setVolumeFaderProps();
    
    // groups
    setGroupProps();
    
    // vu
    auto main = juce::ImageCache::getFromMemory(BinaryData::vu_meter_png, BinaryData::vu_meter_pngSize);
    auto grid = juce::ImageCache::getFromMemory(BinaryData::scale_vumeter_png, BinaryData::scale_vumeter_pngSize);
    auto bg = juce::ImageCache::getFromMemory(BinaryData::back_vumeter_decore5_png, BinaryData::back_vumeter_decore5_pngSize);
    _vuMeter.setVUImages(main, grid, bg);
    addAndMakeVisible(_vuMeter);
    
    
    // window
    viator_utils::PluginWindow::setPluginWindowSize(audioProcessor._width, audioProcessor._height, *this, 2.0, 1.0);
}

ViatorLV50AAudioProcessorEditor::~ViatorLV50AAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void ViatorLV50AAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    
    auto bgImage = juce::ImageCache::getFromMemory(BinaryData::back_png, BinaryData::back_pngSize);
    
    auto bgWidth = getWidth() * 0.9;
    auto bgHeight = getHeight() * 0.82;
    auto bgY = getHeight() * 0.13;
    g.drawImage(bgImage, getLocalBounds().toFloat().withSizeKeepingCentre(bgWidth, bgHeight).withY(bgY));
}

void ViatorLV50AAudioProcessorEditor::resized()
{
    // header
    const auto headerHeightMult = 0.08;
    const auto headerHeight = getHeight() * headerHeightMult;
    _headerComp.setBounds(0, 0, getWidth(), headerHeight);
    
    // vu meter
    auto vuX = getWidth() * 0.1;
    auto vuY = getHeight() * 0.16;
    auto vuWidth = getWidth() * 0.27;
    auto vuHeight = getHeight() * 0.37;
    _vuMeter.setBounds(vuX, vuY, vuWidth, vuHeight);
    
    // pass filter knobs
    auto pfnX = getWidth() * 0.115;
    auto pfnY = getHeight() * 0.55;
    auto pfnSize = getWidth() * 0.12;
    for (auto& knob : _passFilterKnobs)
    {
        knob->setBounds(pfnX, pfnY, pfnSize, pfnSize);
        knob->setSliderTextBoxWidth(pfnSize);
        pfnX += pfnSize;
    }
    
    // eq knobs
    const int numCols = 4;
    const int numRows = 3;
    auto eqX = getWidth() * 0.37;
    auto eqY = getHeight() * 0.23;
    auto eqSize = getWidth() * 0.1;
    
    for (int col = 0; col < numCols; ++col)
    {
        for (int row = 0; row < numRows; ++row)
        {
            int index = col * numRows + row;
            _eqFilterKnobs[index]->setBounds(eqX, eqY, eqSize, eqSize);
            _eqFilterKnobs[index]->setSliderTextBoxWidth(eqSize);
            eqY += eqSize;
        }
        
        eqY = getHeight() * 0.23;
        eqX += eqSize * 1.15;
    }
    
    auto offset = 0.3;
    _group.setBounds(_eqFilterKnobs[0]->getX(), _eqFilterKnobs[0]->getY() - eqSize * offset, eqSize * (numCols + 0.5), eqSize * numRows + eqSize * offset * 2.0);

    // volume fader
    auto vfX = getWidth() * 0.84;
    auto vfY = getHeight() * 0.2;
    auto vfWidth = getWidth() * 0.06;
    auto vfHeight = getHeight() * 0.7;
    _volumeFader.setBounds(vfX, vfY, vfWidth, vfHeight);
    _volumeFader.setSliderTextBoxWidth(vfHeight);
    
    // Save plugin size in value tree
    savePluginBounds();
}

void ViatorLV50AAudioProcessorEditor::setPassFilterKnobProps()
{
    auto params = audioProcessor._parameterMap.getPassFilterSliderParams();
    auto image = juce::ImageCache::getFromMemory(BinaryData::Knob_03_png, BinaryData::Knob_03_pngSize);
    
    for (int i = 0; i < params.size(); ++i)
    {
        _passFilterKnobs.add(std::make_unique<viator_gui::ImageFader>());
        _passFilterSliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, params[i].paramID, *_passFilterKnobs[i]));
        
        _passFilterKnobs[i]->setFaderImageAndNumFrames(image, 129);
        _passFilterKnobs[i]->setName(params[i].paramName);
        _passFilterKnobs[i]->addMouseListener(this, false);
        _passFilterKnobs[i]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        _passFilterKnobs[i]->setDialValueType(viator_gui::CustomDialLabel::ValueType::kInt);
        addAndMakeVisible(*_passFilterKnobs[i]);
    }
}

void ViatorLV50AAudioProcessorEditor::setEQFilterKnobProps()
{
    auto params = audioProcessor._parameterMap.getFilterSliderParams();
    auto image = juce::ImageCache::getFromMemory(BinaryData::Knob_small_png, BinaryData::Knob_small_pngSize);
    
    for (int i = 0; i < params.size(); ++i)
    {
        _eqFilterKnobs.add(std::make_unique<viator_gui::ImageFader>());
        _eqFilterSliderAttachments.add(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, params[i].paramID, *_eqFilterKnobs[i]));
        
        auto valueType = params[i].isInt ? viator_gui::CustomDialLabel::ValueType::kFloat : viator_gui::CustomDialLabel::ValueType::kInt;
        _eqFilterKnobs[i]->setFaderImageAndNumFrames(image, 128);
        _eqFilterKnobs[i]->setName(params[i].paramName);
        _eqFilterKnobs[i]->addMouseListener(this, false);
        _eqFilterKnobs[i]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        _eqFilterKnobs[i]->setDialValueType(valueType);
        addAndMakeVisible(*_eqFilterKnobs[i]);
    }
}

void ViatorLV50AAudioProcessorEditor::setVolumeFaderProps()
{
    auto image = juce::ImageCache::getFromMemory(BinaryData::Ver_slider_png, BinaryData::Ver_slider_pngSize);
    _volumeFader.setFaderImageAndNumFrames(image, 256);
    _volumeFader.setName("Out");
    _volumeFader.addMouseListener(this, false);
    _volumeFader.setSliderStyle(juce::Slider::LinearVertical);
    addAndMakeVisible(_volumeFader);
    
    _volumeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor._treeState, ViatorParameters::volumeID, _volumeFader);
}

void ViatorLV50AAudioProcessorEditor::setGroupProps()
{
    _group.setText("The EQ Sauce");
    _group.setColour(juce::GroupComponent::ColourIds::textColourId, juce::Colours::whitesmoke.withAlpha(0.5f));
    _group.setLookAndFeel(&_borderLAF);
    addAndMakeVisible(_group);
}


void ViatorLV50AAudioProcessorEditor::savePluginBounds()
{
    audioProcessor.variableTree.setProperty("width", getWidth(), nullptr);
    audioProcessor.variableTree.setProperty("height", getHeight(), nullptr);
    audioProcessor._width = getWidth();
    audioProcessor._height = getHeight();
}

void ViatorLV50AAudioProcessorEditor::timerCallback()
{
    _vuMeter.getVUMeter().setValue(audioProcessor.getCurrentPeakSignal());
}
