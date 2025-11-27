#pragma once

#include <stdexcept>
#include <string>

/**
 * @brief Exception hierarchy for audio processing errors
 * 
 * Provides consistent error handling throughout the application.
 * Prefer these over bool return values for exceptional conditions.
 */
namespace audio {

/**
 * @brief Base exception for all audio-related errors
 */
class AudioException : public std::runtime_error {
public:
    explicit AudioException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Thrown when file loading fails
 */
class FileLoadException : public AudioException {
public:
    explicit FileLoadException(const std::string& filepath)
        : AudioException("Failed to load audio file: " + filepath)
        , filepath_(filepath) {}
    
    [[nodiscard]] const std::string& filepath() const noexcept { return filepath_; }

private:
    std::string filepath_;
};

/**
 * @brief Thrown when file saving fails
 */
class FileSaveException : public AudioException {
public:
    explicit FileSaveException(const std::string& filepath)
        : AudioException("Failed to save audio file: " + filepath)
        , filepath_(filepath) {}
    
    [[nodiscard]] const std::string& filepath() const noexcept { return filepath_; }

private:
    std::string filepath_;
};

/**
 * @brief Thrown when audio format is unsupported or invalid
 */
class FormatException : public AudioException {
public:
    explicit FormatException(const std::string& details)
        : AudioException("Invalid audio format: " + details) {}
};

/**
 * @brief Thrown when effect processing fails
 */
class EffectException : public AudioException {
public:
    explicit EffectException(const std::string& effectName, const std::string& details)
        : AudioException("Effect '" + effectName + "' failed: " + details)
        , effectName_(effectName) {}
    
    [[nodiscard]] const std::string& effectName() const noexcept { return effectName_; }

private:
    std::string effectName_;
};

/**
 * @brief Thrown when buffer operations fail
 */
class BufferException : public AudioException {
public:
    explicit BufferException(const std::string& details)
        : AudioException("Buffer error: " + details) {}
};

}  // namespace audio
