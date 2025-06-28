// main.cpp

#include <JuceHeader.h>
#include "MainComponent.h"

class ProofApp : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "AudioEngine POC"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow("AudioEngine POC", new MainComponent(), *this));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name, juce::Component* c, JUCEApplication& a)
            : DocumentWindow(name,
                juce::Colours::darkgrey,
                DocumentWindow::allButtons),
            app(a)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(c, true);
            setResizable(true, true);
            centreWithSize(400, 200);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ProofApp)