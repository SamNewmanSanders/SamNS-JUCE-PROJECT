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
    bufferToFill.clearActiveBufferRegion();

    juce::AudioBuffer<float> mixBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    mixBuffer.clear();

    juce::AudioSourceChannelInfo mixInfo(&mixBuffer, 0, bufferToFill.numSamples);

    for (auto& stem : Stems)
    {

        // Let the stem write its output into mixBuffer directly (overwrite each time)
        stem->getNextAudioBlock(mixInfo);

        float vol = stem->isMuted() ? 0.0f : stem->getVolume();

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
                mixBuffer, channel, 0,
                bufferToFill.numSamples, vol);
        }

        mixBuffer.clear(); // Clear after using for this stem, ready for next one
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

void StemMixer::setStemMute(StemType stemType, bool mute)
{
    for (auto& stem : Stems)
    {
        if (stem->getStemType() == stemType)
        {
            stem->setMuted(mute); // use new Stem method
        }
    }
}

void StemMixer::setStemVolume(StemType stemType, float newVolume)
{
	for (auto& stem : Stems)
	{
		if (stem->getStemType() == stemType)
		{
			// Assuming Stem has a method to set volume
			stem->setVolume(newVolume); // Implement this method in Stem class
		}
	}
}
