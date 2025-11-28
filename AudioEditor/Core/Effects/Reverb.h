#pragma once

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <vector>
#include <memory>
#include <array>

/**
 * @brief Classic Schroeder reverb - smooth diffuse wash
 */
class Reverb : public IEffect {
public:
    explicit Reverb(std::shared_ptr<ILogger> logger);
    
    void apply(std::vector<float>& audioBuffer) override;
    void setParameter(const std::string& name, float value) override;
    [[nodiscard]] std::string getName() const noexcept override { return "Reverb"; }
    
    void setIntensity(float intensityPercent);
    void reset();

private:
    std::array<std::vector<float>, 4> combBuffers_;
    std::array<size_t, 4> combIndices_;
    std::array<float, 4> combFilterState_;
    
    std::array<std::vector<float>, 2> allpassBuffers_;
    std::array<size_t, 2> allpassIndices_;
    
    float wetMix_ = 0.5f;
    float feedback_ = 0.7f;
    float damping_ = 0.3f;
    
    std::shared_ptr<ILogger> logger_;
};