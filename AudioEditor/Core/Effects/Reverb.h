#pragma once
#include "IEffect.h"

class Reverb : public IEffect {
public:
    void applyEffect(float* audioBuffer, size_t bufferSize) override;
};