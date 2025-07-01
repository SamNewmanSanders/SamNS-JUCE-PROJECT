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

    // Map stem file names to their corresponding StemType
    static const std::map<juce::String, StemType> stemMap = {
        { "drums", StemType::Drums },
        { "vocals", StemType::Vocals },
        { "bass", StemType::Bass },
        { "other", StemType::Other }
    };

    for (const auto& [stemName, stemType] : stemMap)
    {
        juce::File stemFile = folder.getChildFile(stemName + ".mp3");

        if (stemFile.existsAsFile())
        {
            addStem(stemFile, stemType);
        }
        else
        {
            DBG("Missing stem: " + stemFile.getFullPathName());
        }
    }

    return true;
}

void Song::addStem(const juce::File& file, StemType stemType)
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

void Song::setStemMute(StemType stemType, bool mute)
{
	stemMixer.setStemMute(stemType, mute);
}
