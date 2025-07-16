//=== StretchJob.cpp ===
#include "StretchJob.h"

StretchJob::StretchJob(Stem* stemToStretch,
    double tempoRatio,
    double sampleRate)
    : ThreadPoolJob("Stem Stretch Job"),
    stem(stemToStretch),
    ratio(tempoRatio),
    sr(sampleRate)
{
}

juce::ThreadPoolJob::JobStatus StretchJob::runJob()
{
    if (stem)
        stem->performStretch(ratio, sr);

    return jobHasFinished;
}
