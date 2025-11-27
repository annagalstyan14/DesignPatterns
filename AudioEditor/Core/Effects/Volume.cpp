#include "Volume.h"
#include <algorithm>

VolumeEffect::VolumeEffect(float gain, std::shared_ptr<ILogger> logger) 
    : gain_(std::clamp(gain, 0.0f, 4.0f)), logger_(logger) {
    logger_->log("VolumeEffect initialized with gain " + std::to_string(gain_));
}

void VolumeEffect::setGain(float gain) {
    gain_ = std::clamp(gain, 0.0f, 4.0f);
    logger_->log("Volume gain set to " + std::to_string(gain_));
}

float VolumeEffect::getGain() const {
    return gain_;
}

size_t VolumeEffect::apply(float* audioBuffer, size_t bufferSize) {
    for (size_t i = 0; i < bufferSize; ++i) {
        audioBuffer[i] *= gain_;
        audioBuffer[i] = std::clamp(audioBuffer[i], -1.0f, 1.0f);
    }
    return bufferSize;
}