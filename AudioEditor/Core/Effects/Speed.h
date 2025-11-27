#pragma once

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include "../Constants.h"
#include <memory>

/**
 * @brief Speed change effect using high-quality interpolation
 * 
 * Changes playback speed by resampling. Uses cubic interpolation
 * for smooth results at non-integer speed factors.
 * 
 * Design Pattern: Strategy (ConcreteStrategy)
 * 
 * @note This effect changes the buffer size (faster = shorter, slower = longer)
 */
class SpeedChangeEffect : public IEffect {
public:
    SpeedChangeEffect(float speedFactor, std::shared_ptr<ILogger> logger);
    
    void apply(std::vector<float>& audioBuffer) override;
    void setParameter(const std::string& name, float value) override;
    [[nodiscard]] std::string getName() const noexcept override { return "Speed"; }
    
    void setSpeedFactor(float speedFactor);
    [[nodiscard]] float getSpeedFactor() const noexcept { return speedFactor_; }

private:
    float speedFactor_;
    std::shared_ptr<ILogger> logger_;
};
