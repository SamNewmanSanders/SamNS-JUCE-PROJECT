#include "SongMixer.h"

SongMixer::SongMixer()
{
    // Constructor implementation (can be empty)
}

SongMixer::~SongMixer()
{
    // Destructor implementation (can be empty)
}

void SongMixer::addSong(std::shared_ptr<Song> song)
{
    songs.push_back(std::move(song));
}

void SongMixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    for (auto& song : songs)
        song->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void SongMixer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear output buffer before mixing
    bufferToFill.clearActiveBufferRegion();

    // Temporary buffer for mixing each song
    juce::AudioBuffer<float> mixBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo mixInfo(&mixBuffer, 0, bufferToFill.numSamples);

    for (auto& song : songs)
    {
        mixBuffer.clear();
        song->getNextAudioBlock(mixInfo);

        // Mix this song's buffer into the output buffer
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
                mixBuffer, channel, 0, bufferToFill.numSamples);
    }
}

void SongMixer::releaseResources()
{
    for (auto& song : songs)
        song->releaseResources();
}


void SongMixer::startAll()
{
    for (auto& song : songs)
    {
        song->start();
    }
}

void SongMixer::stopAll()
{
    for (auto& song : songs)
    {
        song->stop();
    }
}
