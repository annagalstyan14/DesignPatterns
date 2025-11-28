#pragma once

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include "../Constants.h"
#include <vector>
#include <memory>
#include <algorithm>

/**
 * @brief Reverb effect using multiple delay lines
 * 
 * Simulates room acoustics by combining delayed copies
 * of the signal with feedback and damping.
 * 
 * Design Pattern: Strategy (ConcreteStrategy)
 */
class Reverb : public IEffect {
public:
    explicit Reverb(std::shared_ptr<ILogger> logger);
    
    void apply(std::vector<float>& audioBuffer) override;
    void setParameter(const std::string& name, float value) override;
    [[nodiscard]] std::string getName() const noexcept override { return "Reverb"; }
    
    void setIntensity(float intensityPercent);
    void reset();
    
    void setWetMix(float wet) noexcept { 
        wetMix_ = std::clamp(wet, 0.0f, audio::reverb::kMaxWetMix); 
    }
    void setRoomSize(float size) noexcept { 
        roomSize_ = std::clamp(size, 0.0f, 1.0f); 
        updateParameters(); 
    }
    void setDamping(float damp) noexcept { 
        damping_ = std::clamp(damp, 0.0f, 1.0f); 
    }
    
    [[nodiscard]] float getWetMix() const noexcept { return wetMix_; }
    [[nodiscard]] float getRoomSize() const noexcept { return roomSize_; }
    [[nodiscard]] float getDamping() const noexcept { return damping_; }

private:
    void updateParameters();

    // Delay lines
    std::vector<std::vector<float>> delayBuffers_;
    std::vector<size_t> delayIndices_;
    
    // Pre-delay buffer
    std::vector<float> preDelayBuffer_;
    size_t preDelayIndex_ = 0;
    
    // Parameters
    float wetMix_;
    float roomSize_;
    float damping_;
    float feedback_;
    
    std::shared_ptr<ILogger> logger_;
};
