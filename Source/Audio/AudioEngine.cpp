#include "AudioEngine.h"
#include "../Helpers/StemType.h"


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

    auto* deviceType = deviceManager.getCurrentDeviceTypeObject();
    if (deviceType != nullptr)
    {
        auto names = deviceType->getDeviceNames();
        for (const auto& name : names)
            DBG("Available device: " + name);
    }

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

}

void AudioEngine::removeSong(std::shared_ptr<Song> song)
{
	const juce::ScopedLock sl(deviceManager.getAudioCallbackLock());
	songMixer.stopSong(song); // Stop the song before removing it
	songMixer.removeSong(song);
}

void AudioEngine::startSong(std::shared_ptr<Song> song)
{
	const juce::ScopedLock sl(deviceManager.getAudioCallbackLock()); 
	songMixer.startSong(song);
}

void AudioEngine::stopSong(std::shared_ptr<Song> song)
{
	const juce::ScopedLock sl(deviceManager.getAudioCallbackLock());     
	songMixer.stopSong(song);
}

void AudioEngine::startAll()
{
    const juce::ScopedLock sl(deviceManager.getAudioCallbackLock()); 
    songMixer.startAll();
}

void AudioEngine::stopAll()
{
    const juce::ScopedLock sl(deviceManager.getAudioCallbackLock()); 
    songMixer.stopAll();
}

void AudioEngine::setStemMute(juce::String songId, StemType stemType, bool mute)
{
	const juce::ScopedLock sl(deviceManager.getAudioCallbackLock());  
	songMixer.setStemMute(songId, stemType, mute);
}

void AudioEngine::setStemVolume(juce::String songId, StemType stemType, float newVolume)
{
	const juce::ScopedLock sl(deviceManager.getAudioCallbackLock());
	songMixer.setStemVolume(songId, stemType, newVolume);
}


void AudioEngine::setDeviceBufferSize (int newBufferSize, double newSampleRate)
{
    // grab the current setup
    juce::AudioDeviceManager::AudioDeviceSetup setup;
    deviceManager.getAudioDeviceSetup (setup);

    // tweak your size & rate
    setup.bufferSize   = newBufferSize;    // e.g. 1024, 2048, etc.
    setup.sampleRate   = newSampleRate;    // e.g. 44100.0, 48000.0

    // apply immediately (true = restart device if already running)
    deviceManager.setAudioDeviceSetup (setup, true);

    // now re-prepare your mixer with the new blockSize/sampleRate
    if (auto* dev = deviceManager.getCurrentAudioDevice())
        songMixer.prepareToPlay (dev->getCurrentBufferSizeSamples(),
                                 dev->getCurrentSampleRate());
}