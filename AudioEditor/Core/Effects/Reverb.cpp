#include "Reverb.h"
#include <algorithm>
#include <cmath>

constexpr int Reverb::COMB_TUNING[NUM_COMBS];
constexpr int Reverb::ALLPASS_TUNING[NUM_ALLPASSES];

Reverb::Reverb(std::shared_ptr<ILogger> logger)
    : wetMix_(0.3f)
    , dryMix_(1.0f)
    , roomSize_(0.5f)
    , damping_(0.5f)
    , preDelayMs_(20.0f)
    , width_(1.0f)
    , earlyMix_(0.5f)
    , feedback_(0.7f)
    , damp1_(0.5f)
    , damp2_(0.5f)
    , dcBlockL_(0.0f)
    , dcBlockR_(0.0f)
    , logger_(logger)
{
    initializeFilters();
    updateParameters();
}

void Reverb::initializeFilters() {
    for (int i = 0; i < NUM_COMBS; ++i) {
        combsL_[i].init(COMB_TUNING[i]);
        combsR_[i].init(COMB_TUNING[i] + STEREO_SPREAD);
    }
    
    for (int i = 0; i < NUM_ALLPASSES; ++i) {
        allpassesL_[i].init(ALLPASS_TUNING[i]);
        allpassesR_[i].init(ALLPASS_TUNING[i] + STEREO_SPREAD);
    }
    
    earlyL_.init(SAMPLE_RATE);
    earlyR_.init(SAMPLE_RATE);
    
    size_t maxPreDelay = static_cast<size_t>(SAMPLE_RATE * 0.1);
    preDelayBufferL_.resize(maxPreDelay, 0.0f);
    preDelayBufferR_.resize(maxPreDelay, 0.0f);
    preDelayIndex_ = 0;
    preDelaySamples_ = static_cast<size_t>((preDelayMs_ / 1000.0f) * SAMPLE_RATE);
}

void Reverb::setIntensity(float intensityPercent) {
    float intensity = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
    
    dryMix_ = 1.0f;
    
    // Stronger wet signal
    wetMix_ = 0.4f + intensity * 1.0f;  // 0.4 to 1.4
    
    roomSize_ = 0.4f + intensity * 0.45f;  // 0.4 to 0.85
    damping_ = 0.5f - intensity * 0.25f;
    preDelayMs_ = 10.0f + intensity * 30.0f;
    earlyMix_ = 0.35f;
    
    updateParameters();
    
    if (logger_) {
        logger_->log("Reverb intensity: " + std::to_string(intensityPercent) + 
                     "% (wet: " + std::to_string(wetMix_) + 
                     ", room: " + std::to_string(roomSize_) + ")");
    }
}

void Reverb::updateParameters() {
    feedback_ = 0.65f + roomSize_ * 0.25f;  // 0.65 to 0.9
    feedback_ = std::clamp(feedback_, 0.0f, 0.9f);
    
    damp1_ = damping_;
    damp2_ = 1.0f - damping_;
    
    preDelaySamples_ = static_cast<size_t>((preDelayMs_ / 1000.0f) * SAMPLE_RATE);
    if (preDelaySamples_ >= preDelayBufferL_.size()) {
        preDelaySamples_ = preDelayBufferL_.size() - 1;
    }
}

size_t Reverb::apply(float* buffer, size_t bufferSize) {
    const float combInputGain = 0.08f;   // Back up from 0.025
    const float combOutputGain = 0.4f;   // Back up from 0.25
    
    const float wet1 = wetMix_ * (width_ / 2.0f + 0.5f);
    const float wet2 = wetMix_ * ((1.0f - width_) / 2.0f);
    
    const float dcCoeff = 0.995f;
    
    for (size_t i = 0; i < bufferSize; i += 2) {
        float inputL = buffer[i];
        float inputR = buffer[i + 1];
        
        // Pre-delay
        float delayedL, delayedR;
        if (preDelaySamples_ > 0) {
            size_t readIndex = (preDelayIndex_ + preDelayBufferL_.size() - preDelaySamples_) 
                               % preDelayBufferL_.size();
            delayedL = preDelayBufferL_[readIndex];
            delayedR = preDelayBufferR_[readIndex];
            
            preDelayBufferL_[preDelayIndex_] = inputL;
            preDelayBufferR_[preDelayIndex_] = inputR;
            preDelayIndex_ = (preDelayIndex_ + 1) % preDelayBufferL_.size();
        } else {
            delayedL = inputL;
            delayedR = inputR;
        }
        
        // Early reflections
        float earlyL = earlyL_.process(delayedL) * 0.8f;
        float earlyR = earlyR_.process(delayedR) * 0.8f;
        
        // Comb filter input
        float combInputL = delayedL * combInputGain;
        float combInputR = delayedR * combInputGain;
        
        // Parallel comb filters
        float combOutL = 0.0f;
        float combOutR = 0.0f;
        
        for (int c = 0; c < NUM_COMBS; ++c) {
            combOutL += combsL_[c].process(combInputL, feedback_, damp1_, damp2_);
            combOutR += combsR_[c].process(combInputR, feedback_, damp1_, damp2_);
        }
        
        combOutL *= combOutputGain;
        combOutR *= combOutputGain;
        
        // Allpass filters
        for (int a = 0; a < NUM_ALLPASSES; ++a) {
            combOutL = allpassesL_[a].process(combOutL, 0.5f);
            combOutR = allpassesR_[a].process(combOutR, 0.5f);
        }
        
        // DC blocker
        float prevDcL = dcBlockL_;
        float prevDcR = dcBlockR_;
        dcBlockL_ = combOutL + dcCoeff * dcBlockL_;
        dcBlockR_ = combOutR + dcCoeff * dcBlockR_;
        combOutL = dcBlockL_ - prevDcL;
        combOutR = dcBlockR_ - prevDcR;
        
        // Mix early and late
        float reverbL = earlyL * earlyMix_ + combOutL * (1.0f - earlyMix_);
        float reverbR = earlyR * earlyMix_ + combOutR * (1.0f - earlyMix_);
        
        // Stereo width
        float wetL = reverbL * wet1 + reverbR * wet2;
        float wetR = reverbR * wet1 + reverbL * wet2;
        
        // Additive mix
        float outL = inputL * dryMix_ + wetL;
        float outR = inputR * dryMix_ + wetR;
        
        // Tanh saturation - smooth and musical
        buffer[i]     = std::tanh(outL);
        buffer[i + 1] = std::tanh(outR);
    }
    
    return bufferSize;
}

void Reverb::reset() {
    for (int i = 0; i < NUM_COMBS; ++i) {
        combsL_[i].clear();
        combsR_[i].clear();
    }
    
    for (int i = 0; i < NUM_ALLPASSES; ++i) {
        allpassesL_[i].clear();
        allpassesR_[i].clear();
    }
    
    earlyL_.clear();
    earlyR_.clear();
    
    std::fill(preDelayBufferL_.begin(), preDelayBufferL_.end(), 0.0f);
    std::fill(preDelayBufferR_.begin(), preDelayBufferR_.end(), 0.0f);
    preDelayIndex_ = 0;
    
    dcBlockL_ = 0.0f;
    dcBlockR_ = 0.0f;
}