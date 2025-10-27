#ifndef SPEED_H
#define SPEED_H

#include "IEffect.h"
#include <vector>

class SpeedChangeEffect : public IEffect {
public:
    explicit SpeedChangeEffect(float speedFactor);
    void apply(float* audioBuffer, size_t bufferSize) override;
    void setSpeedFactor(float speedFactor);

private:
    float speedFactor_;
};

#endif // SPEED_H