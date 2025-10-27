#ifndef MP3_ADAPTER_H
#define MP3_ADAPTER_H

#include "AudioFileAdapter.h"
#include <string>
#include <vector>

class Mp3Adapter : public AudioFileAdapter {
public:
    Mp3Adapter();
    ~Mp3Adapter() override;
    bool load(const std::string& filePath) override;
    bool save(const std::string& filePath, const std::vector<float, std::allocator<float>>& processedSamples) override;
    std::vector<float> getSamples() const override;
    float getDuration() const override;

private:
    std::vector<float> samples_;
    float duration_ = 0.0f;
    long inputRate = 44100;
    int channels = 2;
};

#endif // MP3_ADAPTER_H