#pragma once
#include <cstddef>

class IEffect {
public:
    virtual ~IEffect() = default;
    virtual void apply(float* audioBuffer, size_t bufferSize) = 0;
};