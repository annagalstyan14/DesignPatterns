#pragma once
#include "IEffect.h"

class Reverb : public IEffect {
public:
    Reverb();
    void apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);

private:
    float reverbGain_ = 0.5f;
    int delaySamples_ = 3308;
};