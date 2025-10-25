#include "Echo.h"
#include "../Logger.h"

void Echo::apply(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Echo Effect");
    
    float echoGain = 0.35f;
    int delaySamples = 6615;
    int numEchoes = 3;
    float decayFactor = 0.5f;
    
    // Assuming stereo (interleaved: left, right, left, right, ...)
    for (int echo = 1; echo <= numEchoes; ++echo) {
        float currentGain = echoGain * std::pow(decayFactor, echo - 1);
        size_t currentDelay = delaySamples * echo * 2;
        Logger::getInstance().log("Applying echo " + std::to_string(echo) + " with gain " + std::to_string(currentGain) + " and delay " + std::to_string(currentDelay / 2) + " samples");
        for (size_t i = currentDelay; i < bufferSize; i += 2) {
            // Left channel
            audioBuffer[i] += audioBuffer[i - currentDelay] * currentGain;
            // Right channel
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