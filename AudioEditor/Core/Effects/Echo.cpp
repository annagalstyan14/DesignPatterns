#include "Echo.h"
#include "../Logger.h"

void Echo::applyEffect(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Echo Effect");
    
    // Simple echo: add delayed copy of the signal
    float echoGain = 0.5f;     // How much echo to add
    int delaySamples = 2205;    // ~50ms delay at 44.1kHz
    
    for (size_t i = delaySamples; i < bufferSize; ++i) {
        // Add delayed signal
        audioBuffer[i] += audioBuffer[i - delaySamples] * echoGain;
        
        // Prevent clipping
        if (audioBuffer[i] > 1.0f) audioBuffer[i] = 1.0f;
        if (audioBuffer[i] < -1.0f) audioBuffer[i] = -1.0f;
    }
}