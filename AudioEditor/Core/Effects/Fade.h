#pragma once
#include "IEffect.h"

class Fade : public IEffect {
public:
    void apply(float* audioBuffer, size_t bufferSize) override;
};