#include "AudioClip.h"
#include "Adapters/Wav.h"
#include "Adapters/Mp3.h"

AudioClip::AudioClip(const std::string& path) : filePath(path) {
    // Use WavAdapter for .wav files, Mp3Adapter for .mp3 files
    if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".wav") == 0) {
        audioFile = std::make_shared<WavAdapter>();
    } else if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".mp3") == 0) {
        audioFile = std::make_shared<Mp3Adapter>();
    } else {
        Logger::getInstance().log("Unsupported file format: " + path);
    }
}

bool AudioClip::load() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!audioFile) return false;
    isLoaded = audioFile->load(filePath);
    if (isLoaded) {
        samples = audioFile->getSamples();
        duration = audioFile->getDuration();
    }
    return isLoaded;
}

void AudioClip::play() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isLoaded) {
        Logger::getInstance().log("Playing clip: " + filePath);
    } else {
        Logger::getInstance().log("Cannot play unloaded clip: " + filePath);
    }
}

void AudioClip::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    Logger::getInstance().log("Stopping clip: " + filePath);
}

float AudioClip::getDuration() const { return duration; }
float AudioClip::getStartTime() const { return startTime; }
void AudioClip::setStartTime(float time) { startTime = time; }
void AudioClip::addEffect(std::shared_ptr<IEffect> effect) {
    std::lock_guard<std::mutex> lock(mutex_);
    effects.push_back(effect);
    Logger::getInstance().log("Effect added to clip: " + filePath);
}