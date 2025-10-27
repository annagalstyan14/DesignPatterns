#pragma once
#include "IEffect.h"

class Echo : public IEffect {
public:
    Echo();
    void apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent); // New method for user input

private:
    float echoGain_ = 0.1f; // Default gain
    int delaySamples_ = 6615; // Default delay in samples
    int numEchoes_ = 1; // Default number of echoes
    float decayFactor_ = 0.25f; // Default decay factor
};