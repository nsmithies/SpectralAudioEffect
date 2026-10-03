#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

SpectralAudioEffectAudioProcessor::SpectralAudioEffectAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

bool SpectralAudioEffectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto in = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo()) && in == out;
}

void SpectralAudioEffectAudioProcessor::prepareToPlay(double sampleRate, int)
{
    currentSampleRate = sampleRate;
    const int size = juce::jmax(1, static_cast<int>(std::ceil(bufferSeconds * sampleRate)));
    circularBuffer.setSize(2, size);
    circularBuffer.clear();
    writePosition = 0;
    samplesUntilNextGrain = 0.0;
    feedbackL = feedbackR = 0.0f;
    for (auto& g : grains) g = {};
}

float SpectralAudioEffectAudioProcessor::hann(int age, int length)
{
    if (length <= 1) return 1.0f;
    const float phase = static_cast<float>(age) / static_cast<float>(length - 1);
    return 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * phase);
}

float SpectralAudioEffectAudioProcessor::readInterpolated(int channel, double position) const
{
    const int size = circularBuffer.getNumSamples();
    if (size <= 0) return 0.0f;
    while (position < 0.0) position += size;
    while (position >= size) position -= size;
    const int i0 = static_cast<int>(position);
    const int i1 = (i0 + 1) % size;
    const float frac = static_cast<float>(position - i0);
    const float a = circularBuffer.getSample(juce::jmin(channel, circularBuffer.getNumChannels() - 1), i0);
    const float b = circularBuffer.getSample(juce::jmin(channel, circularBuffer.getNumChannels() - 1), i1);
    return a + (b - a) * frac;
}

void SpectralAudioEffectAudioProcessor::startGrain(float grainMs, float pitchSemitones)
{
    auto* grain = std::find_if(grains.begin(), grains.end(), [](const Grain& g) { return !g.active; });
    if (grain == grains.end()) return;

    const int size = circularBuffer.getNumSamples();
    const int grainSamples = juce::jlimit(32, juce::jmax(32, size / 2),
        static_cast<int>(grainMs * 0.001f * static_cast<float>(currentSampleRate)));
    const int safety = juce::jmax(64, grainSamples / 4);

    grain->active = true;
    grain->age = 0;
    grain->length = grainSamples;
    grain->increment = std::pow(2.0, static_cast<double>(pitchSemitones) / 12.0);
    grain->readPosition = static_cast<double>(writePosition - grainSamples - safety);
    while (grain->readPosition < 0.0) grain->readPosition += size;
}

void SpectralAudioEffectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();
    const int circSize = circularBuffer.getNumSamples();
    if (circSize == 0) return;

    const float mix = parameters.getRawParameterValue("mix")->load();
    const float grainMs = parameters.getRawParameterValue("grainSize")->load();
    const float density = parameters.getRawParameterValue("density")->load();
    const float pitch = parameters.getRawParameterValue("pitch")->load();
    const float feedback = parameters.getRawParameterValue("feedback")->load();
    const bool freeze = parameters.getRawParameterValue("freeze")->load() > 0.5f;
    const double interval = currentSampleRate / juce::jmax(0.25f, density);

    for (int s = 0; s < numSamples; ++s)
    {
        const float dryL = buffer.getSample(0, s);
        const float dryR = channels > 1 ? buffer.getSample(1, s) : dryL;

        if (!freeze)
        {
            circularBuffer.setSample(0, writePosition, dryL + feedbackL * feedback);
            circularBuffer.setSample(1, writePosition, dryR + feedbackR * feedback);
        }

        if (samplesUntilNextGrain <= 0.0)
        {
            startGrain(grainMs, pitch);
            samplesUntilNextGrain += interval;
        }
        samplesUntilNextGrain -= 1.0;

        float wetL = 0.0f, wetR = 0.0f;
        int active = 0;
        for (auto& g : grains)
        {
            if (!g.active) continue;
            const float env = hann(g.age, g.length);
            wetL += readInterpolated(0, g.readPosition) * env;
            wetR += readInterpolated(1, g.readPosition) * env;
            ++active;

            g.readPosition += g.increment;
            if (g.readPosition >= circSize) g.readPosition -= circSize;
            ++g.age;
            if (g.age >= g.length) g.active = false;
        }

        if (active > 0)
        {
            const float norm = 1.0f / std::sqrt(static_cast<float>(active));
            wetL *= norm;
            wetR *= norm;
        }

        feedbackL = juce::jlimit(-1.0f, 1.0f, wetL);
        feedbackR = juce::jlimit(-1.0f, 1.0f, wetR);

        buffer.setSample(0, s, dryL * (1.0f - mix) + wetL * mix);
        if (channels > 1)
            buffer.setSample(1, s, dryR * (1.0f - mix) + wetR * mix);

        if (!freeze)
        {
            ++writePosition;
            if (writePosition >= circSize) writePosition = 0;
        }
    }
}

juce::AudioProcessorEditor* SpectralAudioEffectAudioProcessor::createEditor()
{
    return new SpectralAudioEffectAudioProcessorEditor(*this);
}

void SpectralAudioEffectAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml()) copyXmlToBinary(*xml, destData);
}

void SpectralAudioEffectAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout SpectralAudioEffectAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.65f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("grainSize", "Grain Size", juce::NormalisableRange<float>(20.0f, 500.0f, 0.1f, 0.5f), 120.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("density", "Density", juce::NormalisableRange<float>(1.0f, 40.0f, 0.1f), 10.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("feedback", "Feedback", juce::NormalisableRange<float>(0.0f, 0.85f, 0.001f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("freeze", "Freeze", false));
    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectralAudioEffectAudioProcessor();
}
