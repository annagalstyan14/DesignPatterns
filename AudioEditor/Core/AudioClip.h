#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Adapters/AudioFileAdapter.h"
#include "Effects/IEffect.h"
#include "Logging/ILogger.h"

class AudioClip {
public:
    explicit AudioClip(const std::string& filePath, std::shared_ptr<ILogger> logger);
    ~AudioClip() = default;

    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;

    AudioClip(AudioClip&&) = default;
    AudioClip& operator=(AudioClip&&) = default;
    [[nodiscard]] bool load();
    [[nodiscard]] bool save(const std::string& outputPath);
    void addEffect(std::shared_ptr<IEffect> effect);
    void applyEffects();
    void clearEffects() noexcept { effects_.clear(); }
    [[nodiscard]] const std::vector<float>& getSamples() const noexcept { 
        return samples_; 
    }
    [[nodiscard]] std::vector<float>& getSamplesRef() noexcept { 
        return samples_; 
    }
    void setSamples(std::vector<float> samples);
    [[nodiscard]] bool isLoaded() const noexcept { return isLoaded_; }
    [[nodiscard]] const std::string& getFilePath() const noexcept { return filePath_; }

private:
    std::string filePath_;
    std::unique_ptr<AudioFileAdapter> audioFile_;
    std::vector<float> samples_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    bool isLoaded_ = false;
    std::shared_ptr<ILogger> logger_;
};
