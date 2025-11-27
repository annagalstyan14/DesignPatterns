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
    
    // Optional: manual control
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
    struct CombFilter {
        std::vector<float> buffer;
        size_t index = 0;
        float filterStore = 0.0f;  // For one-pole lowpass
    };
    
    struct AllPassFilter {
        std::vector<float> buffer;
        size_t index = 0;
    };

    // Comb filters (parallel)
    std::vector<CombFilter> combFiltersL_;
    std::vector<CombFilter> combFiltersR_;
    
    // All-pass filters (series) for diffusion
    std::vector<AllPassFilter> allPassFiltersL_;
    std::vector<AllPassFilter> allPassFiltersR_;
    
    // Pre-delay buffer
    std::vector<float> preDelayBuffer_;
    size_t preDelayIndex_ = 0;
    int preDelaySamples_;

    // Echo blending (for cinematic tails)
    std::vector<float> echoBufferL_;
    std::vector<float> echoBufferR_;
    size_t echoWriteIndexL_ = 0;
    size_t echoWriteIndexR_ = 0;
    int echoDelaySamples_ = 0;
    float echoFeedback_ = 0.0f;
    float echoMix_ = 0.0f;
    
    // Parameters
    float wetMix_;
    float roomSize_;
    float damping_;
    float feedback_;
    
    std::shared_ptr<ILogger> logger_;
    
    // Highpass for mud reduction
    float wetHighpassStoreL_ = 0.0f;
    float wetHighpassStoreR_ = 0.0f;
    float highpassCoeff_ = 0.995f;  // ~100Hz at 44.1kHz
    
    // Helper functions
    void updateParameters();
    float processComb(CombFilter& comb, float input);
    float processAllPass(AllPassFilter& ap, float input);
};