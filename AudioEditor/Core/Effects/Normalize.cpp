#include "Normalize.h"
#include <cmath>
#include <algorithm>
NormalizeEffect::NormalizeEffect(std::shared_ptr<ILogger> logger, float targetRMS)
: logger_(logger), targetRMS_(targetRMS), targetPeak_(0.95f) {
}
float NormalizeEffect::calculateRMS(const float* buffer, size_t size) {
double sumSquares = 0.0;
for (size_t i = 0; i < size; ++i) {
sumSquares += buffer[i] * buffer[i];
}
return std::sqrt(sumSquares / size);
}
float NormalizeEffect::calculatePeak(const float* buffer, size_t size) {
float peak = 0.0f;
for (size_t i = 0; i < size; ++i) {
peak = std::max(peak, std::abs(buffer[i]));
}
return peak;
}
size_t NormalizeEffect::apply(float* audioBuffer, size_t bufferSize) {
if (bufferSize == 0) return 0;
float currentRMS = calculateRMS(audioBuffer, bufferSize);
float currentPeak = calculatePeak(audioBuffer, bufferSize);
logger_->log("Before normalize - RMS: " + std::to_string(currentRMS) +
", Peak: " + std::to_string(currentPeak));
float rmsGain = (currentRMS > 0.0001f) ? (targetRMS_ / currentRMS) : 1.0f;
for (size_t i = 0; i < bufferSize; ++i) {
audioBuffer[i] *= rmsGain;
}
float newPeak = calculatePeak(audioBuffer, bufferSize);
if (newPeak > targetPeak_) {
float peakGain = targetPeak_ / newPeak;
for (size_t i = 0; i < bufferSize; ++i) {
audioBuffer[i] *= peakGain;
}
logger_->log("Clipping prevented - applied limiter (gain: " +
std::to_string(peakGain) + "x)");
}
float finalRMS = calculateRMS(audioBuffer, bufferSize);
float finalPeak = calculatePeak(audioBuffer, bufferSize);
logger_->log("After normalize - RMS: " + std::to_string(finalRMS) +
", Peak: " + std::to_string(finalPeak) +
", Gain applied: " + std::to_string(rmsGain));
return bufferSize;
}