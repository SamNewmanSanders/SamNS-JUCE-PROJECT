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

    // Setup volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.8);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    volumeSlider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(volumeSlider);

    // Add listener to volume slider to handle changes
    volumeSlider.onValueChange = [this]()
        {
            volumeChanged();
        };

    // Setup mute button as a TextButton (NOT toggle or checkbox)
    bool initialMuted = false; // Or query your sessionManager for real mute state

    muteButton.setButtonText(getMuteButtonText(initialMuted));
    muteButton.onClick = [this]()
        {
            muteToggled();
        };
    addAndMakeVisible(muteButton);

    // Moment buttons
    for (int i = 0; i < 4; ++i)
    {
        momentButtons[i].setButtonText("Moment " + juce::String(i + 1));
        addAndMakeVisible(momentButtons[i]);
    }

    updateMuteButtonColor(initialMuted);
}


void StemComponent::resized()
{
    auto area = getLocalBounds().reduced(5);

    int height = area.getHeight();

    int labelWidth = 80;
    int muteButtonWidth = 80;   // wider for text "Unmute"
    int momentButtonWidth = 70;
    int momentButtonsTotalWidth = momentButtonWidth * 4;

    int sliderWidth = area.getWidth() - (labelWidth + muteButtonWidth + momentButtonsTotalWidth + 5 * 5);

    int x = area.getX();

    stemLabel.setBounds(x, area.getY(), labelWidth, height);
    x += labelWidth + 5;

    volumeSlider.setBounds(x, area.getY(), sliderWidth, height);
    x += sliderWidth + 5;

    muteButton.setBounds(x, area.getY(), muteButtonWidth, height);
    x += muteButtonWidth + 5;

    for (int i = 0; i < 4; ++i)
    {
        momentButtons[i].setBounds(x, area.getY(), momentButtonWidth, height);
        x += momentButtonWidth + 5;
    }
}

void StemComponent::muteToggled()
{
    // Toggle mute state (get current from button text)
    bool currentlyMuted = (muteButton.getButtonText() == "Unmute");

    // New muted state is opposite
    bool newMuted = !currentlyMuted;

    sessionManager.setStemMute(songId, stemType, newMuted);

    muteButton.setButtonText(getMuteButtonText(newMuted));
    updateMuteButtonColor(newMuted);
}

void StemComponent::updateMuteButtonColor(bool muted)
{
    if (muted)
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    else
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
}

juce::String StemComponent::getMuteButtonText(bool muted) const
{
    return muted ? "Unmute" : "Mute";
}


void StemComponent::volumeChanged()
{
    float newVolume = (float)volumeSlider.getValue();

    // Inform the session manager of the new volume for this stem
    sessionManager.setStemVolume(songId, stemType, newVolume);
}