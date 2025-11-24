#include "Reverb.h"
#include <algorithm>
#include <cmath>

// Tuned delay line lengths (in samples at 44.1kHz)
// These are carefully chosen prime numbers for optimal diffusion
static const int COMB_TUNINGS_L[] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
static const int COMB_TUNINGS_R[] = {1116+23, 1188+23, 1277+23, 1356+23, 1422+23, 1491+23, 1557+23, 1617+23};
static const int ALLPASS_TUNINGS_L[] = {556, 441, 341, 225};
static const int ALLPASS_TUNINGS_R[] = {556+23, 441+23, 341+23, 225+23};
static const int NUM_COMBS = 8;
static const int NUM_ALLPASS = 4;

Reverb::Reverb(std::shared_ptr<ILogger> logger) 
    : wetMix_(0.3f), roomSize_(0.5f), damping_(0.5f), 
      feedback_(0.84f), preDelaySamples_(0), logger_(logger)
{
    // Initialize comb filters (left channel)
    for (int i = 0; i < NUM_COMBS; ++i) {
        CombFilter comb;
        comb.buffer.resize(COMB_TUNINGS_L[i], 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        combFiltersL_.push_back(comb);
    }
    
    // Initialize comb filters (right channel - slightly detuned for stereo width)
    for (int i = 0; i < NUM_COMBS; ++i) {
        CombFilter comb;
        comb.buffer.resize(COMB_TUNINGS_R[i], 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        combFiltersR_.push_back(comb);
    }
    
    // Initialize all-pass filters (left)
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        AllPassFilter ap;
        ap.buffer.resize(ALLPASS_TUNINGS_L[i], 0.0f);
        ap.index = 0;
        allPassFiltersL_.push_back(ap);
    }
    
    // Initialize all-pass filters (right)
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        AllPassFilter ap;
        ap.buffer.resize(ALLPASS_TUNINGS_R[i], 0.0f);
        ap.index = 0;
        allPassFiltersR_.push_back(ap);
    }
    
    // Pre-delay buffer (up to 50ms)
    preDelayBuffer_.resize(2205, 0.0f);  // 50ms at 44.1kHz
    
    updateParameters();
}

void Reverb::updateParameters() {
    // Room size affects feedback
    feedback_ = 0.7f + roomSize_ * 0.18f;  // 0.7 to 0.88
}

void Reverb::setIntensity(float intensityPercent) {
    float t = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
    
    // Wet mix: 15-40%
    wetMix_ = 0.15f + t * 0.25f;
    
    // Room size: 0.3-0.85 (small room to large hall)
    roomSize_ = 0.3f + t * 0.55f;
    
    // Damping: 0.2-0.7 (bright to dark)
    damping_ = 0.2f + t * 0.5f;
    
    // Pre-delay: 0-30ms (separation between dry and wet)
    preDelaySamples_ = static_cast<int>(t * 0.03f * 44100.0f);
    
    updateParameters();
    
    logger_->log("Reverb intensity: " + std::to_string(intensityPercent) + 
                 "%, wet=" + std::to_string(wetMix_ * 100) + 
                 "%, room=" + std::to_string(roomSize_) +
                 ", damping=" + std::to_string(damping_));
}

float Reverb::processComb(CombFilter& comb, float input) {
    // Read from delay line
    float output = comb.buffer[comb.index];
    
    // One-pole lowpass filter (damping)
    comb.filterStore = output * (1.0f - damping_) + comb.filterStore * damping_;
    
    // Write input + feedback into delay line
    comb.buffer[comb.index] = input + comb.filterStore * feedback_;
    
    // Advance buffer position
    comb.index = (comb.index + 1) % comb.buffer.size();
    
    return output;
}

float Reverb::processAllPass(AllPassFilter& ap, float input) {
    // Read from delay line
    float bufOut = ap.buffer[ap.index];
    
    // All-pass formula: output = -input + bufOut
    // Write: input + bufOut * gain
    float output = -input + bufOut;
    ap.buffer[ap.index] = input + bufOut * 0.5f;
    
    // Advance buffer position
    ap.index = (ap.index + 1) % ap.buffer.size();
    
    return output;
}

size_t Reverb::apply(float* buffer, size_t bufferSize) {
    logger_->log("Applying Reverb (wet: " + std::to_string(wetMix_ * 100) + 
                 "%, room: " + std::to_string(roomSize_) + ")");
    
    const int channels = 2;  // Stereo
    size_t numFrames = bufferSize / channels;

    for (size_t i = 0; i < numFrames; ++i) {
        size_t leftIdx = i * channels;
        size_t rightIdx = i * channels + 1;
        
        float inputL = buffer[leftIdx];
        float inputR = buffer[rightIdx];
        
        // Mono sum for reverb input (typical approach)
        float monoInput = (inputL + inputR) * 0.5f;
        
        // Apply pre-delay
        float delayedInput = preDelayBuffer_[preDelayIndex_];
        preDelayBuffer_[preDelayIndex_] = monoInput;
        preDelayIndex_ = (preDelayIndex_ + 1) % preDelayBuffer_.size();
        
        // Process through parallel comb filters
        float combOutL = 0.0f;
        for (auto& comb : combFiltersL_) {
            combOutL += processComb(comb, delayedInput);
        }
        
        float combOutR = 0.0f;
        for (auto& comb : combFiltersR_) {
            combOutR += processComb(comb, delayedInput);
        }
        
        // Process through series all-pass filters (for diffusion)
        float reverbL = combOutL;
        for (auto& ap : allPassFiltersL_) {
            reverbL = processAllPass(ap, reverbL);
        }
        
        float reverbR = combOutR;
        for (auto& ap : allPassFiltersR_) {
            reverbR = processAllPass(ap, reverbR);
        }
        
        // Mix dry and wet with makeup gain
        float dryMix = 1.0f - wetMix_;
        buffer[leftIdx] = (inputL * dryMix + reverbL * wetMix_ * 0.015f) * 1.2f;
        buffer[rightIdx] = (inputR * dryMix + reverbR * wetMix_ * 0.015f) * 1.2f;
        
        // Safety clipping
        buffer[leftIdx] = std::clamp(buffer[leftIdx], -1.0f, 1.0f);
        buffer[rightIdx] = std::clamp(buffer[rightIdx], -1.0f, 1.0f);
    }
    
    return bufferSize;
}