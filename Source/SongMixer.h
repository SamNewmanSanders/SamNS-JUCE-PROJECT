#pragma once
#include <JuceHeader.h>
#include "Song.h"

class SongMixer : public juce::AudioSource
{
public:
    SongMixer();
    ~SongMixer();

    void addSong(std::unique_ptr<Song> song);


    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void startAll();
    void stopAll();

private:
    std::vector<std::unique_ptr<Song>> songs;
};
