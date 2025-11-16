#pragma once
#include "IEffect.h"

class Echo : public IEffect {
public:
    Echo();
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);

private:
    float echoGain_; // Default gain
    int delaySamples_; // Default delay in samples
    int numEchoes_; // Default number of echoes
    float decayFactor_; // Default decay factor
};