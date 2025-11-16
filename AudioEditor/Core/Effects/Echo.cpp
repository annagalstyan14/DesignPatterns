#include "Echo.h"
#include "../Logger.h"

Echo::Echo() : echoGain_(0.0f), delaySamples_(6615), numEchoes_(1), decayFactor_(0.5f) {
}

void Echo::setIntensity(float intensityPercent) {
    float t = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);

    // Map 0–100% → 100ms to 600ms delay
    float delaySec = 0.1f + t * 0.5f;
    delaySamples_ = static_cast<int>(delaySec * 44100.0f);

    echoGain_ = t * 0.6f;     // Max 60%
    decayFactor_ = 0.3f + t * 0.4f;  // 30% → 70%
    numEchoes_ = 1;

    Logger::getInstance().log("Echo: " + std::to_string(intensityPercent) +
                              "% → " + std::to_string(delaySec*1000) + "ms");
}

size_t Echo::apply(float* audioBuffer, size_t bufferSize) {
    std::vector<float> dry(audioBuffer, audioBuffer + bufferSize);

    int maxEchoes = std::max(3, numEchoes_); // at least 3 echoes

    for (int echo = 1; echo <= maxEchoes; ++echo) {
        float currentGain = echoGain_ * std::pow(decayFactor_, echo - 1);
        size_t currentDelay = delaySamples_ * echo;

        for (size_t i = currentDelay; i < bufferSize; ++i) {
            audioBuffer[i] += dry[i - currentDelay] * currentGain;
        }
    }

    // clamp to avoid clipping
    for (size_t i = 0; i < bufferSize; ++i)
        audioBuffer[i] = std::clamp(audioBuffer[i], -1.0f, 1.0f);
    
    return bufferSize;  // ADD THIS LINE!
}