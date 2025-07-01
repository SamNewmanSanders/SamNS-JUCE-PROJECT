#pragma once
#include <JuceHeader.h>
#include "Stem.h"
#include "../Helpers/StemType.h"


class StemMixer : public juce::AudioSource
{
public:
    StemMixer();
    ~StemMixer();

    void addStem(std::shared_ptr<Stem> Stem);


    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void startAll();
    void stopAll();

    void setStemMute(StemType stemType, bool mute);
	void setStemVolume(StemType stemType, float newVolume);


private:
    std::vector<std::shared_ptr<Stem>> Stems;
};
