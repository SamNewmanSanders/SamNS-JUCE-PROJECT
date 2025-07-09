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
    currentSampleRate = sampleRate;

    // Create a new RubberBandStretcher with higher-quality real-time options
    auto options =
        RubberBand::RubberBandStretcher::OptionProcessRealTime |
        RubberBand::RubberBandStretcher::OptionWindowLong |
        RubberBand::RubberBandStretcher::OptionFormantPreserved |
        RubberBand::RubberBandStretcher::OptionPhaseLaminar |
        RubberBand::RubberBandStretcher::OptionPitchHighQuality |
        RubberBand::RubberBandStretcher::OptionThreadingAuto;

    stretcher = std::make_unique<RubberBand::RubberBandStretcher>(
        sampleRate,
        2, // stereo
        options
    );

    stretcher->setTimeRatio(currentTempoRatio);

    tempBuffer.setSize(2, samplesPerBlockExpected); // stereo buffer
}

void Song::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    // Step 1: Mix all stems into tempBuffer
    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);
    tempBuffer.clear();
    stemMixer.getNextAudioBlock(tempInfo);

    const int numSamples = tempBuffer.getNumSamples();
    const int numChannels = tempBuffer.getNumChannels();

    if (numSamples == 0 || !stretcher)
        return;

    // Step 2: Create float** for Rubber Band input
    std::vector<std::vector<float>> channelData(numChannels, std::vector<float>(numSamples));
    std::vector<float*> inputPtrs(numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        std::memcpy(channelData[ch].data(),
            tempBuffer.getReadPointer(ch),
            sizeof(float) * numSamples);
        inputPtrs[ch] = channelData[ch].data();
    }

    // Step 3: Feed to Rubber Band
    stretcher->process(inputPtrs.data(), numSamples, false);

    // Step 4: Retrieve processed samples
    const int available = stretcher->available();
    if (available <= 0)
        return;

    const int maxSamples = std::min(bufferToFill.numSamples, available);

    juce::AudioBuffer<float>& outBuffer = *bufferToFill.buffer;

    // Step 5: Prepare output channel pointers
    std::vector<float*> outputPtrs(numChannels);
    for (int ch = 0; ch < numChannels; ++ch)
        outputPtrs[ch] = outBuffer.getWritePointer(ch, bufferToFill.startSample);

    // Step 6: Retrieve into output buffer
    const size_t samplesRetrieved = stretcher->retrieve(outputPtrs.data(), maxSamples);

    // Step 7 (optional): clear rest if retrieved fewer than expected
    if (samplesRetrieved < static_cast<size_t>(bufferToFill.numSamples))
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            outBuffer.clear(ch,
                bufferToFill.startSample + static_cast<int>(samplesRetrieved),
                bufferToFill.numSamples - static_cast<int>(samplesRetrieved));
        }
    }
}


void Song::releaseResources()
{
    stemMixer.releaseResources();
    stretcher.reset();
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
    if (stretcher)
        stretcher->setTimeRatio(currentTempoRatio);
}
