#pragma once
#include <JuceHeader.h>
#include "Stem.h"

class StemMixer : public juce::AudioSource
{
public:
    StemMixer();
    ~StemMixer();

    void addStem(std::unique_ptr<Stem> Stem);


    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void startAll();
    void stopAll();

private:
    std::vector<std::unique_ptr<Stem>> Stems;
};
