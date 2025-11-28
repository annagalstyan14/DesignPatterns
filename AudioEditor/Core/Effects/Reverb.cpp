#include "Reverb.h"
#include <algorithm>
#include <cmath>

static constexpr std::array<size_t, 4> kCombDelays = {1116, 1188, 1277, 1356};
static constexpr std::array<size_t, 2> kAllpassDelays = {556, 441};
static constexpr float kAllpassGain = 0.5f;

Reverb::Reverb(std::shared_ptr<ILogger> logger)
    : logger_(std::move(logger))
{
    for (size_t i = 0; i < 4; ++i) {
        combBuffers_[i].resize(kCombDelays[i], 0.0f);
        combIndices_[i] = 0;
        combFilterState_[i] = 0.0f;
    }
    
    for (size_t i = 0; i < 2; ++i) {
        allpassBuffers_[i].resize(kAllpassDelays[i], 0.0f);
        allpassIndices_[i] = 0;
    }
}

void Reverb::setIntensity(float intensityPercent) {
    const float intensity = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
    
    const float scaled = std::min(intensity * 2.0f, 1.0f);
    
    wetMix_ = scaled * 0.7f;
    feedback_ = 0.5f + scaled * 0.3f;
    damping_ = 0.5f - scaled * 0.3f;
    
    if (logger_) {
        logger_->log("Reverb intensity: " + std::to_string(intensityPercent) + "%");
    }
}

void Reverb::setParameter(const std::string& name, float value) {
    if (name == "intensity") {
        setIntensity(value * 100.0f);
    }
}

void Reverb::apply(std::vector<float>& buffer) {
    if (buffer.empty()) return;
    
    reset();
    
    const float wet = wetMix_;
    const float dry = 1.0f - wet;
    
    for (size_t i = 0; i + 1 < buffer.size(); i += 2) {
        const float inputL = buffer[i];
        const float inputR = buffer[i + 1];
        const float mono = (inputL + inputR) * 0.5f;
        
        float combOut = 0.0f;
        for (size_t c = 0; c < 4; ++c) {
            float delayed = combBuffers_[c][combIndices_[c]];
            float filtered = delayed * (1.0f - damping_) + combFilterState_[c] * damping_;
            combFilterState_[c] = filtered;
            combBuffers_[c][combIndices_[c]] = mono + filtered * feedback_;
            combIndices_[c] = (combIndices_[c] + 1) % combBuffers_[c].size();
            combOut += delayed;
        }
        combOut *= 0.25f;
        
        float diffused = combOut;
        for (size_t a = 0; a < 2; ++a) {
            float delayed = allpassBuffers_[a][allpassIndices_[a]];
            float output = delayed - diffused * kAllpassGain;
            allpassBuffers_[a][allpassIndices_[a]] = diffused + delayed * kAllpassGain;
            allpassIndices_[a] = (allpassIndices_[a] + 1) % allpassBuffers_[a].size();
            diffused = output;
        }
        
        buffer[i] = inputL * dry + diffused * wet;
        buffer[i + 1] = inputR * dry + diffused * wet;
    }
}

void Reverb::reset() {
    for (size_t i = 0; i < 4; ++i) {
        std::fill(combBuffers_[i].begin(), combBuffers_[i].end(), 0.0f);
        combIndices_[i] = 0;
        combFilterState_[i] = 0.0f;
    }
    for (size_t i = 0; i < 2; ++i) {
        std::fill(allpassBuffers_[i].begin(), allpassBuffers_[i].end(), 0.0f);
        allpassIndices_[i] = 0;
    }
}