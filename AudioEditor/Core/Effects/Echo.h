#pragma once
#include "IEffect.h"

class Echo : public IEffect {
public:
    void apply(float* audioBuffer, size_t bufferSize) override;
};