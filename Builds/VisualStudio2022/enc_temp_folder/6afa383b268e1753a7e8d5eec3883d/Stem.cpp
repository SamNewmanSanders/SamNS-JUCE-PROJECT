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
    // Play from stretched buffer
    if (readPosition >= stretchedBuffer.getNumSamples())
    {
        info.clearActiveBufferRegion();
        return;
    }

    auto* outBuffer = info.buffer;
    for (int ch = 0; ch < outBuffer->getNumChannels(); ++ch)
    {
        int toCopy = juce::jmin(info.numSamples,
            stretchedBuffer.getNumSamples() - readPosition);
        outBuffer->copyFrom(ch, info.startSample,
            stretchedBuffer, ch, readPosition, toCopy);
        if (toCopy < info.numSamples)
            outBuffer->clear(ch, info.startSample + toCopy,
                info.numSamples - toCopy);
    }

    readPosition += info.numSamples;
}


void Stem::releaseResources()
{
    transportSource.releaseResources();
    stretchedBuffer.setSize(0, 0);
}



void Stem::preStretch(double tempoRatio)
{
    auto* reader = readerSource->getAudioFormatReader();
    int channels = (int)reader->numChannels;
    int totalSamples = (int)reader->lengthInSamples;
    double sr = reader->sampleRate;

    // Read entire stem into buffer
    juce::AudioBuffer<float> input(channels, totalSamples);
    reader->read(&input, 0, totalSamples, 0, true, true);

    // Build the stretcher with formant preservation and high-quality pitch
    using Opt = RubberBand::RubberBandStretcher;
    int options = Opt::OptionProcessOffline
        | Opt::OptionEngineFaster
        | Opt::OptionWindowLong
        | Opt::OptionFormantPreserved
        | Opt::OptionPitchHighQuality;

    RubberBand::RubberBandStretcher stretcher(sr, channels, options);

    // Set time ratio (inverse of tempo ratio) and neutral pitch scale
    double timeRatio = 1.0 / tempoRatio;
    stretcher.setTimeRatio(timeRatio);
    stretcher.setPitchScale(1.0);

    // Prepare output container; we'll size it later
    stretchedBuffer.clear();
    readPosition = 0;

    // --- Chunked processing for offline mode ---
    const size_t total = static_cast<size_t>(totalSamples);
    const size_t chunkSize = 32768; // e.g., 32k samples
    size_t pos = 0;

    // Feed input in chunks
    while (pos < total)
    {
        size_t len = std::min(chunkSize, total - pos);
        std::vector<const float*> inPtrs(channels);
        for (int ch = 0; ch < channels; ++ch)
            inPtrs[ch] = input.getReadPointer(ch, static_cast<int>(pos));

        bool isFinal = (pos + len == total);
        stretcher.process(inPtrs.data(), len, isFinal);
        pos += len;
    }

    // Retrieve all processed output
    int available = static_cast<int>(stretcher.available());
    DBG("requested tempoRatio: " << tempoRatio
        << ", timeRatio used: " << timeRatio
        << ", output ratio: " << (double)available / totalSamples
        << " (" << available << "/" << totalSamples << ")");

    stretchedBuffer.setSize(channels, available);
    std::vector<float*> outPtrs(channels);
    for (int ch = 0; ch < channels; ++ch)
        outPtrs[ch] = stretchedBuffer.getWritePointer(ch);

    int fetched = 0;
    int writePos = 0;
    while ((fetched = static_cast<int>(stretcher.retrieve(outPtrs.data(), available))) > 0)
    {
        for (int ch = 0; ch < channels; ++ch)
            outPtrs[ch] += fetched;
        writePos += fetched;
        available = static_cast<int>(stretcher.available());
    }

    // Final diagnostic
    DBG("Stem pre‑stretched: " << writePos << " samples at " << sr << " Hz");
}