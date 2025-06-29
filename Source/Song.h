#pragma once

#include <JuceHeader.h>
#include "StemMixer.h"

class Song : public juce::AudioSource
{
public:
    explicit Song(const juce::File& folder);
    ~Song() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void stop();

private:
    bool loadFromFolder(const juce::File& folder);
    void addStem(const juce::File& file, const juce::String& stemType);

    StemMixer stemMixer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Song)
};
