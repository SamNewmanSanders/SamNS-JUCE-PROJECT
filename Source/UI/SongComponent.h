#pragma once

#include <JuceHeader.h>
#include "../Audio/Song.h"

class SongComponent : public juce::Component
{
public:
    explicit SongComponent(std::shared_ptr<Song> s);
    ~SongComponent() override = default;

    void resized() override;

private:
    std::shared_ptr<Song> song;

    juce::Label titleLabel;
    juce::TextButton startButton{ "Start" };
    juce::TextButton stopButton{ "Stop" };

    void handleStart();
    void handleStop();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SongComponent)
};
