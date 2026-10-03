#include "PluginEditor.h"

SpectralAudioEffectAudioProcessorEditor::SpectralAudioEffectAudioProcessorEditor(SpectralAudioEffectAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    titleLabel.setText("SPECTRAL AUDIO EFFECT", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("LIVE GRANULAR PROCESSOR  |  MVP v0.2", juce::dontSendNotification);
    subtitleLabel.setJustificationType(juce::Justification::centred);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(160, 170, 185));
    addAndMakeVisible(subtitleLabel);

    setupKnob(grainSize, grainLabel, "GRAIN SIZE", " ms");
    setupKnob(density, densityLabel, "DENSITY", " Hz");
    setupKnob(pitch, pitchLabel, "PITCH", " st");
    setupKnob(feedback, feedbackLabel, "FEEDBACK", "");
    setupKnob(mix, mixLabel, "MIX", "");

    feedback.setNumDecimalPlacesToDisplay(2);
    mix.setNumDecimalPlacesToDisplay(2);

    freeze.setClickingTogglesState(true);
    addAndMakeVisible(freeze);

    auto& apvts = audioProcessor.parameters;
    grainAttachment = std::make_unique<SliderAttachment>(apvts, "grainSize", grainSize);
    densityAttachment = std::make_unique<SliderAttachment>(apvts, "density", density);
    pitchAttachment = std::make_unique<SliderAttachment>(apvts, "pitch", pitch);
    feedbackAttachment = std::make_unique<SliderAttachment>(apvts, "feedback", feedback);
    mixAttachment = std::make_unique<SliderAttachment>(apvts, "mix", mix);
    freezeAttachment = std::make_unique<ButtonAttachment>(apvts, "freeze", freeze);

    setSize(760, 430);
}

void SpectralAudioEffectAudioProcessorEditor::setupKnob(juce::Slider& slider, juce::Label& label,
                                                         const juce::String& name, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 82, 22);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(95, 210, 220));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(13.0f, juce::Font::bold));
    addAndMakeVisible(label);
}

void SpectralAudioEffectAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(15, 18, 24));
    g.setColour(juce::Colour::fromRGB(42, 48, 60));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 14.0f);
    g.setColour(juce::Colour::fromRGB(95, 210, 220));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 14.0f, 1.5f);
}

void SpectralAudioEffectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(32);
    titleLabel.setBounds(area.removeFromTop(44));
    subtitleLabel.setBounds(area.removeFromTop(28));
    area.removeFromTop(18);

    auto controls = area.removeFromTop(220);
    const int width = controls.getWidth() / 5;
    juce::Slider* sliders[] = { &grainSize, &density, &pitch, &feedback, &mix };
    juce::Label* labels[] = { &grainLabel, &densityLabel, &pitchLabel, &feedbackLabel, &mixLabel };
    for (int i = 0; i < 5; ++i)
    {
        auto cell = controls.removeFromLeft(width).reduced(6);
        labels[i]->setBounds(cell.removeFromTop(24));
        sliders[i]->setBounds(cell);
    }

    freeze.setBounds(getLocalBounds().getCentreX() - 55, getHeight() - 78, 110, 34);
}
