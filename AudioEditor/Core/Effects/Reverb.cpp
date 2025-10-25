#include "Reverb.h"
#include "../Logger.h"

void Reverb::applyEffect(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Reverb Effect");
    
    // Simple reverb: add delayed and attenuated copies of the signal
    float reverbGain = 1.0f;  // How much reverb to add
    int delaySamples = 4410;   // ~100ms delay at 44.1kHz
    
    for (size_t i = delaySamples; i < bufferSize; ++i) {
        // Add delayed signal with reduced amplitude
        audioBuffer[i] += audioBuffer[i - delaySamples] * reverbGain;
        
        // Prevent clipping
        if (audioBuffer[i] > 1.0f) audioBuffer[i] = 1.0f;
        if (audioBuffer[i] < -1.0f) audioBuffer[i] = -1.0f;
    }
}