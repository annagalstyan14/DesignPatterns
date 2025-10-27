#pragma once
#include "IEffect.h"
#include <vector>

class Reverb : public IEffect {
public:
    Reverb(float reverbTimeMs, float diffusionPercent, float decayPercent,
           float dryPercent, float wetPercent, float brightnessPercent,
           float sampleRate = 44100.0f);

    void apply(float* audioBuffer, size_t bufferSize) override;
    size_t maxDelaySamples() const { return static_cast<size_t>(reverbTimeMs * sampleRate / 1000.0f); }

private:
    struct CombFilter {
        std::vector<float> buffer;
        size_t index = 0;
        size_t delaySamples = 0;
        float feedback = 0.0f;
        float damping = 0.0f;
        float lowpassState = 0.0f;
    };

    struct AllPassFilter {
        std::vector<float> buffer;
        size_t index = 0;
        size_t delaySamples = 0;
        float feedback = 0.0f;
    };

    float reverbTimeMs;
    float diffusion;
    float decay;
    float dryOut;
    float wetOut;
    float brightness;
    float sampleRate;

    std::vector<CombFilter> combFilters;
    std::vector<AllPassFilter> allPassFilters;
};
