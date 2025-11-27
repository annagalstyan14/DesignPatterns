#include "Reverb.h"
#include <algorithm>
#include <cmath>

Reverb::Reverb(std::shared_ptr<ILogger> logger)
    : wetMix_(0.3f), roomSize_(0.5f), damping_(0.3f),
      feedback_(0.7f), preDelaySamples_(882), logger_(logger)
{
    // Simple delay-based reverb with 4 delay lines at different lengths
    // Using prime numbers for delay lengths to avoid resonance
    int delayLengths[] = {1557, 1617, 1491, 1422, 1277, 1356};
    
    for (int i = 0; i < 6; ++i) {
        delayBuffers_.push_back(std::vector<float>(delayLengths[i], 0.0f));
        delayIndices_.push_back(0);
    }
    
    // Pre-delay buffer (about 20ms at 44.1kHz)
    preDelayBuffer_.resize(882, 0.0f);
    preDelayIndex_ = 0;
    
    updateParameters();
}

void Reverb::setIntensity(float intensityPercent) {
    float intensity = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);

    wetMix_ = 0.3f + intensity * 0.7f;  // Start at 30% wet mix and scale to 100%
    roomSize_ = 0.5f + intensity * 0.5f;  // 0.5 to 1.0 for noticeable space scaling
    damping_ = 0.2f + (1.0f - intensity) * 0.2f;  // Smooth decay with less harshness

    updateParameters();
    logger_->log("Reverb intensity set to " + std::to_string(intensityPercent) + 
                 "% (wet: " + std::to_string(wetMix_) + ")");
}

void Reverb::updateParameters() {
    feedback_ = 0.6f + roomSize_ * 0.4f;  // Extend reverb tail more dramatically
}

size_t Reverb::apply(float* buffer, size_t bufferSize) {
    const float dryMix = 1.0f - wetMix_;
    
    for (size_t i = 0; i < bufferSize; i += 2) {
        float inputL = buffer[i];
        float inputR = buffer[i + 1];
        
        // Mix to mono for reverb input
        float monoInput = (inputL + inputR) * 0.5f;
        
        // Pre-delay
        float delayedInput = preDelayBuffer_[preDelayIndex_];
        preDelayBuffer_[preDelayIndex_] = monoInput;
        preDelayIndex_ = (preDelayIndex_ + 1) % preDelayBuffer_.size();
        
        // Sum of delay lines with different feedback amounts
        float reverbSum = 0.0f;
        
        for (size_t d = 0; d < delayBuffers_.size(); ++d) {
            auto& buf = delayBuffers_[d];
            size_t& idx = delayIndices_[d];
            
            float delayed = buf[idx];
            
            // Apply damping (simple low-pass)
            float damped = delayed * (1.0f - damping_) + delayedInput * damping_ * 0.1f;
            
            // Write back with feedback
            buf[idx] = delayedInput + damped * feedback_;
            
            // Advance index
            idx = (idx + 1) % buf.size();
            
            // Alternate sign for better diffusion
            reverbSum += delayed * ((d % 2 == 0) ? 1.0f : -1.0f);
        }
        
        // Normalize reverb sum
        reverbSum /= delayBuffers_.size();
        
        // Simple soft clipping on reverb to prevent harshness
        if (reverbSum > 0.8f) {
            reverbSum = 0.8f + (reverbSum - 0.8f) * 0.3f;
        } else if (reverbSum < -0.8f) {
            reverbSum = -0.8f + (reverbSum + 0.8f) * 0.3f;
        }
        
        // Mix dry and wet - keep stereo image from dry signal
        float wetL = reverbSum;
        float wetR = reverbSum;
        
        buffer[i] = inputL * dryMix + wetL * wetMix_;
        buffer[i + 1] = inputR * dryMix + wetR * wetMix_;
        
        // Final clamp
        buffer[i] = std::clamp(buffer[i], -1.0f, 1.0f);
        buffer[i + 1] = std::clamp(buffer[i + 1], -1.0f, 1.0f);
    }
    
    return bufferSize;
}

void Reverb::reset() {
    for (auto& buf : delayBuffers_) {
        std::fill(buf.begin(), buf.end(), 0.0f);
    }
    for (auto& idx : delayIndices_) {
        idx = 0;
    }
    std::fill(preDelayBuffer_.begin(), preDelayBuffer_.end(), 0.0f);
    preDelayIndex_ = 0;
}