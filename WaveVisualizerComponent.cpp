#include "WaveVisualizerComponent.h"
#include "PluginProcessor.h"

WaveVisualizerComponent::WaveVisualizerComponent ()
    : juce::Component(), canvas (juce::Image::ARGB, 800, 400, true)
{
    setOpaque (true); // Make the component opaque to avoid flickering
}

WaveVisualizerComponent::~WaveVisualizerComponent()
{
}


void WaveVisualizerComponent::setWaveform (juce::AudioSampleBuffer const& waveform)
{
    canvas = juce::Image (juce::Image::ARGB, 800, 400, true);
    auto graphics = juce::Graphics (canvas);
    graphics.fillAll (juce::Colours::black);

    // Draw the waveform
    graphics.setColour (juce::Colours::white);

    auto delta_x = 800.0f / (float) waveform.getNumSamples();
    auto delta_y = 200.0f;
    for (int channel = 0; channel < waveform.getNumChannels(); ++channel)
    {
        auto* channelData = waveform.getReadPointer (channel);
        for (int sample = 0; sample < waveform.getNumSamples() - 1; ++sample)
        {
            float x1 = (float) sample * delta_x;
            float y1 = (1 - channelData[sample]) * delta_y;
            float x2 = ((float) sample + 1) * delta_x;
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
