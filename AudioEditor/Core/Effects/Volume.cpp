#include "Volume.h"
#include <algorithm>
#include <cmath>

VolumeEffect::VolumeEffect(float gain, std::shared_ptr<ILogger> logger)
    : gain_(gain), logger_(logger)
{
    logger_->log("VolumeEffect created with gain: " + std::to_string(gain_));
}

void VolumeEffect::setGain(float gain) {
    gain_ = std::clamp(gain, 0.0f, 2.0f); // Cap gain between 0.0 and 2.0
    logger_->log("VolumeEffect gain set to: " + std::to_string(gain_));
}

size_t VolumeEffect::apply(float* buffer, size_t bufferSize) {
    if (std::abs(gain_ - 1.0f) < 0.001f) {
        // Gain is essentially 1.0, no change needed
        return bufferSize;
    }
    
    for (size_t i = 0; i < bufferSize; ++i) {
        buffer[i] *= gain_;
        
        // Soft clipping to prevent harsh distortion at high gain
        // Using tanh-like soft clipping
        if (buffer[i] > 1.0f) {
            buffer[i] = 1.0f - 1.0f / (buffer[i] + 1.0f);
        } else if (buffer[i] < -1.0f) {
            buffer[i] = -1.0f + 1.0f / (-buffer[i] + 1.0f);
        }
    }
    
    return bufferSize;
}