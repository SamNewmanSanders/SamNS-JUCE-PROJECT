#include "Song.h"

Song::Song(const juce::File& folder, juce::String songName)
	:songName(songName)
{
	// Call the loadFromFolder method to load stems
    if (!loadFromFolder(folder))
    {
        DBG("Failed to load stems from folder: " << folder.getFullPathName());
    }
}

Song::~Song()
{
    releaseResources();
}

bool Song::loadFromFolder(const juce::File& folder)
{
    if (!folder.isDirectory())
        return false;

    static const juce::StringArray stemTypes = { "drums", "vocals", "bass", "other" };

    for (const auto& stem : stemTypes)
    {
        juce::File stemFile = folder.getChildFile(stem + ".mp3");

        if (stemFile.existsAsFile())
        {
            addStem(stemFile, stem);
        }
        else
        {
            DBG("Missing stem: " + stemFile.getFullPathName());
        }
    }

    return true;
}

void Song::addStem(const juce::File& file, const juce::String& stemType)
{
    auto stem = std::make_unique<Stem>(file, stemType);
    stemMixer.addStem(std::move(stem));
}

void Song::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    stemMixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Song::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    stemMixer.getNextAudioBlock(bufferToFill);
}

void Song::releaseResources()
{
    stemMixer.releaseResources();
}

void Song::start()
{
    stemMixer.startAll();
}

void Song::stop()
{
    stemMixer.stopAll();
}
