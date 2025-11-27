#pragma once
#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <string>
class NormalizeEffect : public IEffect {
public:
explicit NormalizeEffect(std::shared_ptr<ILogger> logger, float targetRMS = 0.15f);
size_t apply(float* audioBuffer, size_t bufferSize) override;
void setTargetRMS(float rms) { targetRMS_ = rms; }
void setTargetPeak(float peak) { targetPeak_ = peak; }
void setParameter(const std::string& name, float value) override {
if (name == "targetRMS") setTargetRMS(value);
else if (name == "targetPeak") setTargetPeak(value);
}
private:
std::shared_ptr<ILogger> logger_;
float targetRMS_;
float targetPeak_;
float calculateRMS(const float* buffer, size_t size);
float calculatePeak(const float* buffer, size_t size);
};

