#pragma once
#include "IEffect.h"

class VolumeEffect : public IEffect {
public:
    explicit VolumeEffect(float gain = 1.0f);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setGain(float gain);  // 0.0 to 2.0
    float getGain() const;

private:
    float gain_;
};