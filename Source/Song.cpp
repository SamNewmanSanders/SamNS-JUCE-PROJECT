#include "Song.h"

Song::Song(const juce::File& audioFile)
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

Song::~Song()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}


void Song::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Song::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (transportSource.isPlaying())
        transportSource.getNextAudioBlock(bufferToFill);
    else
        bufferToFill.clearActiveBufferRegion();  // silence when stopped
}

void Song::releaseResources()
{
    transportSource.releaseResources();
}


