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
                0,      // no read ahead buffer
                nullptr,
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


void Stem::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (readPosition >= stretchedBuffer.getNumSamples())
    {
        info.clearActiveBufferRegion();
        return;
    }

    auto* outBuffer   = info.buffer;
    const int numCh   = outBuffer->getNumChannels();
    const int blockSz = info.numSamples;
    const int start   = info.startSample;

    // how many valid samples remain in our stretched buffer
    int toCopy = juce::jmin(blockSz,
                            stretchedBuffer.getNumSamples() - readPosition);

    for (int ch = 0; ch < numCh; ++ch)
    {
        outBuffer->copyFrom(ch, start,
                            stretchedBuffer, ch,
                            readPosition,
                            toCopy);

        // clear any “leftover” if we’re at the end
        if (toCopy < blockSz)
            outBuffer->clear(ch,
                             start + toCopy,
                             blockSz - toCopy);
    }

    // <-- this line was wrong before!
    readPosition += toCopy;
}


void Stem::releaseResources()
{
    transportSource.releaseResources();
    stretchedBuffer.setSize(0, 0);
}


void Stem::preStretch(double tempoRatio)
{
    auto* reader = readerSource->getAudioFormatReader();
    const int  channels = int(reader->numChannels);
    const int  totalSamples = int(reader->lengthInSamples);
    const double sr = 44100;

    // 1) Read the whole file into 'input'
    juce::AudioBuffer<float> input(channels, totalSamples);
    reader->read(&input, 0, totalSamples, 0, true, true);

    // 2) Set up RubberBand with the options you want
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

    RB stretcher(sr, channels, opts);

    // 3) Tell it how much to stretch by
    //    (e.g. originalTempo/targetTempo, which you’ve inverted as 1/tempoRatio)
    stretcher.setTimeRatio(1.0 / tempoRatio);
    stretcher.setPitchScale(1.0);
    DBG("Stretcher set with Time Ratio " << (1.0 / tempoRatio));
		

    // 4) Process *all* the samples in one go, marking it final
    std::vector<float*> inPtrs(channels);
    for (int ch = 0; ch < channels; ++ch)
        inPtrs[ch] = const_cast<float*>(input.getReadPointer(ch));
    stretcher.process(inPtrs.data(), size_t(totalSamples), /*isFinal=*/true);

    // 5) How many output samples are ready?
    int outSamples = int(stretcher.available());

    // 6) Allocate the exact buffer size and set up pointers
    stretchedBuffer.setSize(channels, outSamples);
    std::vector<float*> outPtrs(channels);
    for (int ch = 0; ch < channels; ++ch)
        outPtrs[ch] = stretchedBuffer.getWritePointer(ch);

    // 7) Retrieve *all* the stretched data in one call
    stretcher.retrieve(outPtrs.data(), size_t(outSamples));

    // 8) Reset your read head for playback
    readPosition = 0;

    DBG("Stem pre-stretched: " << outSamples << " samples at " << sr << " Hz");
}
