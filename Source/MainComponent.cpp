#include "MainComponent.h"

MainComponent::MainComponent()
{
    // 1. Initialize AudioEngine (register formats, open device, etc)
    audioEngine.initialise();

    addTestSong();
    audioEngine.start();

    setSize(600, 400);
}

MainComponent::~MainComponent()
{
    // Make sure to shutdown audio on destruction
    audioEngine.stop();
    audioEngine.shutdown();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("AudioEngine Playback Running...", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // layout any child components if you add UI controls
}


void MainComponent::addTestSong()
{

    // 2. Create your Song and add to SongMixer
    auto songFile = juce::File("C:/FL STUDIO/FLsamples/Dr Dre & Snoop Dogg - Still D.R.E. (Acapella).mp3");
    auto song = std::make_unique<Song>(songFile);
    if (songFile.existsAsFile())
    {
        auto song = std::make_unique<Song>(songFile);

        // Get sample rate & buffer size from AudioEngine’s device manager
        auto* device = audioEngine.getDeviceManager().getCurrentAudioDevice();
        if (device != nullptr)
        {
            double sampleRate = device->getCurrentSampleRate();
            int bufferSize = device->getCurrentBufferSizeSamples();

            song->prepareToPlay(bufferSize, sampleRate);
        }

        song->start();

        // Add the song to the engine's mixer
        audioEngine.addSong(std::move(song));

		DBG("Song added: " << songFile.getFullPathName());
    }

    else
    {
        DBG("Song file does not exist: " << songFile.getFullPathName());
    }
}