#pragma once

#include "AudioFileAdapter.h"
#include "../Logging/ILogger.h"
#include "../Constants.h"
#include <memory>

/**
 * @brief MP3 file adapter using mpg123 for decoding and LAME for encoding
 * 
 * Design Pattern: Adapter (ConcreteAdapter)
 * - Adaptee: mpg123 library, LAME library
 * - Target: AudioFileAdapter interface
 */
class Mp3Adapter : public AudioFileAdapter {
public:
    explicit Mp3Adapter(std::shared_ptr<ILogger> logger);
    ~Mp3Adapter() override = default;
    
    // Non-copyable (owns resources)
    Mp3Adapter(const Mp3Adapter&) = delete;
    Mp3Adapter& operator=(const Mp3Adapter&) = delete;
    
    // Movable
    Mp3Adapter(Mp3Adapter&&) = default;
    Mp3Adapter& operator=(Mp3Adapter&&) = default;
    
    [[nodiscard]] bool load(const std::string& filePath) override;
    [[nodiscard]] bool save(const std::string& filePath, 
                            const std::vector<float>& samples) override;
    
    [[nodiscard]] const std::vector<float>& getSamples() const override { 
        return samples_; 
    }
    
    [[nodiscard]] float getDuration() const noexcept override { 
        return duration_; 
    }
    
    [[nodiscard]] int getSampleRate() const noexcept override { 
        return sampleRate_; 
    }
    
    [[nodiscard]] int getChannels() const noexcept override { 
        return channels_; 
    }
    
private:
    std::vector<float> samples_;
    float duration_ = 0.0f;
    int sampleRate_ = audio::kDefaultSampleRate;
    int channels_ = audio::kDefaultChannels;
    std::shared_ptr<ILogger> logger_;
};
