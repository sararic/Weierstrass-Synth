#pragma once


#include "PluginProcessor.h"
#include "EnvelopeComponent.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::MidiKeyboardState keyboardState;

    EnvelopeComponent envelopeComponent;

    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::Slider weierstrassASlider;
    juce::Slider weierstrassBSlider;
    juce::Label weierstrassALabel;
    juce::Label weierstrassBLabel;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
