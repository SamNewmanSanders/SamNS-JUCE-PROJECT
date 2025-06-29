#pragma once

#include <JuceHeader.h>
#include "../Audio/Song.h"
#include "../Audio/AudioEngine.h"

class MainController
{
public:
    MainController(const juce::File& songsRootFolder, AudioEngine& engine);

    std::vector<juce::String> getAvailableSongNames() const;
    std::shared_ptr<Song> loadSongByName(const juce::String& songName);
    void addSongToAudio(std::shared_ptr<Song> song);

private:
    juce::File rootFolder;
    AudioEngine& audioEngine;  // reference to audio engine
};
