#include "StemMixer.h"

StemMixer::StemMixer()
{
    // Constructor implementation (can be empty)
}

StemMixer::~StemMixer()
{
    // Destructor implementation (can be empty)
}

void StemMixer::addStem(std::shared_ptr<Stem> Stem)
{
    Stems.push_back(std::move(Stem));
}

void StemMixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    for (auto& Stem : Stems)
        Stem->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void StemMixer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear output buffer before mixing
    bufferToFill.clearActiveBufferRegion();

    // Temporary buffer for mixing each Stem
    juce::AudioBuffer<float> mixBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo mixInfo(&mixBuffer, 0, bufferToFill.numSamples);

    for (auto& Stem : Stems)
    {
        mixBuffer.clear();
        Stem->getNextAudioBlock(mixInfo);

        // Mix this Stem's buffer into the output buffer
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
                mixBuffer, channel, 0, bufferToFill.numSamples);
    }
}

void StemMixer::releaseResources()
{
    for (auto& Stem : Stems)
        Stem->releaseResources();
}


void StemMixer::startAll()
{
    for (auto& Stem : Stems)
    {
        Stem->start();
    }
}

void StemMixer::stopAll()
{
    for (auto& Stem : Stems)
    {
        Stem->stop();
    }
}
