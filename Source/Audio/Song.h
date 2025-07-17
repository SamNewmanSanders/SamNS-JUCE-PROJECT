#pragma once

#include <JuceHeader.h>
#include "StemMixer.h"
#include "../Helpers/StemType.h"
#include "Stretching/StretchJob.h"

class Song : public juce::AudioSource
{
public:
    explicit Song(const juce::File& folder, juce::String songName, int selectedtempooncreation);
    ~Song() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void stop();

    void setStemMute(StemType stemType, bool mute);
    void setStemVolume(StemType stemType, float newVolume);

    void setTempoOnCreation(int tempo);

    juce::String getName() const { return songName; }
	double getBpmFromJsonFile(const juce::File& jsonFile);

private:
    bool loadFromFolder(const juce::File& folder);
    void addStem(const juce::File& file, StemType stemType);

    StemMixer stemMixer;
    juce::String songName;

    float originalTempo;
	int selectedTempoOnCreation; // Default tempo, can be overridden
    float currentTempoRatio = 0.91875f; //Think this fixes samplerate bug i cba to figure out (44100/48000)
    double currentSampleRate = 0.0;

    std::unique_ptr<juce::ThreadPool> pool;

    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Song)
};