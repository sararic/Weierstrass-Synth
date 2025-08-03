#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible(levelSlider);
    levelSlider.setRange (-100, 0, 0.1);
    levelSlider.setValue (-13, juce::dontSendNotification);
    levelSlider.setTextValueSuffix (" dB");
    levelSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    levelSlider.setSliderStyle (juce::Slider::LinearVertical);
    levelSlider.onValueChange = [this] {
        processorRef.setLevel (juce::Decibels::decibelsToGain (levelSlider.getValue()));
    };

    addAndMakeVisible (levelLabel);
    levelLabel.setText ("Output\nLevel", juce::dontSendNotification);
    levelLabel.attachToComponent (&levelSlider, false);
    levelLabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible (envelopeComponent);
    envelopeComponent.onChanged = [this] {
        processorRef.setEnvelopeParameters (envelopeComponent.getEnvelopeParameters());
    };

    setSize (800, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    envelopeComponent.setBounds (10, 50, getWidth()/2 - 20, 200);
    levelLabel       .setBounds (getWidth() - 110, 50, 120, 40);
    levelSlider      .setBounds (getWidth() - 90, 100, 60, 150);
}
