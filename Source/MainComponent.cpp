#include "MainComponent.h"

MainComponent::MainComponent()
{
    // Initialize AudioEngine (register formats, open device, etc)
    audioEngine.initialise();
    //audioEngine.startAll();


    mainController = std::make_unique<MainController>(juce::File("C:/Personal Computing Stuff/Python/Song splitter/DJ SONGS + STEMS"), audioEngine);

    mainUI = std::make_unique<MainUI>(*mainController);

    addAndMakeVisible(*mainUI);

    setSize(1500, 800);

	DBG("MainComponent initialized with AudioEngine, MainController and UI");

}

MainComponent::~MainComponent()
{
    // Make sure to shutdown audio on destruction
    audioEngine.stopAll();
    audioEngine.shutdown();
}

void MainComponent::paint(juce::Graphics& g)
{
    /*g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("AudioEngine Playback Running...", getLocalBounds(), juce::Justification::centred, true);*/
}

void MainComponent::resized()
{
    mainUI->setBounds(getLocalBounds());
}
