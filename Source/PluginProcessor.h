#pragma once
#include <JuceHeader.h>
#include <array>

class SpectralAudioEffectAudioProcessor : public juce::AudioProcessor
{
public:
    SpectralAudioEffectAudioProcessor();
    ~SpectralAudioEffectAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    struct Grain
    {
        bool active = false;
        double readPosition = 0.0;
        double increment = 1.0;
        int age = 0;
        int length = 1;
    };

    static constexpr int maxGrains = 24;
    static constexpr double bufferSeconds = 5.0;

    juce::AudioBuffer<float> circularBuffer;
    std::array<Grain, maxGrains> grains;
    int writePosition = 0;
    double currentSampleRate = 44100.0;
    double samplesUntilNextGrain = 0.0;
    float feedbackL = 0.0f;
    float feedbackR = 0.0f;

    void startGrain(float grainMs, float pitchSemitones);
    float readInterpolated(int channel, double position) const;
    static float hann(int age, int length);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralAudioEffectAudioProcessor)
};
