#pragma once
#include <string>
#include <vector>

class AudioFileAdapter {
public:
    virtual ~AudioFileAdapter() = default;
    virtual bool load(const std::string& filePath) = 0;
    virtual std::vector<float> getSamples() const = 0;
    virtual float getDuration() const = 0;
    virtual bool save(const std::string& filePath) = 0;
};