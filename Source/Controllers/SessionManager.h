#pragma once

#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"
#include "../Audio/Song.h"
#include "../Helpers/StemType.h"


class SessionManager
{
public:
    SessionManager(AudioEngine& engine, const juce::File& rootFolder);
    ~SessionManager();

    // Loads a song by folder name, returns song name (ID) or empty string on failure
    juce::String loadSong(const juce::String& folderName);

    // Unloads a song by song ID (string)
    void unloadSong(const juce::String& songId);

    void playSong(const juce::String& songId);
    void stopSong(const juce::String& songId);

    void setStemMute(const juce::String& songId, StemType stemType, bool mute);
    void setStemVolume(const juce::String& songId, StemType stemType, float newVolume);

    void setTempo(int newTempo) { currentSelectedTempo = newTempo; }

    juce::String getSongName(const juce::String& songId) const;
    std::vector<juce::String> getAvailableSongNames() const;
    int getNumSongs() const;


private:
    AudioEngine& audioEngine;
    juce::File rootFolder;

    int currentSelectedTempo = 0;  // holds the UI’s BPM right up to load

    std::unordered_map<juce::String, std::shared_ptr<Song>> loadedSongsById;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SessionManager)
};