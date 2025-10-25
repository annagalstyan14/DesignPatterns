#include "AudioClip.h"
#include "Adapters/Wav.h"
#include "Adapters/Mp3.h"

AudioClip::AudioClip(const std::string& path) : filePath(path) {
    // Use WavAdapter for .wav files, Mp3Adapter for .mp3 files
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".wav") {
        audioFile = std::make_shared<WavAdapter>();
    } else if (path.length() >= 4 && path.substr(path.length() - 4) == ".mp3") {
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

void AudioClip::applyEffects() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isLoaded || samples.empty()) return;
    
    Logger::getInstance().log("Applying " + std::to_string(effects.size()) + " effects to clip: " + filePath);
    
    // Apply each effect to the audio samples
    for (auto& effect : effects) {
        if (effect) {
            effect->applyEffect(samples.data(), samples.size());
        }
    }
}

void AudioClip::mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isLoaded || samples.empty()) return;
    
    // Simple mixing: copy mono samples to stereo output
    // This is a basic implementation - in a real audio editor you'd have more sophisticated mixing
    for (unsigned long i = 0; i < framesPerBuffer && i < samples.size(); ++i) {
        float sample = samples[i];
        outputBuffer[i * 2] += sample;     // Left channel
        outputBuffer[i * 2 + 1] += sample; // Right channel
    }
}

bool AudioClip::isClipLoaded() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return isLoaded;
}

std::vector<float> AudioClip::getSamples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return samples;
}