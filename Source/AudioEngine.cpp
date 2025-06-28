#include "AudioEngine.h"

void AudioEngine::init()
{
    // 1) Make the format manager know WAV/MP3/AIFF:
    formatManager.registerBasicFormats();

    // 2) Grab file
    auto file = juce::File("C:/FL STUDIO/FLsamples/Dr Dre & Snoop Dogg - Still D.R.E. (Acapella).mp3");

    if (file.existsAsFile())
    {
        // 3) Create a reader and wrap it in a reader source:
        if (auto* reader = formatManager.createReaderFor(file))
        {
            readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(),
                0,           // no background thread
                nullptr,     // no thread pool
                reader->sampleRate);
            transportSource.start();
        }
    }

    // 4) Hook the transport into our AudioSourcePlayer:
    sourcePlayer.setSource(&transportSource);

    // 5) Initialize the audio device and register the player:
    deviceManager.initialise(0, 2, nullptr, true);       // 0 in, 2 out
    deviceManager.addAudioCallback(&sourcePlayer);       // <<<<< no override headaches
}

void AudioEngine::shutdown()
{
    // 1) Stop callbacks & clear the player source:
    deviceManager.removeAudioCallback(&sourcePlayer);
    sourcePlayer.setSource(nullptr);

    // 2) Stop transport, free resources:
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}
