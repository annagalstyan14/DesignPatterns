#include "Speed.h"
#include "../Logger.h"
#include <cmath>
#include <algorithm>

SpeedChangeEffect::SpeedChangeEffect(float speedFactor, float sampleRate)
    : speedFactor(speedFactor), sampleRate(sampleRate) {
    Logger::getInstance().log("SpeedChangeEffect created with speed factor: " + std::to_string(speedFactor));
}

void SpeedChangeEffect::apply(float* audioBuffer, size_t bufferSize) {
    Logger::getInstance().log("Applying SpeedChange Effect with speed factor: " + std::to_string(speedFactor));
    
    if (bufferSize < 2 || !audioBuffer) {
        Logger::getInstance().log("SpeedChange Effect: Invalid buffer size or null buffer");
        return;
    }

    size_t newSize = static_cast<size_t>(bufferSize / speedFactor);
    newSize = (newSize < 2) ? 2 : newSize;
    newSize = (newSize % 2) ? newSize + 1 : newSize; // Ensure even for stereo
    resampledBuffer.resize(newSize, 0.0f);

    for (size_t i = 0; i < newSize; i += 2) {
        float srcIndex = i * speedFactor;
        size_t indexLow = static_cast<size_t>(std::floor(srcIndex));
        float frac = srcIndex - indexLow;

        // Ensure indices are safe for stereo (2 channels)
        if (indexLow >= bufferSize - 2) indexLow = bufferSize - 2;
        size_t indexHigh = indexLow + 2;
        if (indexHigh >= bufferSize) indexHigh = bufferSize - 2;

        // Left channel interpolation
        resampledBuffer[i] = audioBuffer[indexLow] * (1.0f - frac) + audioBuffer[indexHigh] * frac;

        // Right channel interpolation
        if (i + 1 < newSize) {
            resampledBuffer[i + 1] = audioBuffer[indexLow + 1] * (1.0f - frac) + audioBuffer[indexHigh + 1] * frac;
        }

        // Clamp samples
        resampledBuffer[i] = std::clamp(resampledBuffer[i], -1.0f, 1.0f);
        if (i + 1 < newSize) {
            resampledBuffer[i + 1] = std::clamp(resampledBuffer[i + 1], -1.0f, 1.0f);
        }
    }

    // Copy back to audioBuffer, handling size differences
    size_t copySize = std::min(newSize, bufferSize);
    std::copy(resampledBuffer.begin(), resampledBuffer.begin() + copySize, audioBuffer);
    if (newSize < bufferSize) {
        std::fill(audioBuffer + newSize, audioBuffer + bufferSize, 0.0f);
    }
}

const std::vector<float>& SpeedChangeEffect::getResampledBuffer() const {
    return resampledBuffer;
}

size_t SpeedChangeEffect::getResampledSize() const {
    return resampledBuffer.size();
}