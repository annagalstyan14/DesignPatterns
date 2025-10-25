#include "Reverb.h"
#include "../Logger.h"

void Reverb::apply(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying Reverb Effect");
    
    float reverbGain = 0.25f;
    int delaySamples = 3308;
    
    // Assuming stereo (interleaved: left, right, left, right, ...)
    for (size_t i = delaySamples * 2; i < bufferSize; i += 2) {
        // Left channel
        audioBuffer[i] += audioBuffer[i - delaySamples * 2] * reverbGain;
        // Right channel
        if (i + 1 < bufferSize) {
            audioBuffer[i + 1] += audioBuffer[i + 1 - delaySamples * 2] * reverbGain;
        }
        
        if (audioBuffer[i] > 1.0f) audioBuffer[i] = 1.0f;
        if (audioBuffer[i] < -1.0f) audioBuffer[i] = -1.0f;
        if (i + 1 < bufferSize) {
            if (audioBuffer[i + 1] > 1.0f) audioBuffer[i + 1] = 1.0f;
            if (audioBuffer[i + 1] < -1.0f) audioBuffer[i + 1] = -1.0f;
        }
    }
}