#include "SongComponent.h"

SongComponent::SongComponent(SessionManager& manager, juce::String songId)
    : sessionManager(manager), songId(songId)
{
    titleLabel.setText(sessionManager.getSongName(songId), juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    startButton.onClick = [this] { handleStart(); };
    stopButton.onClick = [this] { handleStop(); };
    addAndMakeVisible(startButton);
    addAndMakeVisible(stopButton);

    drumsStem = std::make_unique<StemComponent>(sessionManager, songId, StemType::Drums);
    vocalsStem = std::make_unique<StemComponent>(sessionManager, songId, StemType::Vocals);
    bassStem = std::make_unique<StemComponent>(sessionManager, songId, StemType::Bass);
    otherStem = std::make_unique<StemComponent>(sessionManager, songId, StemType::Other);

    addAndMakeVisible(drumsStem.get());
    addAndMakeVisible(vocalsStem.get());
    addAndMakeVisible(bassStem.get());
    addAndMakeVisible(otherStem.get());

    DBG("SongComponent created for song ID: " + songId +
        " with name: " + sessionManager.getSongName(songId));
}

void SongComponent::handleStart()
{
    sessionManager.playSong(songId);
}

void SongComponent::handleStop()
{
    sessionManager.stopSong(songId);
}

void SongComponent::resized()
{
    auto area = getLocalBounds().reduced(10);

    titleLabel.setBounds(area.removeFromTop(25));

    auto buttonArea = area.removeFromTop(30);
    int buttonWidth = buttonArea.getWidth() / 2;
    startButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(buttonArea.reduced(2));

    int stemHeight = area.getHeight() / 4;
    drumsStem->setBounds(area.removeFromTop(stemHeight));
    vocalsStem->setBounds(area.removeFromTop(stemHeight));
    bassStem->setBounds(area.removeFromTop(stemHeight));
    otherStem->setBounds(area.removeFromTop(stemHeight));
}
