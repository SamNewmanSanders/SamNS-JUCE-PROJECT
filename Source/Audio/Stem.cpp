#include "Stem.h"

Stem::Stem(const juce::File& audioFile, StemType stemType)
    : stemType(stemType)
{
    formatManager.registerBasicFormats();

    if (audioFile.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(audioFile))
        {
            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(),
                0,      // no read ahead buffer (default)
                nullptr, // no thread pool for background reading
                reader->sampleRate);
        }
    }
}

Stem::~Stem()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void Stem::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Stem::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (transportSource.isPlaying())
        transportSource.getNextAudioBlock(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();  // silence when stopped
}

void Stem::releaseResources()
{
    transportSource.releaseResources();
}

