#include "Speed.h"
#include <algorithm>
#include <cmath>

SpeedChangeEffect::SpeedChangeEffect(float speedFactor, std::shared_ptr<ILogger> logger)
    : speedFactor_(std::clamp(speedFactor, 
                              audio::speed::kMinSpeedFactor, 
                              audio::speed::kMaxSpeedFactor))
    , logger_(std::move(logger))
{
    if (logger_) {
        logger_->log("SpeedChangeEffect initialized with factor " + std::to_string(speedFactor_));
    }
}

void SpeedChangeEffect::setSpeedFactor(float speedFactor) {
    speedFactor_ = std::clamp(speedFactor, 
                              audio::speed::kMinSpeedFactor, 
                              audio::speed::kMaxSpeedFactor);
    if (logger_) {
        logger_->log("Speed factor set to " + std::to_string(speedFactor_) + "x");
    }
}

void SpeedChangeEffect::setParameter(const std::string& name, float value) {
    if (name == "speed") {
        setSpeedFactor(value);
    }
}

void SpeedChangeEffect::apply(std::vector<float>& audioBuffer) {
    if (std::abs(speedFactor_ - 1.0f) < 0.001f || audioBuffer.empty()) {
        return;  // No change needed
    }

    constexpr int channels = audio::kDefaultChannels;
    const size_t numFrames = audioBuffer.size() / channels;
    const size_t newFrames = static_cast<size_t>(numFrames / speedFactor_);
    const size_t newSize = newFrames * channels;
    
    // Create output buffer with correct size
    std::vector<float> output(newSize);

    // Process each channel separately
    for (int ch = 0; ch < channels; ++ch) {
        for (size_t i = 0; i < newFrames; ++i) {
            const float srcFloat = i * speedFactor_;
            const size_t src = static_cast<size_t>(srcFloat);
            const float frac = srcFloat - static_cast<float>(src);

            const size_t dstIdx = i * channels + ch;

            if (src > 0 && src + 2 < numFrames) {
                // Cubic interpolation for smooth resampling
                const float y0 = audioBuffer[(src - 1) * channels + ch];
                const float y1 = audioBuffer[src * channels + ch];
                const float y2 = audioBuffer[(src + 1) * channels + ch];
                const float y3 = audioBuffer[(src + 2) * channels + ch];
                
                const float c0 = y1;
                const float c1 = 0.5f * (y2 - y0);
                const float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
                const float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
                
                output[dstIdx] = c0 + c1 * frac + c2 * frac * frac + c3 * frac * frac * frac;
            } else if (src + 1 < numFrames) {
                // Linear interpolation at boundaries
                output[dstIdx] = audioBuffer[src * channels + ch] * (1.0f - frac) + 
                                 audioBuffer[(src + 1) * channels + ch] * frac;
            } else if (src < numFrames) {
                output[dstIdx] = audioBuffer[src * channels + ch];
            } else {
                output[dstIdx] = 0.0f;
            }
            
            output[dstIdx] = std::clamp(output[dstIdx], -1.0f, 1.0f);
        }
    }

    // Replace input buffer with output (safe - no buffer overflow possible)
    audioBuffer = std::move(output);
}
