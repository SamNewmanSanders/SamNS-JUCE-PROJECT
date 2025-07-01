#include "SessionManager.h"

SessionManager::SessionManager(AudioEngine& engine, const juce::File& root)
    : audioEngine(engine), rootFolder(root)
{
}

SessionManager::~SessionManager()
{
	audioEngine.shutdown();
}

juce::String SessionManager::loadSong(const juce::String& folderName)
{
    if (loadedSongsById.find(folderName) != loadedSongsById.end())
    {
        DBG("Song already loaded: " + folderName);
        return folderName;
    }

    auto folder = rootFolder.getChildFile(folderName);
    if (!folder.isDirectory())
    {
        DBG("SessionManager: Invalid folder: " + folderName);
        return {}; // empty string = failure
    }

    auto songPtr = std::make_shared<Song>(folder, folderName);
    audioEngine.addSong(songPtr);
    loadedSongsById[folderName] = songPtr;

    return folderName;
}


void SessionManager::unloadSong(const juce::String& songId)
{
    auto it = loadedSongsById.find(songId);
    if (it == loadedSongsById.end())
        return;

    audioEngine.stopSong(it->second);
    audioEngine.removeSong(it->second);
    loadedSongsById.erase(it);
}

void SessionManager::playSong(const juce::String& songId)
{
    auto it = loadedSongsById.find(songId);
    if (it == loadedSongsById.end())
        return;

    audioEngine.startSong(it->second);
}

void SessionManager::stopSong(const juce::String& songId)
{
    auto it = loadedSongsById.find(songId);
    if (it == loadedSongsById.end())
        return;

    audioEngine.stopSong(it->second);
}

void SessionManager::setStemMute(const juce::String& songId, StemType stemType, bool mute)
{
    auto it = loadedSongsById.find(songId);
    if (it == loadedSongsById.end())
        return;

    audioEngine.setStemMute(songId, stemType, mute);
}

juce::String SessionManager::getSongName(const juce::String& songId) const
{
    auto it = loadedSongsById.find(songId);
    if (it == loadedSongsById.end())
        return {};

    return it->second->getName();
#
	//This function is redundant as the ID is the same as the name but will keep for now
}

int SessionManager::getNumSongs() const
{
    return static_cast<int>(loadedSongsById.size());
}

std::vector<juce::String> SessionManager::getAvailableSongNames() const
{
    std::vector<juce::String> folderNames;

    if (!rootFolder.isDirectory())
        return folderNames;

    for (auto& folder : rootFolder.findChildFiles(juce::File::findDirectories, false))
        folderNames.push_back(folder.getFileName());

    return folderNames;
}