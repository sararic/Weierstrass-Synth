#include "Weierstrass.h"

WeierstrassSound::WeierstrassSound(double sampleRate)
{
    tableSize = (unsigned int) sampleRate / 20; // 20 Hz base frequency
    maxFrequency =  sampleRate / (2.0 * 20.0); // Nyquist frequency rescaled to 20 Hz
    wavetable.setSize (1, (int) tableSize + 1);
    createWavetable(0.5, 7.0);
}

void WeierstrassSound::createWavetable(double weierstrass_a, double weierstrass_b)
{
    wavetable.clear();
    auto* samples = wavetable.getWritePointer (0);

    double harmonic = 1.0;
    double weight = 1.0;

    while (harmonic <= maxFrequency)
    {
        auto angleDelta = juce::MathConstants<double>::twoPi / (double) (tableSize) * harmonic;
        double angle = 0.0;
        for (unsigned int i = 0; i < tableSize; ++i)
        {
            samples[i] += weight * std::cos(angle);
            angle += angleDelta;
        }
        harmonic *= weierstrass_b; // Increase frequency for next harmonic
        weight *= weierstrass_a; // Decrease weight for next harmonic
    }

    samples[tableSize] = samples[0];

    double gain = (1 - weierstrass_a) / (1 - weight); // Normalize the wavetable
    wavetable.applyGain(0, 0, wavetable.getNumSamples(), gain);
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