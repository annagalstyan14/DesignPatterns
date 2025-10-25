#include "Wav.h"
#include <sndfile.h>
#include "Logger.h"

bool WavAdapter::load(const std::string& filePath) {
    SF_INFO info;
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &info);
    if (!file) {
        Logger::getInstance().log("Failed to load WAV: " + filePath);
        return false;
    }
    samples.resize(info.frames * info.channels);
    sf_read_float(file, samples.data(), samples.size());
    duration = static_cast<float>(info.frames) / info.samplerate;
    sf_close(file);
    Logger::getInstance().log("Loaded WAV: " + filePath);
    return true;
}

std::vector<float> WavAdapter::getSamples() const { return samples; }
float WavAdapter::getDuration() const { return duration; }
bool WavAdapter::save(const std::string& filePath) {
    Logger::getInstance().log("Saving WAV: " + filePath);
    return true; // Placeholder
}