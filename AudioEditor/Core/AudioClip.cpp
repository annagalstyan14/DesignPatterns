#include "AudioClip.h"
#include "Adapters/Mp3.h"
#include "Effects/Speed.h" // SpeedChangeEffect
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

void AudioClip::applyEffects() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isLoaded || samples.empty()) {
        Logger::getInstance().log("Cannot apply effects: no samples loaded");
        return;
    }

    // Ensure stereo buffer (duplicate last sample if odd)
    if (samples.size() % 2 != 0) {
        samples.push_back(samples.back());
    }

    Logger::getInstance().log("Applying effects to clip: " + filePath);

    // Apply each effect
    for (auto& effect : effects) {
        if (!effect) continue;

        if (auto speedEffect = std::dynamic_pointer_cast<SpeedChangeEffect>(effect)) {
            // Apply speed resampling
            speedEffect->apply(samples.data(), samples.size());
            auto& resampled = speedEffect->getResampledBuffer();
            samples = resampled;
            Logger::getInstance().log("Applied SpeedChangeEffect, new sample count = " + std::to_string(samples.size()));
        } else {
            // Apply other effects (Reverb, etc.)
            effect->apply(samples.data(), samples.size());
            // Log max sample to ensure effect is modifying audio
            float maxVal = 0.0f;
            for (auto s : samples) maxVal = std::max(maxVal, std::abs(s));
            Logger::getInstance().log("Effect applied: max abs sample = " + std::to_string(maxVal));
        }
    }

    // Clamp final output to [-1,1] to prevent clipping
    for (auto& s : samples) s = std::clamp(s, -1.0f, 1.0f);

    Logger::getInstance().log("All effects applied, final sample count = " + std::to_string(samples.size()));
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
    if (effect) effects.push_back(effect);
    Logger::getInstance().log("Effect added to clip: " + filePath);
}

bool AudioClip::isClipLoaded() const { return isLoaded; }
std::vector<float> AudioClip::getSamples() const { return samples; }
