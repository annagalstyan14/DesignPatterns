#pragma once
#include <cstddef>
#include <string>

class IEffect {
public:
    virtual ~IEffect() = default;
    virtual size_t apply(float* audioBuffer, size_t bufferSize) = 0;
    virtual void setParameter(const std::string& name, float value) = 0;
};