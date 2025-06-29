#include "SongComponent.h"

SongComponent::SongComponent(std::shared_ptr<Song> s) : song(std::move(s))
{
    titleLabel.setText(song->getName(), juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    startButton.onClick = [this] { handleStart(); };
    stopButton.onClick = [this] { handleStop(); };

    addAndMakeVisible(startButton);
    addAndMakeVisible(stopButton);
}

void SongComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    titleLabel.setBounds(area.removeFromTop(25));

    auto buttonArea = area.removeFromTop(30);
    auto buttonWidth = buttonArea.getWidth() / 2;

    startButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(2));
    stopButton.setBounds(buttonArea.reduced(2));
}

void SongComponent::handleStart()
{
    if (song)
        song->start();
}

void SongComponent::handleStop()
{
    if (song)
        song->stop();
}
