#ifndef SPEED_CHANGE_EFFECT_H
#define SPEED_CHANGE_EFFECT_H

#include "IEffect.h"
#include <vector>

class SpeedChangeEffect : public IEffect {
public:
    SpeedChangeEffect(float speedFactor, float sampleRate = 44100.0f);
    void apply(float* audioBuffer, size_t bufferSize) override;
    const std::vector<float>& getResampledBuffer() const;
    size_t getResampledSize() const;
private:
    float speedFactor;
    float sampleRate;
    std::vector<float> resampledBuffer;
};

#endif // SPEED_CHANGE_EFFECT_H