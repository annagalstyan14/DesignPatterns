#pragma once
#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>

class VolumeEffect : public IEffect {
public:
    VolumeEffect(float gain, std::shared_ptr<ILogger> logger);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setGain(float gain);  // 0.0 to 2.0
    float getGain() const;

private:
    float gain_;
    std::shared_ptr<ILogger> logger_;
};