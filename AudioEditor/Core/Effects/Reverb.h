#pragma once
#include "IEffect.h"
#include <vector>

class Reverb : public IEffect {
public:
    Reverb();
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);

private:

    struct DelayLine{

        std::vector<float> audioBuffer;
        size_t index = 0;
    };

    std::vector<DelayLine> delayLines;
    float reverbGain;
    float decayFactor;
    float lowPass;
    int baseDelaySamples;



};