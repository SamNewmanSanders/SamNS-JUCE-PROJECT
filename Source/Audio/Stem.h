#pragma once

#include <JuceHeader.h>
#include "../Helpers/StemType.h"
#include <rubberband/RubberBandStretcher.h>
#include "Stretching/StretchJob.h"

class Stem : public juce::AudioSource
{
public:
    Stem(const juce::File& audioFile, StemType stemType);
    ~Stem();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Playback control
    void start() { paused = false; DBG("Stem Started"); }
    void stop() { paused = true; readPosition = 0; }

    StemType getStemType() const { return stemType; }
    void setMuted(bool m) { muted = m; }
    bool isMuted() const { return muted; }
    void setVolume(float v) { volume = v; }
    float getVolume() const { return volume; }

    void startStretching(juce::ThreadPool& pool, double tempoRatio, double sampleRate);    
    void performStretch(double tempoRatio, double sampleRate);    
    //bool isStretched() const { return stretchingFinished.load(); }

private:

    StemType stemType;
    bool muted = false;
    float volume = 1.0f;
    bool paused = true;

    // For file reading
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReader> reader;

    // Stretched audio buffer
    std::atomic<bool> stretchingFinished{ false };
    juce::AudioBuffer<float> stretchedBuffer;
    int readPosition = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stem)
};
