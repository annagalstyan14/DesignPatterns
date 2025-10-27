#include "Speed.h"
#include <algorithm>
#include <cmath>
#include "../Logger.h"

SpeedChangeEffect::SpeedChangeEffect(float speedFactor) : speedFactor_(std::max(0.1f, std::min(2.0f, speedFactor))) {
    Logger::getInstance().log("SpeedChangeEffect initialized with factor " + std::to_string(speedFactor_));
}

void SpeedChangeEffect::setSpeedFactor(float speedFactor) {
    speedFactor_ = std::max(0.1f, std::min(2.0f, speedFactor));
    Logger::getInstance().log("Speed factor set to " + std::to_string(speedFactor_) + "x");
}

void SpeedChangeEffect::apply(float* audioBuffer, size_t bufferSize) {
    if (bufferSize == 0 || !audioBuffer || speedFactor_ == 1.0f) {
        return;
    }

    size_t newSize = static_cast<size_t>(bufferSize / speedFactor_);
    std::vector<float> tempBuffer(newSize, 0.0f);

    for (size_t i = 0; i < newSize && i * speedFactor_ < bufferSize; ++i) {
        size_t sourceIndex = static_cast<size_t>(i * speedFactor_);
        tempBuffer[i] = audioBuffer[sourceIndex];
    }

    std::copy(tempBuffer.begin(), tempBuffer.begin() + std::min(bufferSize, newSize), audioBuffer);
}