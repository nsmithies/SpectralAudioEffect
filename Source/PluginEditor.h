#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SpectralAudioEffectAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SpectralAudioEffectAudioProcessorEditor(SpectralAudioEffectAudioProcessor&);
    ~SpectralAudioEffectAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SpectralAudioEffectAudioProcessor& audioProcessor;
    juce::Label titleLabel, subtitleLabel;
    juce::Slider grainSize, density, pitch, feedback, mix;
    juce::ToggleButton freeze { "FREEZE" };
    juce::Label grainLabel, densityLabel, pitchLabel, feedbackLabel, mixLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> grainAttachment, densityAttachment, pitchAttachment, feedbackAttachment, mixAttachment;
    std::unique_ptr<ButtonAttachment> freezeAttachment;

    void setupKnob(juce::Slider&, juce::Label&, const juce::String&, const juce::String& suffix);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralAudioEffectAudioProcessorEditor)
};
