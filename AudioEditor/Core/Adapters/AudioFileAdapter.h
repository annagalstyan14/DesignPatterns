#pragma once

#include <vector>
#include <string>

/**
 * @brief Adapter Pattern Interface for audio file I/O
 * 
 * Abstracts the underlying audio library (mpg123/LAME),
 * allowing different format adapters to be swapped in.
 * 
 * Design Pattern: Adapter (Target Interface)
 * - Adapter: Mp3Adapter (wraps mpg123/LAME)
 * - Client: AudioClip
 */
class AudioFileAdapter {
public:
    virtual ~AudioFileAdapter() = default;
    
    /**
     * @brief Load audio from file
     * 
     * @param filePath Path to audio file
     * @return true on success, false on failure
     */
    [[nodiscard]] virtual bool load(const std::string& filePath) = 0;
    
    /**
     * @brief Save audio to file
     * 
     * @param filePath Output file path
     * @param samples Audio samples to save
     * @return true on success, false on failure
     */
    [[nodiscard]] virtual bool save(const std::string& filePath, 
                                    const std::vector<float>& samples) = 0;
    
    /**
     * @brief Get loaded audio samples
     * 
     * @return Reference to internal sample buffer
     */
    [[nodiscard]] virtual const std::vector<float>& getSamples() const = 0;
    
    /**
     * @brief Get audio duration in seconds
     */
    [[nodiscard]] virtual float getDuration() const noexcept = 0;
    
    /**
     * @brief Get sample rate
     */
    [[nodiscard]] virtual int getSampleRate() const noexcept = 0;
    
    /**
     * @brief Get number of channels
     */
    [[nodiscard]] virtual int getChannels() const noexcept = 0;
};
