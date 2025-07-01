#pragma once
#include <JuceHeader.h>
#include "../Controllers/SessionManager.h"
#include "StemComponent.h"  

class SongComponent : public juce::Component
{
public:
    SongComponent(SessionManager& manager, juce::String songId);

    void resized() override;

private:
    void handleStart();
    void handleStop();

    SessionManager& sessionManager;
    juce::String songId;

    juce::Label titleLabel;
    juce::TextButton startButton{ "Start" };
    juce::TextButton stopButton{ "Stop" };


    std::unique_ptr<StemComponent> drumsStem;
    std::unique_ptr<StemComponent> vocalsStem;
    std::unique_ptr<StemComponent> bassStem;
    std::unique_ptr<StemComponent> otherStem;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SongComponent)
};
