#include "Reverb.h"
#include "../Logger.h"
#include <vector>
#include <algorithm>
#include <cmath>

Reverb::Reverb() : reverbGain(0.4f), decayFactor(0.6f), baseDelaySamples(1500), lowPass(0.8f) 
{
    int delays[] = {1493, 1733, 2111, 2333, 2579};
    for (int d : delays)
        delayLines.push_back({std::vector<float>(d, 0.0f), 0});
}

void Reverb::setIntensity(float intensityPercent){
    float t = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
    reverbGain = 0.4f + t* 0.6f;
    decayFactor = 0.6f + t* 0.3f;
    lowPass = 0.8f + t * 0.15f;
}

size_t Reverb::apply(float* buffer, size_t bufferSize){  // Rename parameter!
    Logger::getInstance().log("Applying Reverb: ");

    for (size_t i = 0; i < bufferSize; ++i){
        float input = buffer[i];  // Use 'buffer' not 'audioBuffer'
        float reverbOut = 0.0f;

        for (auto& d : delayLines){
            float delayed = d.audioBuffer[d.index];  // This is the struct member
            reverbOut += delayed;

            float feedback = input + delayed * decayFactor;
            d.audioBuffer[d.index] = d.audioBuffer[d.index] * (1- lowPass) + feedback * lowPass;

            d.index = (d.index + 1) % d.audioBuffer.size();
        }

        buffer[i] = input * (1.0f - reverbGain) + reverbOut * (reverbGain / delayLines.size());
        buffer[i] = std::clamp(buffer[i], -1.0f, 1.0f);
    }
    
    return bufferSize;  // Add this!
}