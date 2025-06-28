#pragma once

#include <JuceHeader.h>
#include "SongMixer.h"

class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    void initialise();  // Init device and audio chain
    void shutdown();    // Shutdown device and cleanup

    void addSong(std::unique_ptr<Song> song);

    void start();
    void stop();


    // Get functions
    SongMixer& getSongMixer() { return songMixer; }
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer sourcePlayer;

    SongMixer songMixer;
};