#include "Volume.h"
#include <algorithm>
#include <cmath>

VolumeEffect::VolumeEffect(float gain, std::shared_ptr<ILogger> logger)
    : gain_(std::clamp(gain, audio::volume::kMinGain, audio::volume::kMaxGain))
    , logger_(std::move(logger))
{
    if (logger_) {
        logger_->log("VolumeEffect created with gain: " + std::to_string(gain_));
    }
}

void VolumeEffect::setGain(float gain) {
    gain_ = std::clamp(gain, audio::volume::kMinGain, audio::volume::kMaxGain);
    if (logger_) {
        logger_->log("VolumeEffect gain set to: " + std::to_string(gain_));
    }
}

void VolumeEffect::setParameter(const std::string& name, float value) {
    if (name == "gain") {
        setGain(value);
    }
}

void VolumeEffect::apply(std::vector<float>& buffer) {
    // Skip if gain is essentially 1.0
    if (std::abs(gain_ - 1.0f) < 0.001f) {
        return;
    }
    
    for (float& sample : buffer) {
        sample *= gain_;
        
        // Soft clipping to prevent harsh distortion at high gain
        // Using tanh-like soft clipping curve
        if (sample > audio::volume::kSoftClipThreshold) {
            sample = audio::volume::kSoftClipThreshold - 
                     audio::volume::kSoftClipThreshold / (sample + audio::volume::kSoftClipThreshold);
        } else if (sample < -audio::volume::kSoftClipThreshold) {
            sample = -audio::volume::kSoftClipThreshold + 
                     audio::volume::kSoftClipThreshold / (-sample + audio::volume::kSoftClipThreshold);
        }
    }
}
