#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class EnvelopeComponent : public juce::Component
{
public:
    EnvelopeComponent();

    void resized() override;

    void paint (juce::Graphics& g) override;

    inline juce::ADSR::Parameters getEnvelopeParameters() const
    {
        return envelopeParams;
    }

    std::function<void()> onChanged;

private:
    juce::ADSR::Parameters envelopeParams;
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, titleLabel;

};