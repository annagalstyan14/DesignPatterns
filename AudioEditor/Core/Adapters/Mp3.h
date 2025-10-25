#pragma once
#include "AudioFileAdapter.h"
#include <vector>

class Mp3Adapter : public AudioFileAdapter {
public:
    bool load(const std::string& filePath) override;
    std::vector<float> getSamples() const;
    float getDuration() const;
    bool save(const std::string& filePath) override;
};