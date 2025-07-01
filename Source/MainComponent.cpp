#include "MainComponent.h"

MainComponent::MainComponent()
{
    // 1. Start the audio engine
    audioEngine.initialise();

    // 2. Create SessionManager with root folder of songs
    sessionManager = std::make_unique<SessionManager>(
        audioEngine,
        juce::File("C:/Personal Computing Stuff/Python/Song splitter/DJ SONGS + STEMS")
    );

    // 3. Connect UI to session manager
    mainUI = std::make_unique<MainUI>(*sessionManager);
    addAndMakeVisible(*mainUI);

    // 4. UI size
    setSize(1500, 700);

    DBG("MainComponent initialized with AudioEngine, SessionManager, and MainUI");
}

MainComponent::~MainComponent()
{
    // Shutdown on destruction
    audioEngine.stopAll();
    audioEngine.shutdown();
}

void MainComponent::paint(juce::Graphics& g)
{
    // Optional UI background (uncomment if needed)
    // g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized()
{
    if (mainUI)
        mainUI->setBounds(getLocalBounds());
}
