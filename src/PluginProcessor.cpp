#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Weierstrass.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int /* samplesPerBlock */)
{
        for (auto i = 0; i < 10; ++i)
            synth.addVoice (new WeierstrassVoice(sampleRate));

        synth.addSound (new WeierstrassSound(sampleRate));

        synth.setCurrentPlaybackSampleRate (sampleRate);
        midiCollector.reset (sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::setEnvelopeParameters (juce::ADSR::Parameters newParams)
{
    for (int voiceIndex = 0; voiceIndex < synth.getNumVoices(); ++voiceIndex)
    {
        if (auto* weierstrassVoice = dynamic_cast<WeierstrassVoice*> (synth.getVoice (voiceIndex)))
        {
            weierstrassVoice->onEnvelopeChanged(newParams);
        }
    }
}

void AudioPluginAudioProcessor::setLevel (double level)
{
    m_targetLevel = level;
}

void AudioPluginAudioProcessor::setWeierstrassParameters (double a, double b)
{
    auto ref_count_ptr = synth.getSound (0);
    auto* weierstrassSound = dynamic_cast<WeierstrassSound*>(ref_count_ptr.get());
    jassert (weierstrassSound != nullptr);
    weierstrassSound->createWavetable(a, b);
    // ref_count_ptr is guaranteed to live until here
}

juce::SynthesiserSound::Ptr AudioPluginAudioProcessor::getSoundRef() const
{
    return synth.getSound (0);
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    midiCollector.removeNextBlockOfMessages (midiMessages, buffer.getNumSamples());
    keyboardState.processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());


    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= (float) m_level; // Apply level to the output
        }
    }

    if (!juce::approximatelyEqual (m_level, m_targetLevel)) {
        auto localLevel = m_targetLevel;
        buffer.applyGainRamp (0, buffer.getNumSamples(), (float) m_level, (float) localLevel);
        m_level = localLevel;
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
