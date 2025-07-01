#pragma once

#include <JuceHeader.h>
#include "../Helpers/StemType.h"

class Stem : public juce::AudioSource
{
public:
    Stem(const juce::File& audioFile, StemType stemType);
    ~Stem();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }

    void setMuted(bool shouldMute) { muted = shouldMute;  }
    bool isMuted() const { return muted; }

    StemType getStemType() const { return stemType; } // getter for stem type

private:
    StemType stemType;  // Store the stem type ("vocals", "drums", etc.)
    bool muted = false;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
};
