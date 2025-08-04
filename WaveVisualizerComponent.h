#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class WaveVisualizerComponent : public juce::Component
{
public:
    WaveVisualizerComponent (unsigned width, unsigned height, juce::AudioSampleBuffer const& waveform);
    ~WaveVisualizerComponent() override;

    void setWaveform (juce::AudioSampleBuffer const& waveform);

    void paint (juce::Graphics& g) override;

private:
    juce::Image canvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveVisualizerComponent)
};