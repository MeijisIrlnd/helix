/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& p, APVTS& tree)
    : AudioProcessorEditor (&p), audioProcessor (p), m_tree(tree)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    instantiateSlider(m_springLengthSlider, "SpringLength");
    instantiateSlider(m_springBouncinessSlider, "SpringBounciness");
    instantiateSlider(m_highDampingSlider, "HighDampingGain");
    resized();
}

PluginEditor::~PluginEditor()
{
    
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Wagwan", getLocalBounds(), juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    auto h = getHeight() / 12;
    for(auto i = 0; i < m_sliders.size(); i++) { 
      m_sliders[i]->label.setBounds(0, h * i, getWidth() / 8, h);
      m_sliders[i]->slider.setBounds(getWidth() / 8, h * i, getWidth() - getWidth() / 8, h);
    }
}

void PluginEditor::instantiateSlider(SliderComponent& slider, const juce::String& paramID) { 
    auto* param = m_tree.getParameter(paramID);
    slider.slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(&slider.slider);
    slider.label.setText(param->getName(80), juce::dontSendNotification);
    addAndMakeVisible(&slider.label);
    slider.attachment.reset(new juce::SliderParameterAttachment(*param, slider.slider, nullptr));
    m_sliders.push_back(&slider);
}