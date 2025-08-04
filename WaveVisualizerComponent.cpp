#include "WaveVisualizerComponent.h"
#include "PluginProcessor.h"

WaveVisualizerComponent::WaveVisualizerComponent (unsigned width, unsigned height, juce::AudioSampleBuffer const& waveform)
    : juce::Component(), canvas (juce::Image::ARGB, width, height, true)
{
    setOpaque (true); // Make the component opaque to avoid flickering
    setSize (width, height);
    setWaveform (waveform);
}

WaveVisualizerComponent::~WaveVisualizerComponent()
{
}


void WaveVisualizerComponent::setWaveform (juce::AudioSampleBuffer const& waveform)
{
    auto graphics = juce::Graphics (canvas);
    graphics.fillAll (juce::Colours::black);

    // Draw the waveform
    graphics.setColour (juce::Colours::white);

    auto delta_x = static_cast<float> (getWidth()) / waveform.getNumSamples();
    auto delta_y = static_cast<float> (getHeight()) / 2.0f;
    for (int channel = 0; channel < waveform.getNumChannels(); ++channel)
    {
        auto* channelData = waveform.getReadPointer (channel);
        for (int sample = 0; sample < waveform.getNumSamples() - 1; ++sample)
        {
            float x1 = sample * delta_x;
            float y1 = (1 - channelData[sample]) * delta_y;
            float x2 = (sample + 1) * delta_x;
            float y2 = (1 - channelData[sample + 1]) * delta_y;

            graphics.drawLine (x1, y1, x2, y2);
        }
    }
    repaint(); // Request a repaint to update the visualizer
}

void WaveVisualizerComponent::paint (juce::Graphics& g)
{
    g.drawImage (canvas, getLocalBounds().toFloat());
}
