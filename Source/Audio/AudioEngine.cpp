#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    // Nothing heavy in constructor
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

void AudioEngine::initialise()
{
    // Initialise device manager for output only: 0 inputs, 2 outputs (stereo)
    deviceManager.initialise(0, 2, nullptr, true);

    // Connect the SongMixer as the source for the AudioSourcePlayer
    sourcePlayer.setSource(&songMixer);

    // Connect the AudioSourcePlayer to the device
    deviceManager.addAudioCallback(&sourcePlayer);

    // Prepare songMixer with default buffer size and sample rate
    auto* currentDevice = deviceManager.getCurrentAudioDevice();
    if (currentDevice != nullptr)
    {
        songMixer.prepareToPlay(currentDevice->getCurrentBufferSizeSamples(),
            currentDevice->getCurrentSampleRate());
    }
}

void AudioEngine::shutdown()
{
    // Disconnect audio chain
    deviceManager.removeAudioCallback(&sourcePlayer);
    sourcePlayer.setSource(nullptr);

    // Release resources in mixer
    songMixer.releaseResources();

    // Shut down device manager
    deviceManager.closeAudioDevice();
}

void AudioEngine::addSong(std::shared_ptr<Song> song)
{
    const juce::ScopedLock sl(deviceManager.getAudioCallbackLock());

    // 1) Prepare the song now that we know the device settings:
    if (auto* dev = deviceManager.getCurrentAudioDevice())
        song->prepareToPlay(dev->getCurrentBufferSizeSamples(),
            dev->getCurrentSampleRate());

    // 2) Add it to the mixer
    songMixer.addSong(std::move(song));
    DBG("AudioEngine added song to mixer");

    // 3) Start playback
    //start();
}

void AudioEngine::startAll()
{
    const juce::ScopedLock sl(deviceManager.getAudioCallbackLock()); // Corrected method name  
    songMixer.startAll();
}

void AudioEngine::stopAll()
{
    const juce::ScopedLock sl(deviceManager.getAudioCallbackLock()); // Corrected method name  
    songMixer.stopAll();
}
