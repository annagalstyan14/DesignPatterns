#pragma once
#include "IEffect.h"

class Echo : public IEffect {
public:
    void applyEffect(float* audioBuffer, size_t bufferSize) override;
};