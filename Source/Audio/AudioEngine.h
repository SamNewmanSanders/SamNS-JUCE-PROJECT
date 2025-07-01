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

    void addSong(std::shared_ptr<Song> song);
    void removeSong(std::shared_ptr<Song> song);  

    void startSong(std::shared_ptr<Song> song);   
    void stopSong(std::shared_ptr<Song> song);   

    void startAll();
    void stopAll();

    void setStemMute(juce::String songId, StemType stemType, bool mute);


    // Get functions
    SongMixer& getSongMixer() { return songMixer; }
    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

private:
    juce::AudioDeviceManager deviceManager;
    juce::AudioSourcePlayer sourcePlayer;

    SongMixer songMixer;
};