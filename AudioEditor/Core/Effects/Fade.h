#pragma once
#include "IEffect.h"

class Fade : public IEffect {
public:
    void applyEffect(float* audioBuffer, size_t bufferSize) override;
};