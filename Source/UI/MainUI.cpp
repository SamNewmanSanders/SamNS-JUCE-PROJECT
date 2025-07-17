// MainUI.cpp
#include "MainUI.h"

namespace
{
    constexpr int kPadding = 10;
    constexpr int kSelectorHeight = 70;   // original song-browser height
    constexpr int kGridGap = 5;
    constexpr int kGridCols = 2;
    constexpr int kGridRows = 2;
    constexpr int kMaxSongComponents = kGridCols * kGridRows;

    constexpr int kTempoLabelWidth = 60;
    constexpr int kTempoEditorWidth = 80;
    // Height of text + a little vertical padding
    constexpr int kTempoTextHeight = 24;
}

MainUI::MainUI(SessionManager& sm)
    : sessionManager(sm)
{
    // ——— Tempo label setup ———
    tempoLabel.setText("Tempo:", juce::dontSendNotification);
    tempoLabel.setFont({ 15.0f, juce::Font::bold });
    tempoLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(tempoLabel);

    // ——— Tempo editor setup ———
    tempoEditor.setInputRestrictions(3, "0123456789");
    tempoEditor.setText("135");
    tempoEditor.setJustification(juce::Justification::centred);
    addAndMakeVisible(tempoEditor);

    // ←—— HERE: wire up the callback to SessionManager:
    tempoEditor.onReturnKey = [this]()
        {
            int newTempo = tempoEditor.getText().getIntValue();
            if (newTempo > 0)
            {
                sessionManager.setTempo(newTempo);
                DBG("SessionManager::setTempo called with " << newTempo);
            }
        };

    // ——— Song browser setup ———
    addAndMakeVisible(songBrowser);
    songBrowser.setSongNames(sessionManager.getAvailableSongNames());
    songBrowser.onSongSelected = [this](auto name) { handleSongSelected(name); };

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
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);

    auto bounds = getLocalBounds().reduced(kPadding);
    auto gridStart = bounds.removeFromTop(kSelectorHeight + kGridGap);

    int cellW = bounds.getWidth() / kGridCols;
    int cellH = bounds.getHeight() / kGridRows;

    for (int r = 0; r < kGridRows; ++r)
        for (int c = 0; c < kGridCols; ++c)
            g.drawRect(bounds.getX() + c * cellW,
                bounds.getY() + r * cellH,
                cellW, cellH, 1);
}

void MainUI::resized()
{
    auto fullBounds = getLocalBounds().reduced(kPadding);
    int  w = getWidth();

    // === Song selector (centered at top) ===
    int selectorW = w / 4;
    int selectorX = (w - selectorW) / 2;
    songBrowser.setBounds(selectorX,
        kPadding,
        selectorW,
        kSelectorHeight);

    // === Tempo controls (top-right, text‑height tall, vertically centered) ===
    int tempoY = kPadding + (kSelectorHeight - kTempoTextHeight) / 2;
    int editorX = w - kPadding - kTempoEditorWidth;
    tempoEditor.setBounds(editorX,
        tempoY,
        kTempoEditorWidth,
        kTempoTextHeight);

    int labelX = editorX - kGridGap - kTempoLabelWidth;
    tempoLabel.setBounds(labelX,
        tempoY,
        kTempoLabelWidth,
        kTempoTextHeight);

    // === Grid area (below selector) ===
    auto gridArea = getLocalBounds().reduced(kPadding);
    gridArea.removeFromTop(kSelectorHeight + kGridGap);

    int cellW = gridArea.getWidth() / kGridCols;
    int cellH = gridArea.getHeight() / kGridRows;

    for (size_t i = 0; i < songComponents.size(); ++i)
    {
        if (songComponents[i])
        {
            int col = (int)i % kGridCols;
            int row = (int)i / kGridCols;
            songComponents[i]->setBounds(gridArea.getX() + col * cellW,
                gridArea.getY() + row * cellH,
                cellW, cellH);
        }
    }
}