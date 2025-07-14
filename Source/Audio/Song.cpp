#include "Song.h"

Song::Song(const juce::File& folder, juce::String songName)
    : songName(songName)
{
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

    static const std::map<juce::String, StemType> stemMap = {
        {"drums", StemType::Drums},
        {"vocals", StemType::Vocals},
        {"bass",  StemType::Bass},
        {"other", StemType::Other}
    };

    for (auto& [name, type] : stemMap)
    {
        auto file = folder.getChildFile(name + ".mp3");
        if (file.existsAsFile())
            addStem(file, type);
        else
            DBG("Missing stem: " << file.getFullPathName());
    }

	DBG("Current tempo ratio: " << currentTempoRatio);

    // Synchronously pre-stretch each stem
    for (auto& sPtr : stemMixer.stems) // assume stems accessible in mixer
        sPtr->preStretch(currentTempoRatio);

    return true;
}

void Song::addStem(const juce::File& file, StemType stemType)
{
    auto stem = std::make_unique<Stem>(file, stemType);
    stemMixer.addStem(std::move(stem));
}

void Song::prepareToPlay(int samplesPerBlockExpected, double sr)
{
    currentSampleRate = sr;
    stemMixer.prepareToPlay(samplesPerBlockExpected, currentSampleRate);
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

void Song::setStemVolume(StemType stemType, float newVolume)
{
    stemMixer.setStemVolume(stemType, newVolume);
}

void Song::setTempoRatio(float newRatio)
{
    currentTempoRatio = newRatio;
}