#include "Reverb.h"
#include "../Logger.h"

Reverb::Reverb() : reverbGain_(0.5f), delaySamples_(3308) {
}

void Reverb::setIntensity(float intensityPercent) {
    reverbGain_ = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
    Logger::getInstance().log("Reverb intensity set to " + std::to_string(intensityPercent) + "%");
}

void Reverb::apply(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Reverb Effect with gain " + std::to_string(reverbGain_));
    
    for (size_t i = delaySamples_ * 2; i < bufferSize; i += 2) {

        audioBuffer[i] += audioBuffer[i - delaySamples_ * 2] * reverbGain_;
        if (i + 1 < bufferSize) {
            audioBuffer[i + 1] += audioBuffer[i + 1 - delaySamples_ * 2] * reverbGain_;
        }
        
        if (audioBuffer[i] > 1.0f) audioBuffer[i] = 1.0f;
        if (audioBuffer[i] < -1.0f) audioBuffer[i] = -1.0f;
        if (i + 1 < bufferSize) {
            if (audioBuffer[i + 1] > 1.0f) audioBuffer[i + 1] = 1.0f;
            if (audioBuffer[i + 1] < -1.0f) audioBuffer[i + 1] = -1.0f;
        }
    }
}