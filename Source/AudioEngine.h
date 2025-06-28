#pragma once
#include <JuceHeader.h>

class AudioEngine
{
public:
    AudioEngine() = default;
    ~AudioEngine() = default;

    /** Call once to set up device and start playing */
    void init();

    /** Call once to tear down and stop playing */
    void shutdown();

private:
    juce::AudioDeviceManager            deviceManager;
    juce::AudioFormatManager            formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource          transportSource;
    juce::AudioSourcePlayer             sourcePlayer;
};