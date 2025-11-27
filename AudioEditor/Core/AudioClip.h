#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Adapters/AudioFileAdapter.h"
#include "Effects/IEffect.h"
#include "Effects/Speed.h"
#include "ILogger.h"

class AudioClip {
public:
    explicit AudioClip(const std::string& filePath, std::shared_ptr<ILogger> logger);
    bool load();
    bool save(const std::string& outputPath);
    void addEffect(std::shared_ptr<IEffect> effect);
    void applyEffects();
    std::vector<float> getSamples() const;
    void setSamples(const std::vector<float>& samples);
    void clearEffects() { effects_.clear(); }

private:
    std::string filePath_;
    std::unique_ptr<AudioFileAdapter> audioFile_;
    std::vector<float> samples_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    bool isLoaded_ = false;
    std::shared_ptr<ILogger> logger_;
};