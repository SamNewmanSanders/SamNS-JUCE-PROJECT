#include "MainUI.h"

MainUI::MainUI(MainController& controller)
    : mainController(controller)
{
    addAndMakeVisible(songBrowser);

    auto songNames = mainController.getAvailableSongNames();
    songBrowser.setSongNames(songNames);

    songBrowser.onSongSelected = [this](const juce::String& selectedName) {
        handleSongSelected(selectedName);
        };

    DBG("Song selector callback connected");
}

void MainUI::resized()
{
    auto area = getLocalBounds().reduced(10);

    const int selectorHeight = 40;
    songBrowser.setBounds(area.removeFromTop(selectorHeight).withTrimmedBottom(5));

    // 2x2 grid layout for up to 4 SongComponents
    auto gridArea = area;
    auto halfWidth = gridArea.getWidth() / 2;
    auto halfHeight = gridArea.getHeight() / 2;

    for (int i = 0; i < 4; ++i)
    {
        if (songComponents[i])
        {
            int col = i % 2;
            int row = i / 2;
            songComponents[i]->setBounds(
                gridArea.getX() + col * halfWidth,
                gridArea.getY() + row * halfHeight,
                halfWidth,
                halfHeight
            );
        }
    }
}

void MainUI::handleSongSelected(const juce::String& songName)
{
    auto song = mainController.loadSongByName(songName);
    if (!song)
        return;

    mainController.addSongToAudio(song);

    auto songComponent = std::make_unique<SongComponent>(song);
    addAndMakeVisible(songComponent.get());

    // Replace slot in round-robin fashion
    songComponents[songSlotCounter % 4] = std::move(songComponent);
    ++songSlotCounter;

    resized();
}
