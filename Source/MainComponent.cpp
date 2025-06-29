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
    auto songFolder = juce::File("C:/Personal Computing Stuff/Python/Song splitter/DJ SONGS + STEMS/100%");

    if (songFolder.exists() && songFolder.isDirectory())
    {
        auto song = std::make_unique<Song>(songFolder); // constructor does loading

        auto* device = audioEngine.getDeviceManager().getCurrentAudioDevice();
        if (device != nullptr)
        {
            double sampleRate = device->getCurrentSampleRate();
            int bufferSize = device->getCurrentBufferSizeSamples();

            song->prepareToPlay(bufferSize, sampleRate);
        }

        song->start();

        audioEngine.addSong(std::move(song));

        DBG("Song added: " << songFolder.getFullPathName());
    }
    else
    {
        DBG("Song folder does not exist or is not a directory: " << songFolder.getFullPathName());
    }
}
