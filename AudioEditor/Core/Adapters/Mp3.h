#pragma once
#include "AudioFileAdapter.h"
#include <vector>

class Mp3Adapter : public AudioFileAdapter {
private:
    std::vector<float> samples;
    float duration = 0.0f;
public:
    bool load(const std::string& filePath) override;
    std::vector<float> getSamples() const override;
    float getDuration() const override;
    bool save(const std::string& filePath) override;
};