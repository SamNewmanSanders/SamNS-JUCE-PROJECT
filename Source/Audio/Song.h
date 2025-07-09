#pragma once

#include <JuceHeader.h>
#include "StemMixer.h"
#include "../Helpers/StemType.h"
#include <rubberband/RubberBandStretcher.h>

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
    void setTempoRatio(float newRatio);

private:
    bool loadFromFolder(const juce::File& folder);
    void addStem(const juce::File& file, StemType stemType);

    StemMixer stemMixer;
    juce::String songName;

    std::unique_ptr<RubberBand::RubberBandStretcher> stretcher;
    float currentTempoRatio = 0.98f;
    double currentSampleRate = 44100.0;

    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Song)
};
