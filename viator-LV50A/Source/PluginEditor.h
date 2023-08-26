#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "components/Header.h"
#include "globals/Parameters.h"

class ViatorLV50AAudioProcessorEditor  : public juce::AudioProcessorEditor
, private juce::Timer
{
public:
    ViatorLV50AAudioProcessorEditor (ViatorLV50AAudioProcessor&);
    ~ViatorLV50AAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ViatorLV50AAudioProcessor& audioProcessor;
    
    Header _headerComp;
    
    // vu meter
    viator_gui::VUMeter _vuMeter;
    
    // pass filters
    juce::OwnedArray<viator_gui::ImageFader> _passFilterKnobs;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> _passFilterSliderAttachments;
    void setPassFilterKnobProps();
    
    // eq filters
    juce::OwnedArray<viator_gui::ImageFader> _eqFilterKnobs;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> _eqFilterSliderAttachments;
    void setEQFilterKnobProps();
    
    // volume
    viator_gui::ImageFader _volumeFader;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _volumeAttach;
    void setVolumeFaderProps();
    
    // timer
    void timerCallback() override;
    
    juce::GroupComponent _group;
    viator_gui::CustomBorder _borderLAF;
    void setGroupProps();
    
    // Save plugin size in value tree
    void savePluginBounds();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViatorLV50AAudioProcessorEditor)
};
