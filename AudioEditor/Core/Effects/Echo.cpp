#include "Echo.h"
#include <algorithm>
#include <cmath>
#include <vector>

Echo::Echo(std::shared_ptr<ILogger> logger) 
    : wetMix_(0.3f), feedback_(0.5f), delaySamples_(11025), 
      writePos_(0), logger_(logger) {
    
    // Allocate delay buffer - make it large enough for longest delay
    // 2 seconds = 88200 samples at 44.1kHz
    delayBuffer_.resize(88200, 0.0f);
}

void Echo::setIntensity(float intensityPercent) {
    float t = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);

    // Delay time: 250ms to 500ms (1/8 to 1/4 note at 120 BPM)
    float delaySec = 0.25f + t * 0.25f;
    delaySamples_ = static_cast<int>(delaySec * 44100.0f);

    // Wet mix: 20-50%
    wetMix_ = 0.4f + t * 0.3f;
    
    // Feedback: 30-70% (higher = longer tail)
    // At 70%, echo will repeat ~10 times before becoming inaudible
    feedback_ = 0.5f + t * 0.25f;

    logger_->log("Echo: " + std::to_string(intensityPercent) +
                 "% -> " + std::to_string(delaySec*1000) + "ms, " +
                 "feedback: " + std::to_string(feedback_ * 100) + "%, " +
                 "wet: " + std::to_string(wetMix_ * 100) + "%");
}

size_t Echo::apply(float* audioBuffer, size_t bufferSize) {
    size_t bufferLen = delayBuffer_.size();

    for (size_t i = 0; i < bufferSize; ++i) {
        float input = audioBuffer[i];
        
        // Calculate read position (where to read the delayed signal from)
        size_t readPos = (writePos_ + bufferLen - delaySamples_) % bufferLen;
        
        // Read the delayed signal
        float delayed = delayBuffer_[readPos];
        
        // Write current input + feedback from delayed signal into delay buffer
        // This creates the recirculating echo effect
        delayBuffer_[writePos_] = input + delayed * feedback_;
        
        // Mix dry and wet signals
        float dryMix = 1.0f - wetMix_;
        audioBuffer[i] = input * dryMix + delayed * wetMix_;
        
        // Makeup gain to prevent volume loss
        audioBuffer[i] *= 1.1f;
        
        // Safety clipping
        audioBuffer[i] = std::clamp(audioBuffer[i], -1.0f, 1.0f);
        
        // Advance write position
        writePos_ = (writePos_ + 1) % bufferLen;
    }
    
    logger_->log("Echo applied: delay=" + std::to_string(delaySamples_) + 
                 " samples, feedback=" + std::to_string(feedback_ * 100) + "%");
    
    return bufferSize;
}