#include "MainUI.h"

MainUI::MainUI(SessionManager& sm)
    : sessionManager(sm)
{
    addAndMakeVisible(songBrowser);

    auto songNames = sessionManager.getAvailableSongNames();
    songBrowser.setSongNames(songNames);

    songBrowser.onSongSelected = [this](const juce::String& selectedName)
        {
            handleSongSelected(selectedName);
        };

    DBG("Song selector callback connected");
}

void MainUI::handleSongSelected(const juce::String& selectedSongName)
{
    DBG("MainUI: handleSongSelected called with: " + selectedSongName);

    // Load the song by name (returns song ID string, or empty string on failure)
    juce::String songId = sessionManager.loadSong(selectedSongName);

    if (songId.isEmpty())
    {
        DBG("Failed to load song: " + selectedSongName);
        return; // Loading failed
    }

    // Create a UI component for this songId (string)
    auto comp = std::make_unique<SongComponent>(sessionManager, songId);
    addAndMakeVisible(comp.get());

    // Keep up to 4 SongComponents in round-robin
    if (songComponents.size() < 4)
        songComponents.resize(4);

    songComponents[songSlotCounter % 4] = std::move(comp);
    ++songSlotCounter;

    resized();
}


void MainUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    const int selectorH = 40;
    songBrowser.setBounds(area.removeFromTop(selectorH).withTrimmedBottom(5));

    auto grid = area;
    int w = grid.getWidth() / 2, h = grid.getHeight() / 2;

    for (int i = 0; i < (int)songComponents.size(); ++i)
    {
        if (songComponents[i])
        {
            int col = i % 2, row = i / 2;
            songComponents[i]->setBounds(
                grid.getX() + col * w,
                grid.getY() + row * h,
                w, h
            );
        }
    }
}
