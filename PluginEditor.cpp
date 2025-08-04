#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
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

    addAndMakeVisible (weierstrassASlider);
    weierstrassASlider.setRange (0.0, 1.0, 0.01);
    weierstrassASlider.setValue (0.5, juce::dontSendNotification);
    weierstrassASlider.setSliderStyle (juce::Slider::Rotary);
    weierstrassASlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    weierstrassASlider.onValueChange = [this] {
        if (!isTimerRunning()) startTimer(100);
        // we do not want to call setWeierstrassParameters too often, so we start a timer
    };

    addAndMakeVisible (weierstrassALabel);
    weierstrassALabel.setText ("a", juce::dontSendNotification);
    weierstrassALabel.attachToComponent (&weierstrassASlider, false);
    weierstrassALabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible (weierstrassBSlider);
    weierstrassBSlider.setRange (7.0, 30.0, 0.01);
    weierstrassBSlider.setValue (7.0, juce::dontSendNotification);
    weierstrassBSlider.setSliderStyle (juce::Slider::Rotary);
    weierstrassBSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    weierstrassBSlider.onValueChange = [this] {
        if (!isTimerRunning()) startTimer(100);
    };

    addAndMakeVisible (weierstrassBLabel);
    weierstrassBLabel.setText ("b", juce::dontSendNotification);
    weierstrassBLabel.attachToComponent (&weierstrassBSlider, false);
    weierstrassBLabel.setJustificationType (juce::Justification::centred);

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
    weierstrassASlider.setBounds (10, getHeight() - 110, 100, 100);
    weierstrassBSlider.setBounds (120, getHeight() - 110, 100, 100);
    envelopeComponent.setBounds (230, 10, getWidth() - 350, getHeight() - 20);
    levelSlider.setBounds (getWidth() - 110, 50, 100, getHeight() - 100);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Here we update the Weierstrass parameters, no more than once every 100 ms
    // to avoid flooding the processor with too many calls.
    stopTimer();
    processorRef.setWeierstrassParameters(weierstrassASlider.getValue(), weierstrassBSlider.getValue());
}
