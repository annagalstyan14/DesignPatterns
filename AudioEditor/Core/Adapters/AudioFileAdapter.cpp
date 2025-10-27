#include "AudioFileAdapter.h"

bool AudioFileAdapter::load(const std::string& filePath) {
    return false;
}

std::vector<float> AudioFileAdapter::getSamples() const {
    return {};
}

float AudioFileAdapter::getDuration() const {
    return 0.0f;
}

bool AudioFileAdapter::save(const std::string& filePath, const std::vector<float, std::allocator<float>>& processedSamples) {
    return false;
}