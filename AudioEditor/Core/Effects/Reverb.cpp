#include "Reverb.h"
#include <algorithm>
#include <cmath>

Reverb::Reverb(std::shared_ptr<ILogger> logger)
    : wetMix_(audio::reverb::kDefaultWetMix)
    , roomSize_(audio::reverb::kDefaultRoomSize)
    , damping_(audio::reverb::kDefaultDamping)
    , feedback_(audio::reverb::kDefaultFeedback)
    , logger_(std::move(logger))
{
    // Initialize delay lines with prime number lengths to avoid resonance
    delayBuffers_.reserve(audio::reverb::kNumDelayLines);
    delayIndices_.reserve(audio::reverb::kNumDelayLines);
    
    for (size_t i = 0; i < audio::reverb::kNumDelayLines; ++i) {
        delayBuffers_.emplace_back(audio::reverb::kDelayLineLengths[i], 0.0f);
        delayIndices_.push_back(0);
    }
    
    // Pre-delay buffer (~20ms at 44.1kHz)
    preDelayBuffer_.resize(audio::reverb::kPreDelaySamples, 0.0f);
    preDelayIndex_ = 0;
    
    updateParameters();
}

void Reverb::setIntensity(float intensityPercent) {
    const float intensity = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);

    wetMix_ = 0.3f + intensity * 0.7f;
    roomSize_ = 0.5f + intensity * 0.5f;
    damping_ = 0.2f + (1.0f - intensity) * 0.2f;

    updateParameters();
    
    if (logger_) {
        logger_->log("Reverb intensity set to " + std::to_string(intensityPercent) + 
                     "% (wet: " + std::to_string(wetMix_) + ")");
    }
}

void Reverb::setParameter(const std::string& name, float value) {
    if (name == "intensity") {
        setIntensity(value * 100.0f);
    } else if (name == "wetMix") {
        setWetMix(value);
    } else if (name == "roomSize") {
        setRoomSize(value);
    } else if (name == "damping") {
        setDamping(value);
    }
}

void Reverb::updateParameters() {
    feedback_ = 0.6f + roomSize_ * 0.4f;
}

void Reverb::apply(std::vector<float>& buffer) {
    if (buffer.empty()) return;
    
    const float dryMix = 1.0f - wetMix_;
    const size_t bufferSize = buffer.size();
    
    for (size_t i = 0; i + 1 < bufferSize; i += 2) {
        const float inputL = buffer[i];
        const float inputR = buffer[i + 1];
        
        // Mix to mono for reverb input
        const float monoInput = (inputL + inputR) * 0.5f;
        
        // Pre-delay
        const float delayedInput = preDelayBuffer_[preDelayIndex_];
        preDelayBuffer_[preDelayIndex_] = monoInput;
        preDelayIndex_ = (preDelayIndex_ + 1) % preDelayBuffer_.size();
        
        // Sum of delay lines with different feedback amounts
        float reverbSum = 0.0f;
        
        for (size_t d = 0; d < delayBuffers_.size(); ++d) {
            auto& buf = delayBuffers_[d];
            size_t& idx = delayIndices_[d];
            
            const float delayed = buf[idx];
            
            // Apply damping (simple low-pass)
            const float damped = delayed * (1.0f - damping_) + delayedInput * damping_ * 0.1f;
            
            // Write back with feedback
            buf[idx] = delayedInput + damped * feedback_;
            
            // Advance index
            idx = (idx + 1) % buf.size();
            
            // Alternate sign for better diffusion
            reverbSum += delayed * ((d % 2 == 0) ? 1.0f : -1.0f);
        }
        
        // Normalize reverb sum
        reverbSum /= static_cast<float>(delayBuffers_.size());
        
        // Soft clipping to prevent harshness
        constexpr float threshold = audio::reverb::kSoftClipThreshold;
        if (reverbSum > threshold) {
            reverbSum = threshold + (reverbSum - threshold) * 0.3f;
        } else if (reverbSum < -threshold) {
            reverbSum = -threshold + (reverbSum + threshold) * 0.3f;
        }
        
        // Mix dry and wet
        buffer[i] = std::clamp(inputL * dryMix + reverbSum * wetMix_, -1.0f, 1.0f);
        buffer[i + 1] = std::clamp(inputR * dryMix + reverbSum * wetMix_, -1.0f, 1.0f);
    }
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
