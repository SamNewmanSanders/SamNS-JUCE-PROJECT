#include "Song.h"

Song::Song(const juce::File& folder, juce::String songName, int selectedtempooncreation)
    : songName(songName), selectedTempoOnCreation(selectedtempooncreation)
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

    // 2) Locate beatgrid.json next to those files
    auto jsonFile = folder.getChildFile("beatgrid.json");
    if (jsonFile.existsAsFile())
    {
        double fileBpm = getBpmFromJsonFile(jsonFile);
        if (fileBpm > 0.0)
        {
            originalTempo = fileBpm;
        }
        else
        {
            DBG("beatgrid.json present but BPM was invalid");
        }
    }
    else
    {
        DBG("No beatgrid.json found in " << folder.getFullPathName());
    }

    // 3) Compute your stretch ratio relative to the bpm at creation
    //    e.g. if user wants 128 BPM but fileBpm was 140 BPM:
    //    currentTempoRatio = fileBpm / userDesiredBpm;
    if (selectedTempoOnCreation > 0)
		currentTempoRatio = 0.91875f * (double(selectedTempoOnCreation) / originalTempo);

    DBG("Selected Tempo On Creation:" << selectedTempoOnCreation
        << ", Original Tempo: " << originalTempo
        << ", Current Tempo Ratio: " << currentTempoRatio);

    return true;
}

void Song::addStem(const juce::File& file, StemType stemType)
{
    auto stem = std::make_unique<Stem>(file, stemType);
    stemMixer.addStem(std::move(stem));
}

void Song::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;

    // 1) Create thread pool once (use e.g. 2 threads per core)
    if (!pool)
        pool = std::make_unique<juce::ThreadPool>(4);

    // 2) Let mixer / stems prepare their audio callbacks
    stemMixer.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // 3) Kick off asynchronous stretching of each stem
    for (auto& stem : stemMixer.getStems())
        stem->startStretching(*pool, currentTempoRatio, currentSampleRate);
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

void Song::setTempoOnCreation(int tempo)
{
	selectedTempoOnCreation = tempo;
}

double Song::getBpmFromJsonFile(const juce::File& jsonFile)
{
    // 1) Load the entire file into a String
    juce::String jsonText = jsonFile.loadFileAsString();
    if (jsonText.isEmpty())
    {
        DBG("Failed to load JSON file or file was empty: " << jsonFile.getFullPathName());
        return 0.0;
    }

    // 2) Parse it into a var
    juce::var topLevel = juce::JSON::parse(jsonText);
    if (!topLevel.isObject())
    {
        DBG("JSON parse error or top‑level not an object");
        return 0.0;
    }

    // 3) Grab the DynamicObject and pull out "bpm"
    if (auto* obj = topLevel.getDynamicObject())
    {
        juce::var bpmVar = obj->getProperty("bpm");
        if (bpmVar.isDouble() || bpmVar.isInt())
            return static_cast<double> (bpmVar);
        else
            DBG("JSON field “bpm” is not numeric");
    }
    return 0.0;
}