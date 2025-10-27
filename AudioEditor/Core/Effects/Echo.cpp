#include "Echo.h"
#include "../Logger.h"

Echo::Echo() : echoGain_(0.1f), delaySamples_(6615), numEchoes_(1), decayFactor_(0.25f) {
}

void Echo::setIntensity(float intensityPercent) {
    echoGain_ = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f); 
    Logger::getInstance().log("Echo intensity set to " + std::to_string(intensityPercent) + "%");
}

void Echo::apply(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Echo Effect with gain " + std::to_string(echoGain_));

    for (int echo = 1; echo <= numEchoes_; ++echo) {
        float currentGain = echoGain_ * std::pow(decayFactor_, echo - 1);
        size_t currentDelay = delaySamples_ * echo * 2;
        Logger::getInstance().log("Applying echo " + std::to_string(echo) + " with gain " + std::to_string(currentGain) + " and delay " + std::to_string(currentDelay / 2) + " samples");
        for (size_t i = currentDelay; i < bufferSize; i += 2) {
            audioBuffer[i] += audioBuffer[i - currentDelay] * currentGain;

            if (i + 1 < bufferSize) {
                audioBuffer[i + 1] += audioBuffer[i + 1 - currentDelay] * currentGain;
            }
            
            if (audioBuffer[i] > 1.0f) audioBuffer[i] = 1.0f;
            if (audioBuffer[i] < -1.0f) audioBuffer[i] = -1.0f;
            if (i + 1 < bufferSize) {
                if (audioBuffer[i + 1] > 1.0f) audioBuffer[i + 1] = 1.0f;
                if (audioBuffer[i + 1] < -1.0f) audioBuffer[i + 1] = -1.0f;
            }
        }
    }
}