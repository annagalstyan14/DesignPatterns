#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Adapters/AudioFileAdapter.h"
#include "Effects/IEffect.h"
#include "Logging/ILogger.h"

/**
 * @brief Represents a loaded audio file with effects
 * 
 * Manages audio samples and a chain of effects to apply.
 */
class AudioClip {
public:
    explicit AudioClip(const std::string& filePath, std::shared_ptr<ILogger> logger);
    ~AudioClip() = default;
    
    // Non-copyable (owns large sample buffer)
    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;
    
    // Movable
    AudioClip(AudioClip&&) = default;
    AudioClip& operator=(AudioClip&&) = default;
    
    /**
     * @brief Load the audio file
     * @return true on success
     */
    [[nodiscard]] bool load();
    
    /**
     * @brief Save to output file
     * @return true on success
     */
    [[nodiscard]] bool save(const std::string& outputPath);
    
    /**
     * @brief Add an effect to the processing chain
     */
    void addEffect(std::shared_ptr<IEffect> effect);
    
    /**
     * @brief Apply all effects in the chain
     */
    void applyEffects();
    
    /**
     * @brief Clear all effects from the chain
     */
    void clearEffects() noexcept { effects_.clear(); }
    
    /**
     * @brief Get audio samples (const reference - no copy)
     */
    [[nodiscard]] const std::vector<float>& getSamples() const noexcept { 
        return samples_; 
    }
    
    /**
     * @brief Get mutable access to samples (use sparingly)
     */
    [[nodiscard]] std::vector<float>& getSamplesRef() noexcept { 
        return samples_; 
    }
    
    /**
     * @brief Replace samples (for undo/redo)
     */
    void setSamples(std::vector<float> samples);
    
    /**
     * @brief Check if audio is loaded
     */
    [[nodiscard]] bool isLoaded() const noexcept { return isLoaded_; }
    
    /**
     * @brief Get file path
     */
    [[nodiscard]] const std::string& getFilePath() const noexcept { return filePath_; }

private:
    std::string filePath_;
    std::unique_ptr<AudioFileAdapter> audioFile_;
    std::vector<float> samples_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    bool isLoaded_ = false;
    std::shared_ptr<ILogger> logger_;
};
