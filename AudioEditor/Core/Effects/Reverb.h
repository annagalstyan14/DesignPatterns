#pragma once
#include "IEffect.h"
#include "../Logging/ILogger.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <array>
#include <cmath>

/**
 * @brief Professional-quality reverb using Freeverb/Schroeder algorithm
 * 
 * This implementation mirrors the sound of Adobe After Effects reverb:
 * - 8 parallel comb filters with lowpass-filtered feedback
 * - 4 series all-pass filters for diffusion
 * - Separate early reflections
 * - Stereo decorrelation
 */
class Reverb : public IEffect {
public:
    explicit Reverb(std::shared_ptr<ILogger> logger);
    size_t apply(float* audioBuffer, size_t bufferSize) override;
    void setIntensity(float intensityPercent);
    void reset();
    
    void setWetMix(float wet) { wetMix_ = std::clamp(wet, 0.0f, 1.0f); }
    void setRoomSize(float size) { roomSize_ = std::clamp(size, 0.0f, 1.0f); updateParameters(); }
    void setDamping(float damp) { damping_ = std::clamp(damp, 0.0f, 1.0f); updateParameters(); }
    void setPreDelay(float ms) { preDelayMs_ = std::clamp(ms, 0.0f, 100.0f); }
    void setWidth(float w) { width_ = std::clamp(w, 0.0f, 1.0f); }
    
    void setParameter(const std::string& name, float value) override {
        if (name == "intensity") setIntensity(value * 100.0f);
        else if (name == "wetMix") setWetMix(value);
        else if (name == "roomSize") setRoomSize(value);
        else if (name == "damping") setDamping(value);
    }

private:
    static constexpr int NUM_COMBS = 8;
    static constexpr int NUM_ALLPASSES = 4;
    static constexpr int STEREO_SPREAD = 23;
    static constexpr int SAMPLE_RATE = 44100;

    float dcBlockL_ = 0.0f;
    float dcBlockR_ = 0.0f;
    
    // Freeverb tuning constants (at 44100Hz)
    static constexpr int COMB_TUNING[NUM_COMBS] = {
        1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617
    };
    static constexpr int ALLPASS_TUNING[NUM_ALLPASSES] = {
        556, 441, 341, 225
    };
    
    // Comb filter with lowpass in feedback
    struct CombFilter {
        std::vector<float> buffer;
        size_t bufferSize = 0;
        size_t index = 0;
        float filterStore = 0.0f;
        
        void init(size_t size) {
            bufferSize = size;
            buffer.resize(size, 0.0f);
            index = 0;
            filterStore = 0.0f;
        }
        
        float process(float input, float feedback, float damp1, float damp2) {
            float output = buffer[index];
            filterStore = (output * damp2) + (filterStore * damp1);
            buffer[index] = input + (filterStore * feedback);
            if (++index >= bufferSize) index = 0;
            return output;
        }
        
        void clear() {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            filterStore = 0.0f;
            index = 0;
        }
    };
    
    // All-pass filter for diffusion
    struct AllpassFilter {
        std::vector<float> buffer;
        size_t bufferSize = 0;
        size_t index = 0;
        
        void init(size_t size) {
            bufferSize = size;
            buffer.resize(size, 0.0f);
            index = 0;
        }
        
        float process(float input, float feedback = 0.5f) {
            float buffered = buffer[index];
            float output = -input + buffered;
            buffer[index] = input + (buffered * feedback);
            if (++index >= bufferSize) index = 0;
            return output;
        }
        
        void clear() {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            index = 0;
        }
    };
    
    // Early reflections using tapped delay line
    struct EarlyReflections {
        std::vector<float> buffer;
        size_t bufferSize = 0;
        size_t writeIndex = 0;
        
        static constexpr int NUM_TAPS = 7;
        std::array<size_t, NUM_TAPS> tapDelays;
        std::array<float, NUM_TAPS> tapGains;
        
        void init(int sampleRate) {
            bufferSize = static_cast<size_t>(sampleRate * 0.1);
            buffer.resize(bufferSize, 0.0f);
            writeIndex = 0;
            
            float tapTimesMs[NUM_TAPS] = {7.0f, 11.0f, 19.0f, 27.0f, 37.0f, 53.0f, 79.0f};
            float tapGainsDb[NUM_TAPS] = {-1.5f, -3.0f, -5.0f, -7.0f, -9.0f, -12.0f, -15.0f};
            
            for (int i = 0; i < NUM_TAPS; ++i) {
                tapDelays[i] = static_cast<size_t>((tapTimesMs[i] / 1000.0f) * sampleRate);
                tapGains[i] = std::pow(10.0f, tapGainsDb[i] / 20.0f);
            }
        }
        
        float process(float input) {
            buffer[writeIndex] = input;
            float output = 0.0f;
            for (int i = 0; i < NUM_TAPS; ++i) {
                size_t readIndex = (writeIndex + bufferSize - tapDelays[i]) % bufferSize;
                output += buffer[readIndex] * tapGains[i];
            }
            writeIndex = (writeIndex + 1) % bufferSize;
            return output;
        }
        
        void clear() {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            writeIndex = 0;
        }
    };
    
    std::array<CombFilter, NUM_COMBS> combsL_;
    std::array<CombFilter, NUM_COMBS> combsR_;
    std::array<AllpassFilter, NUM_ALLPASSES> allpassesL_;
    std::array<AllpassFilter, NUM_ALLPASSES> allpassesR_;
    EarlyReflections earlyL_;
    EarlyReflections earlyR_;
    
    std::vector<float> preDelayBufferL_;
    std::vector<float> preDelayBufferR_;
    size_t preDelayIndex_ = 0;
    size_t preDelaySamples_ = 0;
    
    float wetMix_ = 0.3f;
    float dryMix_ = 0.7f;
    float roomSize_ = 0.5f;
    float damping_ = 0.5f;
    float preDelayMs_ = 20.0f;
    float width_ = 1.0f;
    float earlyMix_ = 0.5f;
    
    float feedback_ = 0.7f;
    float damp1_ = 0.5f;
    float damp2_ = 0.5f;
    
    std::shared_ptr<ILogger> logger_;
    
    void updateParameters();
    void initializeFilters();
};