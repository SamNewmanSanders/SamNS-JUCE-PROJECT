// MainComponent.h

#pragma once
#include <JuceHeader.h>
#include "AudioEngine.h"

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override {}
    void resized() override {}

private:
    AudioEngine engine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

