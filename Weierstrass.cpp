#include "Weierstrass.h"

WeierstrassSound::WeierstrassSound(double sampleRate)
{
    tableSize = sampleRate / 20.0; // 20 Hz base frequency
    wavetable.setSize (1, (int) tableSize + 1);
    wavetable.clear();
    createWavetable();
}

void WeierstrassSound::createWavetable()
{
    auto* samples = wavetable.getWritePointer (0);

    int harmonics[] = { 1, 3, 5, 6, 7, 9, 13, 15 };
    float harmonicWeights[] = { 0.5f, 0.1f, 0.05f, 0.125f, 0.09f, 0.005f, 0.002f, 0.001f };

    jassert (juce::numElementsInArray (harmonics) == juce::numElementsInArray (harmonicWeights));

    for (auto harmonic = 0; harmonic < juce::numElementsInArray (harmonics); ++harmonic)
    {
        auto angleDelta = juce::MathConstants<double>::twoPi / (double) (tableSize) * harmonics[harmonic];
        auto currentAngle = 0.0;

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin (currentAngle);
            samples[i] += (float) sample * harmonicWeights[harmonic];
            currentAngle += angleDelta;
        }
    }

    samples[tableSize] = samples[0];
}

WeierstrassVoice::WeierstrassVoice(double sampleRate)
{
    m_envelope.setSampleRate (sampleRate);
}

bool WeierstrassVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<WeierstrassSound*> (sound) != nullptr;
}

void WeierstrassVoice::startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int /*currentPitchWheelPosition*/)
{
    m_currentSound = dynamic_cast<WeierstrassSound*> (sound);
    jassert (m_currentSound != nullptr);

    m_currentIndex = 0.0;
    m_velocity = velocity;
    m_envelope.noteOn();

    auto frequency = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    auto tableSizeOverSampleRate = (float) m_currentSound->get_table_size() / (float) getSampleRate();
    m_tableDelta = frequency * tableSizeOverSampleRate;
}

void WeierstrassVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_envelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        m_envelope.reset();
        m_tableDelta = 0.0;
    }
}

void WeierstrassVoice::onEnvelopeChanged(juce::ADSR::Parameters newParams)
{
    m_envelope.setParameters(newParams);
}   

void WeierstrassVoice::renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (m_tableDelta > 0.0)
    {
        auto endSample = startSample + numSamples;
        for (auto sample = startSample; sample < endSample; ++sample)
        {
            auto index = (unsigned int) m_currentIndex;

            auto* table = m_currentSound->get_wavetable().getReadPointer (0);

            auto value = table[index] * m_envelope.getNextSample() * m_velocity;

            if ((m_currentIndex += m_tableDelta) > (float) m_currentSound->get_table_size())
                m_currentIndex -= (float) m_currentSound->get_table_size();

            for (auto channel = outputBuffer.getNumChannels(); --channel >= 0;)
                outputBuffer.addSample (channel, sample, value);
        }

        if (!m_envelope.isActive()) {
            clearCurrentNote();
            m_envelope.reset();
            m_tableDelta = 0.0;
        }
    }
}