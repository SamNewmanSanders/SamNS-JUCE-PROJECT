#pragma once

#include <JuceHeader.h>
#include "../Controllers/SessionManager.h"
#include "../Helpers/StemType.h"

class StemComponent : public juce::Component
{
public:
    StemComponent(SessionManager& manager, juce::String songId, StemType stemType);
    ~StemComponent() override = default;

    void resized() override;

private:
    void muteToggled();
    void updateMuteButtonColor(bool muted);
    juce::String getMuteButtonText(bool muted) const;

    void volumeChanged();

    SessionManager& sessionManager;
    juce::String songId;
    StemType stemType;

    juce::Label stemLabel;
    juce::Slider volumeSlider;
    juce::TextButton muteButton;
    juce::TextButton momentButtons[4];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemComponent)
};
