#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

struct WeierstrassSound   : public juce::SynthesiserSound
{
    WeierstrassSound(double sampleRate);

    inline bool appliesToNote    (int) override        { return true; }
    inline bool appliesToChannel (int) override        { return true; }

    inline juce::AudioSampleBuffer& get_wavetable() noexcept
    {
        return wavetable;
    }

    inline unsigned int get_table_size() const noexcept
    {
        return tableSize;
    }

    void createWavetable();

private:
    juce::AudioSampleBuffer wavetable;
    unsigned int tableSize;
};

//==============================================================================
struct WeierstrassVoice   : public juce::SynthesiserVoice
{
    WeierstrassVoice(double sampleRate);

    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote (float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved (int) override      {}
    void controllerMoved (int, int) override {}

    void onEnvelopeChanged(juce::ADSR::Parameters newParams);

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;

private:
    float m_currentIndex = 0.0f, m_tableDelta = 0.0f, m_velocity;
    juce::ADSR m_envelope;
    WeierstrassSound* m_currentSound;
};