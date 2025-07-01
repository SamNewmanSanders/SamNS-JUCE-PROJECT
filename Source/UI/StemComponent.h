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
    void updateMuteButtonColor(bool muted);

private:
    SessionManager& sessionManager;
    juce::String songId;
    StemType stemType;

    juce::Label stemLabel;
    juce::Slider volumeSlider;
    juce::ToggleButton muteButton;

    //void volumeChanged();
    void muteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemComponent)
};
