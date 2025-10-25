#pragma once
#include "IEffect.h"

class Reverb : public IEffect {
public:
    void apply(float* audioBuffer, size_t bufferSize) override;
};