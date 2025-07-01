#pragma once
#include <JuceHeader.h>
#include "Song.h"

class SongMixer : public juce::AudioSource
{
public:
    SongMixer();
    ~SongMixer();

    void addSong(std::shared_ptr<Song> song);
    void removeSong(std::shared_ptr<Song> song);

    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

	void startSong(std::shared_ptr<Song> song);
    void stopSong(std::shared_ptr<Song> song);
    void startAll();
    void stopAll();

	void setStemMute(juce::String songId, StemType stemType, bool mute);
	void setStemVolume(juce::String songId, StemType stemType, float newVolume);

private:
    std::vector<std::shared_ptr<Song>> songs;
};
