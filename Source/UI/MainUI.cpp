#include "MainUI.h"

namespace
{
    constexpr int kPadding = 10;
    constexpr int kSelectorHeight = 70; // Shared constant controlling selector & top region
    constexpr int kGridGap = 5;
    constexpr int kGridCols = 2;
    constexpr int kGridRows = 2;
    constexpr int kMaxSongComponents = kGridCols * kGridRows;
}

MainUI::MainUI(SessionManager& sm)
    : sessionManager(sm)
{
    addAndMakeVisible(songBrowser);

    songBrowser.setSongNames(sessionManager.getAvailableSongNames());

    songBrowser.onSongSelected = [this](const juce::String& selectedName)
        {
            handleSongSelected(selectedName);
        };

    DBG("Song selector callback connected");
}

void MainUI::handleSongSelected(const juce::String& selectedSongName)
{
    DBG("MainUI: handleSongSelected called with: " + selectedSongName);

    auto songId = sessionManager.loadSong(selectedSongName);
    if (songId.isEmpty())
    {
        DBG("Failed to load song: " + selectedSongName);
        return;
    }

    auto comp = std::make_unique<SongComponent>(sessionManager, songId);
    addAndMakeVisible(comp.get());

    if (songComponents.size() < kMaxSongComponents)
        songComponents.resize(kMaxSongComponents);

    songComponents[songSlotCounter % kMaxSongComponents] = std::move(comp);
    ++songSlotCounter;

    resized();
    repaint();
}

void MainUI::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);

    auto bounds = getLocalBounds().reduced(kPadding);
    bounds.removeFromTop(kSelectorHeight + kGridGap);

    int cellWidth = bounds.getWidth() / kGridCols;
    int cellHeight = bounds.getHeight() / kGridRows;

    for (int row = 0; row < kGridRows; ++row)
    {
        for (int col = 0; col < kGridCols; ++col)
        {
            g.drawRect(bounds.getX() + col * cellWidth,
                bounds.getY() + row * cellHeight,
                cellWidth,
                cellHeight,
                1);
        }
    }
}

void MainUI::resized()
{
    auto area = getLocalBounds().reduced(kPadding);

    // Set selector bounds
    songBrowser.setBounds(area.getX(), area.getY(), area.getWidth(), kSelectorHeight);

    // Area below selector for grid
    auto gridArea = area.withTrimmedTop(kSelectorHeight + kGridGap);

    int cellWidth = gridArea.getWidth() / kGridCols;
    int cellHeight = gridArea.getHeight() / kGridRows;

    for (size_t i = 0; i < songComponents.size(); ++i)
    {
        if (songComponents[i])
        {
            int col = static_cast<int>(i) % kGridCols;
            int row = static_cast<int>(i) / kGridCols;

            songComponents[i]->setBounds(gridArea.getX() + col * cellWidth,
                gridArea.getY() + row * cellHeight,
                cellWidth,
                cellHeight);
        }
    }
}
