#include "SongBrowserComponent.h"

SongBrowserComponent::SongBrowserComponent()
{
    addAndMakeVisible(songDropdown);

    // Set up the callback - called when dropdown selection changes
    songDropdown.onChange = [this]() {
        if (suppressFirstChange) {
            suppressFirstChange = false;
            return;  // Skip first automatic change
        }

        if (onSongSelected) {
            DBG("Song selected: " + songDropdown.getText());
            onSongSelected(songDropdown.getText());
        }
    };
}
void SongBrowserComponent::resized()
{
    songDropdown.setBounds(getLocalBounds().reduced(10));
}

void SongBrowserComponent::setSongNames(const std::vector<juce::String>& names)
{
    songDropdown.clear();

    for (int i = 0; i < names.size(); ++i)
        songDropdown.addItem(names[i], i + 1);

    if (!names.empty())
        songDropdown.setSelectedId(1);
}
