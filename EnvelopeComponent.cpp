#include "EnvelopeComponent.h"

EnvelopeComponent::EnvelopeComponent()
{
    setSize (400, 200);

    addAndMakeVisible (attackSlider);
    addAndMakeVisible (decaySlider);
    addAndMakeVisible (sustainSlider);
    addAndMakeVisible (releaseSlider);
    addAndMakeVisible (attackLabel);
    addAndMakeVisible (decayLabel);
    addAndMakeVisible (sustainLabel);
    addAndMakeVisible (releaseLabel);
    addAndMakeVisible (titleLabel);

    attackSlider.setRange (0.01, 5.0, 0.01);
    attackSlider.setValue (0.01, juce::dontSendNotification);
    attackSlider.setTextValueSuffix (" s");
    attackSlider.setSliderStyle (juce::Slider::Rotary);
    attackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    attackSlider.onValueChange = [this] {
        envelopeParams.attack = (float) attackSlider.getValue();
        onChanged();
    };


    decaySlider.setRange (0.01, 5.0, 0.01);
    decaySlider.setValue (0.01, juce::dontSendNotification);
    decaySlider.setTextValueSuffix (" s");
    decaySlider.setSliderStyle (juce::Slider::Rotary);
    decaySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    decaySlider.onValueChange = [this] {
        envelopeParams.decay = (float) decaySlider.getValue();
        onChanged();
    };

    sustainSlider.setRange (0.0, 100.0, 1);
    sustainSlider.setValue (100.0, juce::dontSendNotification);
    sustainSlider.setTextValueSuffix (" %");
    sustainSlider.setSliderStyle (juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    sustainSlider.onValueChange = [this] {
        envelopeParams.sustain = (float) sustainSlider.getValue()/ 100.0f;
        onChanged();
    };

    releaseSlider.setRange (0.01, 5.0, 0.01);
    releaseSlider.setValue (0.01, juce::dontSendNotification);
    releaseSlider.setTextValueSuffix (" s");
    releaseSlider.setSliderStyle (juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    releaseSlider.onValueChange = [this] {
        envelopeParams.release = (float) releaseSlider.getValue();
        onChanged();
    };

    attackLabel.setText ("Attack", juce::dontSendNotification);
    decayLabel.setText ("Decay", juce::dontSendNotification);
    sustainLabel.setText ("Sustain", juce::dontSendNotification);
    releaseLabel.setText ("Release", juce::dontSendNotification);
    titleLabel.setText ("Envelope", juce::dontSendNotification);

    attackLabel.attachToComponent (&attackSlider, false);
    decayLabel.attachToComponent (&decaySlider, false);
    sustainLabel.attachToComponent (&sustainSlider, false);
    releaseLabel.attachToComponent (&releaseSlider, false);

    attackLabel.setJustificationType (juce::Justification::centred);
    decayLabel.setJustificationType (juce::Justification::centred);
    sustainLabel.setJustificationType (juce::Justification::centred);
    releaseLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setJustificationType (juce::Justification::centred);

    titleLabel.setFont (juce::Font(juce::FontOptions{20.0f, juce::Font::bold}));
}

void EnvelopeComponent::resized()
{
    auto area = getLocalBounds().reduced (40);
    auto sliderWidth = area.getWidth() / 4;
    area.removeFromTop (20); // Space for labels

    attackSlider.setBounds (area.removeFromLeft (sliderWidth));
    decaySlider.setBounds (area.removeFromLeft (sliderWidth));
    sustainSlider.setBounds (area.removeFromLeft (sliderWidth));
    releaseSlider.setBounds (area.removeFromLeft (sliderWidth));

    attackLabel.setBounds (attackSlider.getBounds().withY(40).withBottom(60));
    decayLabel.setBounds (decaySlider.getBounds().withY(40).withBottom(60));
    sustainLabel.setBounds (sustainSlider.getBounds().withY(40).withBottom(60));
    releaseLabel.setBounds (releaseSlider.getBounds().withY(40).withBottom(60));

    titleLabel.setBounds (getLocalBounds().removeFromTop (30));
}

void EnvelopeComponent::paint (juce::Graphics& g)
{
    auto area = getLocalBounds().reduced (15);
    auto titleArea = getLocalBounds();
    titleArea.removeFromBottom (getHeight() - 30);
    titleArea.removeFromLeft (getWidth() / 2 - 50);
    titleArea.removeFromRight (getWidth() / 2 - 50);

    g.setColour (juce::Colours::lightgrey);
    g.excludeClipRegion (titleArea);
    g.drawRect (area, 2); // Draw border
}