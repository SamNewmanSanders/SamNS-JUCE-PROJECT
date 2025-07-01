#include "StemComponent.h"

StemComponent::StemComponent(SessionManager& manager, juce::String songId, StemType stemType)
    : sessionManager(manager), songId(songId), stemType(stemType)
{
    juce::String labelText;
    switch (stemType)
    {
    case StemType::Vocals: labelText = "Vocals"; break;
    case StemType::Drums:  labelText = "Drums"; break;
    case StemType::Bass:   labelText = "Bass"; break;
    case StemType::Other:  labelText = "Other"; break;
    default:               labelText = "Unknown"; break;
    }

    stemLabel.setText(labelText, juce::dontSendNotification);
    stemLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(stemLabel);

    muteButton.setButtonText("Mute");
    muteButton.onClick = [this] { muteToggled(); };
    addAndMakeVisible(muteButton);

    // Initialize color based on current mute state
    bool initialMuted = false;
    // If you have a way to query current mute state, use it here instead of false
    updateMuteButtonColor(initialMuted);
    muteButton.setToggleState(initialMuted, juce::dontSendNotification);
}

void StemComponent::resized()
{
    auto area = getLocalBounds().reduced(5);

    int labelHeight = 20;
    stemLabel.setBounds(area.removeFromTop(labelHeight));

    int sliderHeight = 25;
    volumeSlider.setBounds(area.removeFromTop(sliderHeight)); // invisible

    muteButton.setBounds(area.removeFromTop(25));
}

void StemComponent::muteToggled()
{
    bool muted = muteButton.getToggleState();

    sessionManager.setStemMute(songId, stemType, muted);

    updateMuteButtonColor(muted);
}

void StemComponent::updateMuteButtonColor(bool muted)
{
    if (muted)
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    else
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
}
