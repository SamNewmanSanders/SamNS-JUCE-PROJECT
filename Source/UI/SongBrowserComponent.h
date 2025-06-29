#pragma once
#include <JuceHeader.h>

class SongBrowserComponent : public juce::Component
{
public:
    SongBrowserComponent();
    void resized() override;

    void setSongNames(const std::vector<juce::String>& names);

    std::function<void(juce::String)> onSongSelected;

private:
    juce::ComboBox songDropdown;
};
