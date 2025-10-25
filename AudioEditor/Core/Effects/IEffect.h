#pragma once
#include <cstddef>

class IEffect{

public:
    virtual ~IEffect() = default;

    virtual void applyEffect(float* audioBuffer, size_t bufferSize) = 0;
};