#include "AudioClip.h"
#include "Adapters/Mp3.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

AudioClip::AudioClip(const std::string& filePath) : filePath_(filePath) {
    audioFile_ = std::make_unique<Mp3Adapter>();
}

bool AudioClip::load() {
    if (!audioFile_->load(filePath_)) {
        Logger::getInstance().log("Failed to load file: " + filePath_);
        return false;
    }

    samples_ = audioFile_->getSamples();  // <-- FIXED: samples_
    isLoaded_ = true;
    Logger::getInstance().log("Loaded " + std::to_string(samples_.size()) + " samples from " + filePath_);
    return true;
}

bool AudioClip::save(const std::string& outputPath) {
    if (!isLoaded_ || samples_.empty()) {  // <-- FIXED
        Logger::getInstance().log("Cannot save: no samples loaded");
        return false;
    }

    // Log stats before save
    double sumSquaresBefore = 0.0;
    for (const auto& sample : samples_) {  // <-- FIXED
        sumSquaresBefore += sample * sample;
    }
    double rmsBefore = std::sqrt(sumSquaresBefore / samples_.size());
    Logger::getInstance().log("Saving samples - Max sample: " + std::to_string(*std::max_element(samples_.begin(), samples_.end())) +
                              ", RMS: " + std::to_string(rmsBefore));

    return audioFile_->save(outputPath, samples_);  // <-- FIXED
}

void AudioClip::addEffect(std::shared_ptr<IEffect> effect) {
    if (effect) {
        effects_.push_back(effect);  // <-- FIXED: effects_ is now a vector
    }
}

void AudioClip::applyEffects() {
    if (!isLoaded_ || samples_.empty()) return;

    // Log before
    double sumSquaresBefore = std::accumulate(samples_.begin(), samples_.end(), 0.0,
        [](double acc, float s) { return acc + s * s; });
    double rmsBefore = std::sqrt(sumSquaresBefore / samples_.size());
    Logger::getInstance().log("Before normalization - Max sample: " + std::to_string(*std::max_element(samples_.begin(), samples_.end())) +
                              ", RMS: " + std::to_string(rmsBefore));

    // Optional: normalize
    float maxSample = *std::max_element(samples_.begin(), samples_.end(), 
        [](float a, float b) { return std::abs(a) < std::abs(b); });
    if (maxSample > 1.0f) {
        for (auto& sample : samples_) {
            sample /= maxSample;
        }
        Logger::getInstance().log("After normalization - Max sample: 1.0");
    }

    // Apply effects
    for (auto& effect : effects_) {
        // Check if this is a speed effect and pre-allocate space
        if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
            float factor = speed->getSpeedFactor();
            size_t oldSize = samples_.size();
            size_t newSize = static_cast<size_t>(oldSize / factor);
            
            // Only resize if we need MORE space (slowing down)
            if (newSize > oldSize) {
                samples_.resize(newSize);
            }
            
            Logger::getInstance().log("Speed " + std::to_string(factor) + "x: pre-allocating from " +
                                      std::to_string(oldSize) + " to " + std::to_string(newSize) + " samples");
        }
        
        // Apply the effect and get the actual new size
        size_t actualNewSize = effect->apply(samples_.data(), samples_.size());
        
        // Resize to the actual size returned by the effect
        samples_.resize(actualNewSize);
        
        Logger::getInstance().log("After effect: buffer size is now " + std::to_string(actualNewSize) + " samples");
    }

    // Log after
    double sumSquaresAfter = std::accumulate(samples_.begin(), samples_.end(), 0.0,
        [](double acc, float s) { return acc + s * s; });
    double rmsAfter = std::sqrt(sumSquaresAfter / samples_.size());
    Logger::getInstance().log("After effects - Max sample: " + std::to_string(*std::max_element(samples_.begin(), samples_.end())) +
                              ", RMS: " + std::to_string(rmsAfter));
}

std::vector<float> AudioClip::getSamples() const {
    return samples_;  // <-- FIXED
}