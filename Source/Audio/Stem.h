#pragma once

#include <JuceHeader.h>

class Stem : public juce::AudioSource
{
public:
    Stem(const juce::File& audioFile, const juce::String& stemType);
    ~Stem();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }

    juce::String getStemType() const { return stemType; } // getter for stem type

private:
    juce::String stemType;  // Store the stem type ("vocals", "drums", etc.)

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
};
