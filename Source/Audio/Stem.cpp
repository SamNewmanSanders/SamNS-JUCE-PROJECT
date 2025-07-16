#include "Stem.h"

Stem::Stem(const juce::File& audioFile, StemType stemType)
    : stemType(stemType)
{
    formatManager.registerBasicFormats();
    if (audioFile.existsAsFile())
        reader.reset(formatManager.createReaderFor(audioFile));
}

Stem::~Stem()
{
    reader.reset();
}

void Stem::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
}


void Stem::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (paused||!stretchingFinished)
    {
        info.clearActiveBufferRegion();
        return;
    }

    auto* buffer = info.buffer;
    int channels = buffer->getNumChannels();
    int blockSz = info.numSamples;
    int start = info.startSample;

    int available = stretchedBuffer.getNumSamples() - readPosition;
    int toCopy = juce::jmin(blockSz, available);

    for (int ch = 0; ch < channels; ++ch)
    {
        if (ch < stretchedBuffer.getNumChannels())
            buffer->copyFrom(ch, start, stretchedBuffer, ch, readPosition, toCopy);
        if (toCopy < blockSz)
            buffer->clear(ch, start + toCopy, blockSz - toCopy);
    }
    readPosition += toCopy;
}



void Stem::releaseResources()
{
    //transportSource.releaseResources();
}


void Stem::startStretching(juce::ThreadPool& pool, double tempoRatio, double sampleRate)
{
    if (stretchingFinished)
        return;

    pool.addJob(new StretchJob(this, tempoRatio, sampleRate), true);

    DBG("Starting stem stretch job at tempoRatio " << tempoRatio);
}


void Stem::performStretch(double tempoRatio, double sampleRate)
{
    if (!reader) { DBG("Reader is null"); return; }
    const int channels = int(reader->numChannels);
    const int totalSamples = int(reader->lengthInSamples);

    // 1) Read entire input
    juce::AudioBuffer<float> inputBuffer(channels, totalSamples);
    reader->read(&inputBuffer, 0, totalSamples, 0, true, true);
    std::vector<float*> inPtrs(channels);
    for (int ch = 0; ch < channels; ++ch)
        inPtrs[ch] = inputBuffer.getWritePointer(ch);

    // 2) Configure stretcher
    using RB = RubberBand::RubberBandStretcher;
    

    int opts = RB::OptionProcessOffline;

    // Use best rubberband settings for each stem type

    switch (stemType)
    {
    case StemType::Vocals:
        opts |= RB::OptionWindowLong
            | RB::OptionFormantPreserved
            | RB::OptionPitchHighQuality;
        break;

    case StemType::Drums:
        opts |= RB::OptionWindowShort
            | RB::OptionTransientsCrisp;
        break;

    case StemType::Bass:
        opts |= RB::OptionWindowLong
            | RB::OptionPitchHighQuality;
        break;

    case StemType::Other:
    default:
        opts |= RB::OptionWindowLong
            | RB::OptionPitchHighQuality;

        break;
    }


    RB stretcher(sampleRate, channels, opts);
    double tb = 1.0 / tempoRatio;  // if tempoRatio was “BPM out / BPM in”
    stretcher.setTimeRatio(tb);
    DBG("Configured stretcher with timeRatio=" << stretcher.getTimeRatio());

    // 3) Tell it upfront how many input frames
    stretcher.setExpectedInputDuration(totalSamples);

    // 4) Two‑pass offline: study then process
    stretcher.study(inPtrs.data(), size_t(totalSamples), true);
    stretcher.process(inPtrs.data(), size_t(totalSamples), true);

    // 5) Grab the stretched output
    int needed = int(stretcher.available());
    DBG("RubberBand produced " << needed << " frames (expected "
        << int(totalSamples * tb) << ")");
    stretchedBuffer.setSize(channels, needed, false, false, false);
    std::vector<float*> outPtrs(channels);
    for (int ch = 0; ch < channels; ++ch)
        outPtrs[ch] = stretchedBuffer.getWritePointer(ch);
    stretcher.retrieve(outPtrs.data(), size_t(needed));

    // 6) Finish
    readPosition = 0;
    stretchingFinished = true;
    paused = false;
}