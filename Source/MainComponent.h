#pragma once

#include <JuceHeader.h>
#include "Audio/AudioEngine.h"
#include "UI/MainUI.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    std::unique_ptr<MainController> mainController;
    std::unique_ptr<MainUI> mainUI;
	AudioEngine audioEngine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
