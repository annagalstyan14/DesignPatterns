#pragma once

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include "../Constants.h"
#include <memory>

/**
 * @brief Volume/gain adjustment effect with soft clipping
 * 
 * Applies gain to audio samples with soft clipping to prevent
 * harsh distortion at high gain levels.
 * 
 * Design Pattern: Strategy (ConcreteStrategy)
 */
class VolumeEffect : public IEffect {
public:
    VolumeEffect(float gain, std::shared_ptr<ILogger> logger);
    
    void apply(std::vector<float>& audioBuffer) override;
    void setParameter(const std::string& name, float value) override;
    [[nodiscard]] std::string getName() const noexcept override { return "Volume"; }
    
    void setGain(float gain);
    [[nodiscard]] float getGain() const noexcept { return gain_; }

private:
    float gain_;
    std::shared_ptr<ILogger> logger_;
};
