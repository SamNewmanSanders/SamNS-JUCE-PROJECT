#include "MainController.h"

MainController::MainController(const juce::File& songsRootFolder, AudioEngine& engine)
    : rootFolder(songsRootFolder), audioEngine(engine)
{
}

std::vector<juce::String> MainController::getAvailableSongNames() const
{
    std::vector<juce::String> names;

    if (rootFolder.isDirectory())
    {
        for (const auto& entry : rootFolder.findChildFiles(juce::File::findDirectories, false))
        {
            names.push_back(entry.getFileName());
        }
    }

    return names;
}

std::shared_ptr<Song> MainController::loadSongByName(const juce::String& songName)
{
    juce::File folder = rootFolder.getChildFile(songName);
    if (!folder.exists() || !folder.isDirectory())
        return nullptr;

    return std::make_shared<Song>(folder, songName);
}

void MainController::addSongToAudio(std::shared_ptr<Song> song)
{
    audioEngine.addSong(std::move(song));
}
