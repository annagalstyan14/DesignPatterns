#include "AudioFileAdapter.h"

AudioFileAdapter::~AudioFileAdapter() = default;

bool AudioFileAdapter::load(const std::string& filePath) {
    return false;
}

std::vector<float> AudioFileAdapter::getSamples(){
    return {};
}

float AudioFileAdapter::getDuration(){
    return 0.0f;
}

bool AudioFileAdapter::save(const std::string& filePath) {
    return false;
}

