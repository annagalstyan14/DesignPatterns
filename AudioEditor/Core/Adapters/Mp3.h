#ifndef MP3_H
#define MP3_H

#include "AudioFileAdapter.h"
#include <vector>
#include <string>

class Mp3Adapter : public AudioFileAdapter {
public:
    bool load(const std::string& filePath) override;
    bool save(const std::string& filePath, const std::vector<float>& processedSamples) override;
    std::vector<float> getSamples() const override;
    float getDuration() const override;
private:
    std::vector<float> samples;
    float duration = 0.0f;
    long sampleRate = 44100;
    int channels = 2;
};

#endif // MP3_H