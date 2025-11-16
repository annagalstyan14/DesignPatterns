#pragma once
#include <memory>
#include <vector>
#include <string>
#include "AudioFileAdapter.h"
#include "IEffect.h"
#include "Effects/Speed.h"

class AudioClip {
public:
    explicit AudioClip(const std::string& filePath);
    bool load();
    bool save(const std::string& outputPath);
    void addEffect(std::shared_ptr<IEffect> effect);
    void applyEffects();
    std::vector<float> getSamples() const;

private:
    std::string filePath_;
    std::unique_ptr<AudioFileAdapter> audioFile_;
    std::vector<float> samples_;
    std::vector<std::shared_ptr<IEffect>> effects_;  // <-- ADD THIS
    bool isLoaded_ = false;
};