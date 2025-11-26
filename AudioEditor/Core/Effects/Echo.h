#pragma once
#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>
#include <algorithm>

class Echo : public IEffect {
public:
    explicit Echo(std::shared_ptr<ILogger> logger);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);
    
    // Optional: for manual control
    void setWetMix(float wet) { wetMix_ = std::clamp(wet, 0.0f, 0.6f); }
    void setFeedback(float fb) { feedback_ = std::clamp(fb, 0.0f, 0.85f); }

private:
    float wetMix_;
    float feedback_;        // How much echo feeds back into itself
    int delaySamples_;
    std::vector<float> delayBuffer_;  // Circular buffer for echo
    size_t writePos_;       // Current write position in buffer
    std::shared_ptr<ILogger> logger_;
};