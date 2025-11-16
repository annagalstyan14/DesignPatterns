#pragma once
#include <cstddef>

class IEffect {
public:
    virtual ~IEffect() = default;
    virtual size_t apply(float* audioBuffer, size_t bufferSize) = 0;
};