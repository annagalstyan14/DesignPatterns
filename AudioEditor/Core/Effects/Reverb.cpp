#include "Reverb.h"
#include "../Logger.h"
#include <cmath>
#include <algorithm>

Reverb::Reverb(float reverbTimeMs, float diffusionPercent, float decayPercent,
               float dryPercent, float wetPercent, float brightnessPercent,
               float sampleRate)
    : reverbTimeMs(std::clamp(reverbTimeMs, 100.0f, 5000.0f)),
      diffusion(std::clamp(diffusionPercent, 0.0f, 100.0f) / 100.0f),
      decay(std::clamp(decayPercent, 0.0f, 100.0f) / 100.0f),
      dryOut(std::clamp(dryPercent, 0.0f, 100.0f) / 100.0f),
      wetOut(std::clamp(wetPercent, 0.0f, 100.0f) / 100.0f),
      brightness(std::clamp(brightnessPercent, 0.0f, 100.0f) / 100.0f),
      sampleRate(std::max(sampleRate, 44100.0f))
{
    Logger::getInstance().log("Reverb Effect Initialized");

    // Standard Schroeder reverb delays (ms)
    const float combDelaysMs[4] = {29.7f, 37.1f, 41.1f, 43.7f};
    const float allpassDelaysMs[2] = {5.0f, 1.7f};

    // Initialize Comb filters
    for (float delayMs : combDelaysMs) {
        CombFilter comb;
        comb.delaySamples = static_cast<size_t>(delayMs * sampleRate / 1000.0f);
        comb.buffer.assign(comb.delaySamples, 0.0f);
        comb.index = 0;
        comb.feedback = 0.7f * decay; 
        comb.damping = 0.3f + 0.5f * (1.0f - brightness);
        comb.lowpassState = 0.0f;
        combFilters.push_back(comb);
    }

    // Initialize All-pass filters
    for (float delayMs : allpassDelaysMs) {
        AllPassFilter ap;
        ap.delaySamples = static_cast<size_t>(delayMs * sampleRate / 1000.0f);
        ap.buffer.assign(ap.delaySamples, 0.0f);
        ap.index = 0;
        ap.feedback = 0.5f + 0.3f * diffusion; 
        allPassFilters.push_back(ap);
    }
}

void Reverb::apply(float* audioBuffer, size_t bufferSize) {
    if (!audioBuffer || bufferSize == 0) return;

    // Process interleaved stereo buffer
    for (size_t i = 0; i < bufferSize; i += 2) {
        float left = audioBuffer[i];
        float right = (i + 1 < bufferSize) ? audioBuffer[i + 1] : left;

        // Mono input for reverb processing
        float input = 0.5f * (left + right);

        // Parallel comb filters
        float combOut = 0.0f;
        for (auto& comb : combFilters) {
            float delayed = comb.buffer[comb.index];
            comb.lowpassState = (1.0f - comb.damping) * delayed + comb.damping * comb.lowpassState;
            comb.buffer[comb.index] = input + comb.lowpassState * comb.feedback;
            comb.index = (comb.index + 1) % comb.buffer.size();
            combOut += delayed;
        }
        combOut /= combFilters.size();

        // Series all-pass filters
        float apOut = combOut;
        for (auto& ap : allPassFilters) {
            float bufOut = ap.buffer[ap.index];
            float x = apOut + (-ap.feedback) * bufOut;
            ap.buffer[ap.index] = x;
            apOut = bufOut + ap.feedback * x;
            ap.index = (ap.index + 1) % ap.buffer.size();
        }

        // Mix dry and wet
        float outL = dryOut * left + wetOut * apOut;
        float outR = dryOut * right + wetOut * apOut;

        // Clamp output
        audioBuffer[i]     = std::clamp(outL, -1.0f, 1.0f);
        if (i + 1 < bufferSize)
            audioBuffer[i + 1] = std::clamp(outR, -1.0f, 1.0f);
    }
}
