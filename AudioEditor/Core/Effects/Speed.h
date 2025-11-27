#ifndef SPEED_H
#define SPEED_H

#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <vector>
#include <memory>

class SpeedChangeEffect : public IEffect {
public:
    SpeedChangeEffect(float speedFactor, std::shared_ptr<ILogger> logger);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setSpeedFactor(float speedFactor);
    float getSpeedFactor() const;
    void setParameter(const std::string& name, float value) override {
        if (name == "speed") setSpeedFactor(value);
    }

private:
    float speedFactor_;
    std::shared_ptr<ILogger> logger_;
};

#endif // SPEED_H