#pragma once

#include <string>
#include <vector>

/**
 * @brief Strategy Pattern Interface for audio effects
 * 
 * Each effect implements a different audio processing algorithm.
 * Effects are interchangeable and can be combined in chains.
 * 
 * Design Pattern: Strategy
 * - Context: AudioClip, AudioEngine
 * - ConcreteStrategies: Reverb, SpeedChangeEffect, VolumeEffect, NormalizeEffect
 * 
 * @note Uses vector reference instead of raw pointer for memory safety.
 *       The effect may resize the buffer (e.g., speed changes duration).
 */
class IEffect {
public:
    virtual ~IEffect() = default;
    
    /**
     * @brief Apply the effect to the audio buffer
     * 
     * @param audioBuffer The audio samples to process (modified in-place)
     *                    Effect may resize this buffer if needed
     * 
     * @note Thread-safety: Not thread-safe. Caller must ensure exclusive access.
     */
    virtual void apply(std::vector<float>& audioBuffer) = 0;
    
    /**
     * @brief Set a named parameter
     * 
     * @param name Parameter name (effect-specific)
     * @param value New parameter value
     */
    virtual void setParameter(const std::string& name, float value) = 0;
    
    /**
     * @brief Get the effect name for display/logging
     * 
     * @return Human-readable effect name
     */
    [[nodiscard]] virtual std::string getName() const noexcept = 0;
};
