/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class PluginEditor  : public juce::AudioProcessorEditor
{
private: 
  struct SliderComponent;
public:
    PluginEditor (PluginProcessor&, APVTS&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void instantiateSlider(SliderComponent& slider, const juce::String& param);
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& audioProcessor;
    APVTS& m_tree;
    struct SliderComponent { 
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::SliderParameterAttachment> attachment;
    };
    SliderComponent m_springLengthSlider, m_springBouncinessSlider, m_highDampingSlider;
    std::vector<SliderComponent*> m_sliders;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
