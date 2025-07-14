#pragma once

#include <JuceHeader.h>
#include "../Helpers/StemType.h"
#include <rubberband/RubberBandStretcher.h>

class Stem : public juce::AudioSource
{
public:
    Stem(const juce::File& audioFile, StemType stemType);
    ~Stem();

    void preStretch(double tempoRatio);

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }

    void setMuted(bool shouldMute) { muted = shouldMute; }
    bool isMuted() const { return muted; }

    void setVolume(float newVolume) { volume = newVolume; }
    float getVolume() const { return volume; }

    StemType getStemType() const { return stemType; }

private:
    StemType stemType;
    bool muted = false;
    float volume = 1.0f;  // default full volume

    juce::AudioBuffer<float> stretchedBuffer;
    int readPosition = 0;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
};
