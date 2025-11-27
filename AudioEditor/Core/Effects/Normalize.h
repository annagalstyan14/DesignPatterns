#pragma once

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include "../Constants.h"
#include <memory>

/**
 * @brief Normalize effect to match target RMS level
 * 
 * Adjusts gain to achieve a target RMS level while
 * preventing clipping with a peak limiter.
 * 
 * Design Pattern: Strategy (ConcreteStrategy)
 */
class NormalizeEffect : public IEffect {
public:
    explicit NormalizeEffect(std::shared_ptr<ILogger> logger, 
                            float targetRMS = audio::normalize::kDefaultTargetRMS);
    
    void apply(std::vector<float>& audioBuffer) override;
    void setParameter(const std::string& name, float value) override;
    [[nodiscard]] std::string getName() const noexcept override { return "Normalize"; }
    
    void setTargetRMS(float rms) noexcept { targetRMS_ = rms; }
    void setTargetPeak(float peak) noexcept { targetPeak_ = peak; }
    
    [[nodiscard]] float getTargetRMS() const noexcept { return targetRMS_; }
    [[nodiscard]] float getTargetPeak() const noexcept { return targetPeak_; }

private:
    [[nodiscard]] static float calculateRMS(const std::vector<float>& buffer);
    [[nodiscard]] static float calculatePeak(const std::vector<float>& buffer);

    std::shared_ptr<ILogger> logger_;
    float targetRMS_;
    float targetPeak_;
};
