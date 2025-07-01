#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "Controllers/SessionManager.h"
#include "UI/MainUI.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioEngine audioEngine;
    std::unique_ptr<SessionManager> sessionManager;
    std::unique_ptr<MainUI> mainUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
