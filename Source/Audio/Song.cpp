#include "Song.h"

Song::Song(const juce::File& folder, juce::String songName)
	:songName(songName)
{

    // Setup SoundTouch params (sample rate and channels will be updated in prepareToPlay)
    soundTouch.setChannels(2);  // Assuming stereo
    soundTouch.setSampleRate(44100); // default; update later
    soundTouch.setTempo(currentTempoRatio);

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

    // Update SoundTouch sample rate
    soundTouch.setSampleRate(static_cast<uint>(sampleRate));
    soundTouch.setChannels(2); // stereo
    soundTouch.setTempo(currentTempoRatio);

    tempBuffer.setSize(2, samplesPerBlockExpected); // stereo buffer for stems mixed output
}

void Song::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Step 4a: Mix all stems into tempBuffer
    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);
    tempBuffer.clear();
    stemMixer.getNextAudioBlock(tempInfo);

    // Step 4b: Feed mixed data to SoundTouch
    // SoundTouch expects interleaved samples, so we need to interleave the buffer
    const int numSamples = tempBuffer.getNumSamples();
    const int numChannels = tempBuffer.getNumChannels();

    // Prepare interleaved buffer
    std::vector<float> interleavedBuffer(numSamples * numChannels);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            interleavedBuffer[sample * numChannels + channel] = tempBuffer.getSample(channel, sample);
        }
    }

    // Feed samples into SoundTouch
    soundTouch.putSamples(interleavedBuffer.data(), numSamples);

    // Receive processed samples
    const int maxOutputSamples = bufferToFill.numSamples;
    std::vector<float> processedInterleaved(maxOutputSamples * numChannels);
    int receivedSamples = soundTouch.receiveSamples(processedInterleaved.data(), maxOutputSamples);

    // Copy processed data to bufferToFill
    bufferToFill.buffer->clear(bufferToFill.startSample, bufferToFill.numSamples);

    for (int sample = 0; sample < receivedSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float val = processedInterleaved[sample * numChannels + channel];
            bufferToFill.buffer->addSample(channel, bufferToFill.startSample + sample, val);
        }
    }
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
    soundTouch.setTempo(currentTempoRatio);
}