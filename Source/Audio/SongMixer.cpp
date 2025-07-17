#include "SongMixer.h"
#include "../UI/StemComponent.h" // For StemType enum

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
	song->setTempoOnCreation(currentSelectedTempo); // Set initial tempo on creation
    songs.push_back(std::move(song));
}

void SongMixer::removeSong(std::shared_ptr<Song> song)
{
	auto it = std::remove(songs.begin(), songs.end(), song);
	if (it != songs.end())
	{
		songs.erase(it, songs.end());
	}
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

void SongMixer::startSong(std::shared_ptr<Song> song)
{
	if (song)
	{
		song->start();
	}
}

void SongMixer::stopSong(std::shared_ptr<Song> song)
{
	if (song)
	{
		song->stop();
	}
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


void SongMixer::setStemMute(juce::String songId, StemType stemType, bool mute)
{
	for (auto& song : songs)
	{
		if (song->getName() == songId)
		{
			song->setStemMute(stemType, mute);
			break;
		}
	}
}

void SongMixer::setStemVolume(juce::String songId, StemType stemType, float newVolume)
{
	for (auto& song : songs)
	{
		if (song->getName() == songId)
		{
			song->setStemVolume(stemType, newVolume);
			break;
		}
	}
}
