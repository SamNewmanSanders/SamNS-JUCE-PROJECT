#pragma once

#include <JuceHeader.h>
#include "../Stem.h"

class Stem; // forward declaration

// A ThreadPoolJob that calls Stem::performStretch on a background thread
class StretchJob : public juce::ThreadPoolJob
{
public:
    /**
     Constructs a stretch job for the given Stem.
     @param stemToStretch   Pointer to the Stem instance to process
     @param tempoRatio      Time-stretch ratio (e.g., 1.2 for 20% slower)
     @param sampleRate      Sample rate for stretching
    */
    StretchJob(Stem* stemToStretch, double tempoRatio, double sampleRate);

    /**
     Runs the stretching job on a worker thread.
     Must return jobHasFinished when done.
    */
    JobStatus runJob() override;

private:
    Stem* stem;   // Non-owning pointer to the Stem
    double ratio;  // Time-stretch ratio
    double sr;     // Sample rate
};