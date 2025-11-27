#pragma once
#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <vector>
#include <memory>
#include <algorithm>

class Reverb : public IEffect {
public:
    explicit Reverb(std::shared_ptr<ILogger> logger);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);
    void reset();
    
    void setWetMix(float wet) { wetMix_ = std::clamp(wet, 0.0f, 0.5f); }
    void setRoomSize(float size) { roomSize_ = std::clamp(size, 0.0f, 1.0f); updateParameters(); }
    void setDamping(float damp) { damping_ = std::clamp(damp, 0.0f, 1.0f); }
    
    void setParameter(const std::string& name, float value) override {
        if (name == "intensity") setIntensity(value * 100.0f);
        else if (name == "wetMix") setWetMix(value);
        else if (name == "roomSize") setRoomSize(value);
        else if (name == "damping") setDamping(value);
    }

private:
    // Simple delay lines
    std::vector<std::vector<float>> delayBuffers_;
    std::vector<size_t> delayIndices_;
    
    // Pre-delay buffer
    std::vector<float> preDelayBuffer_;
    size_t preDelayIndex_ = 0;
    int preDelaySamples_;
    
    // Parameters
    float wetMix_;
    float roomSize_;
    float damping_;
    float feedback_;
    
    std::shared_ptr<ILogger> logger_;
    
    void updateParameters();
};