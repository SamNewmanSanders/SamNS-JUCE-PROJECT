#pragma once
#include <JuceHeader.h>
#include "../Controllers/SessionManager.h"
#include "SongBrowserComponent.h"
#include "SongComponent.h"

class MainUI : public juce::Component
{
public:
    explicit MainUI(SessionManager& sessionManager);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void handleSongSelected(const juce::String& name);

    SessionManager& sessionManager;
    SongBrowserComponent songBrowser;

    std::vector<std::unique_ptr<SongComponent>> songComponents;
    int songSlotCounter = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainUI)
};
