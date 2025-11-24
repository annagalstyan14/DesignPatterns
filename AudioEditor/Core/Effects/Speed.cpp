#include "Speed.h"
#include <algorithm>
#include <cmath>

SpeedChangeEffect::SpeedChangeEffect(float speedFactor, std::shared_ptr<ILogger> logger)
    : speedFactor_(std::clamp(speedFactor, 0.1f, 2.0f)), logger_(logger) {
    logger_->log("SpeedChangeEffect initialized with factor " + std::to_string(speedFactor_));
}

void SpeedChangeEffect::setSpeedFactor(float speedFactor) {
    speedFactor_ = std::clamp(speedFactor, 0.1f, 2.0f);
    logger_->log("Speed factor set to " + std::to_string(speedFactor_) + "x");
}

float SpeedChangeEffect::getSpeedFactor() const {
    return speedFactor_;
}

size_t SpeedChangeEffect::apply(float* audioBuffer, size_t bufferSize) {
    if (speedFactor_ == 1.0f || bufferSize == 0) return bufferSize;

    const int channels = 2; // Stereo audio
    size_t numFrames = bufferSize / channels;
    size_t newFrames = static_cast<size_t>(numFrames / speedFactor_);
    size_t newSize = newFrames * channels;
    
    std::vector<float> temp(newSize);

    // Process each channel separately
    for (int ch = 0; ch < channels; ++ch) {
        for (size_t i = 0; i < newFrames; ++i) {
            float srcFloat = i * speedFactor_;
            size_t src = static_cast<size_t>(srcFloat);
            float frac = srcFloat - src;

            //size_t srcIdx = src * channels + ch;
            size_t dstIdx = i * channels + ch;

            if (src > 0 && src + 2 < numFrames) {
                // Cubic interpolation per channel
                float y0 = audioBuffer[(src - 1) * channels + ch];
                float y1 = audioBuffer[src * channels + ch];
                float y2 = audioBuffer[(src + 1) * channels + ch];
                float y3 = audioBuffer[(src + 2) * channels + ch];
                
                float c0 = y1;
                float c1 = 0.5f * (y2 - y0);
                float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
                float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
                
                temp[dstIdx] = c0 + c1 * frac + c2 * frac * frac + c3 * frac * frac * frac;
            } else if (src + 1 < numFrames) {
                // Linear interpolation at boundaries
                temp[dstIdx] = audioBuffer[src * channels + ch] * (1.0f - frac) + 
                               audioBuffer[(src + 1) * channels + ch] * frac;
            } else if (src < numFrames) {
                temp[dstIdx] = audioBuffer[src * channels + ch];
            } else {
                temp[dstIdx] = 0.0f;
            }
            
            temp[dstIdx] = std::clamp(temp[dstIdx], -1.0f, 1.0f);
        }
    }

    std::copy(temp.begin(), temp.end(), audioBuffer);
    return newSize;
}