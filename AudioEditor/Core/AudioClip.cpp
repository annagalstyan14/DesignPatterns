#include "AudioClip.h"
#include "Adapters/Mp3.h"
#include <algorithm>
#include <numeric>

AudioClip::AudioClip(const std::string& path) : filePath(path) {
    if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".mp3") == 0) {
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
        Logger::getInstance().log("Loaded " + std::to_string(samples.size()) + " samples from " + filePath);
    }
    return isLoaded;
}

void AudioClip::play() {
    // Not needed for minimal version
}

void AudioClip::stop() {
    // Not needed for minimal version
}

void AudioClip::mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer) {
    // Not needed for minimal version
}

void AudioClip::applyEffects() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isLoaded || samples.empty()) {
        Logger::getInstance().log("Cannot apply effects: no samples loaded for " + filePath);
        return;
    }
n
    float maxSampleBefore = 0.0f;
    double sumSquaresBefore = 0.0;
    for (const auto& sample : samples) {
        maxSampleBefore = std::max(maxSampleBefore, std::abs(sample));
        sumSquaresBefore += sample * sample;
    }
    double rmsBefore = std::sqrt(sumSquaresBefore / samples.size());
    Logger::getInstance().log("Before normalization - Max sample: " + std::to_string(maxSampleBefore) + ", RMS: " + std::to_string(rmsBefore));

    if (maxSampleBefore > 0.0f) {
        float scale = 0.5f / maxSampleBefore; 
        for (auto& sample : samples) {
            sample *= scale;
        }
        Logger::getInstance().log("After normalization - Max sample: 0.5");
    }

    for (const auto& effect : effects) {
        if (effect) {
            effect->apply(samples.data(), samples.size());
        }
    }

    float maxSampleAfter = 0.0f;
    double sumSquaresAfter = 0.0;
    for (const auto& sample : samples) {
        maxSampleAfter = std::max(maxSampleAfter, std::abs(sample));
        sumSquaresAfter += sample * sample;
    }
    double rmsAfter = std::sqrt(sumSquaresAfter / samples.size());
    Logger::getInstance().log("After effects - Max sample: " + std::to_string(maxSampleAfter) + ", RMS: " + std::to_string(rmsAfter));
    Logger::getInstance().log("Effects applied to clip: " + filePath);
}

bool AudioClip::save(const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isLoaded) {
        Logger::getInstance().log("Cannot save unloaded clip: " + filePath);
        return false;
    }
    return audioFile->save(outputPath, samples);
}

float AudioClip::getDuration() const { return duration; }
float AudioClip::getStartTime() const { return startTime; }
void AudioClip::setStartTime(float time) { startTime = time; }
void AudioClip::addEffect(std::shared_ptr<IEffect> effect) {
    std::lock_guard<std::mutex> lock(mutex_);
    effects.push_back(effect);
    Logger::getInstance().log("Effect added to clip: " + filePath);
}
bool AudioClip::isClipLoaded() const { return isLoaded; }
std::vector<float> AudioClip::getSamples() const { return samples; }