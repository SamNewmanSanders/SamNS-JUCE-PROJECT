#pragma once

#include <JuceHeader.h>
#include "../Controllers/MainController.h"
#include "SongBrowserComponent.h"
#include "SongComponent.h"

class MainUI : public juce::Component
{
public:
    explicit MainUI(MainController& controller);
    ~MainUI() override = default;

    void resized() override;

private:
    SongBrowserComponent songBrowser;
    MainController& mainController;

    std::array<std::unique_ptr<SongComponent>, 4> songComponents;
    int songSlotCounter = 0;

    void handleSongSelected(const juce::String& songName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainUI)
};
