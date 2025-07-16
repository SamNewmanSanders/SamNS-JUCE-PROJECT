#pragma once

#include <JuceHeader.h>
#include "StemMixer.h"
#include "../Helpers/StemType.h"
#include "Stretching/StretchJob.h"

class Song : public juce::AudioSource
{
public:
    explicit Song(const juce::File& folder, juce::String songName);
    ~Song() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void stop();

    void setStemMute(StemType stemType, bool mute);
    void setStemVolume(StemType stemType, float newVolume);

    juce::String getName() const { return songName; }

private:
    bool loadFromFolder(const juce::File& folder);
    void addStem(const juce::File& file, StemType stemType);

    StemMixer stemMixer;
    juce::String songName;

    float currentTempoRatio = 0.8f;
    double currentSampleRate = 0.0;

    std::unique_ptr<juce::ThreadPool> pool;

    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Song)
};