#include "AudioClip.h"
#include "Adapters/Mp3.h"
#include "Effects/Normalize.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>

AudioClip::AudioClip(const std::string& filePath, std::shared_ptr<ILogger> logger) 
    : filePath_(filePath), logger_(logger) {
    audioFile_ = std::make_unique<Mp3Adapter>(logger);
}

bool AudioClip::load() {
    if (!audioFile_->load(filePath_)) {
        logger_->error("Failed to load file: " + filePath_);
        return false;
    }

    samples_ = audioFile_->getSamples();
    isLoaded_ = true;
    logger_->log("Loaded " + std::to_string(samples_.size()) + " samples from " + filePath_);
    return true;
}

bool AudioClip::save(const std::string& outputPath) {
    if (!isLoaded_ || samples_.empty()) {
        logger_->error("Cannot save: no samples loaded");
        return false;
    }

    // Log stats before save
    double sumSquaresBefore = 0.0;
    for (const auto& sample : samples_) {
        sumSquaresBefore += sample * sample;
    }
    double rmsBefore = std::sqrt(sumSquaresBefore / samples_.size());
    logger_->log("Saving samples - Max sample: " + 
                 std::to_string(*std::max_element(samples_.begin(), samples_.end())) +
                 ", RMS: " + std::to_string(rmsBefore));

    return audioFile_->save(outputPath, samples_);
}

void AudioClip::addEffect(std::shared_ptr<IEffect> effect) {
    if (effect) {
        effects_.push_back(effect);
    }
}

void AudioClip::applyEffects() {
    if (!isLoaded_ || samples_.empty()) return;

    // Log before
    double sumSquaresBefore = std::accumulate(samples_.begin(), samples_.end(), 0.0,
        [](double acc, float s) { return acc + s * s; });
    double rmsBefore = std::sqrt(sumSquaresBefore / samples_.size());
    
    logger_->log("Before effects - RMS: " + std::to_string(rmsBefore));

    // Apply all effects (reverb, echo, speed, etc.)
    for (auto& effect : effects_) {
        if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
            float factor = speed->getSpeedFactor();
            size_t oldSize = samples_.size();
            size_t newSize = static_cast<size_t>(oldSize / factor);
            
            if (newSize > oldSize) {
                samples_.resize(newSize);
            }
        }
        
        size_t actualNewSize = effect->apply(samples_.data(), samples_.size());
        samples_.resize(actualNewSize);
    }

    // ALWAYS normalize at the end to match original loudness
    auto normalizer = std::make_shared<NormalizeEffect>(logger_, rmsBefore);
    normalizer->apply(samples_.data(), samples_.size());

    // Log after
    double sumSquaresAfter = std::accumulate(samples_.begin(), samples_.end(), 0.0,
        [](double acc, float s) { return acc + s * s; });
    double rmsAfter = std::sqrt(sumSquaresAfter / samples_.size());
    
    logger_->log("After effects - RMS: " + std::to_string(rmsAfter));
}

std::vector<float> AudioClip::getSamples() const {
    return samples_;
}

void AudioClip::setSamples(const std::vector<float>& samples) {
    samples_ = samples;
    logger_->log("Samples updated: " + std::to_string(samples_.size()) + " samples");
}