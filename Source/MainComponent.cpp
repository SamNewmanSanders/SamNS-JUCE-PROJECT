// MainComponent.cpp

#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400, 200);
    engine.init();
}

MainComponent::~MainComponent()
{
    engine.shutdown();
}
