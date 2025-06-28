#pragma once

#include <JuceHeader.h>

class Song : public juce::AudioSource
{
public:
	Song(const juce::File& audioFile);
	~Song();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void start() { transportSource.start(); }
	void stop() { transportSource.stop(); }

private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;

};