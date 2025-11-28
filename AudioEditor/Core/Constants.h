#pragma once

/**
 * @brief Central location for all compile-time constants
 * 
 * Eliminates magic numbers throughout the codebase,
 * making the code self-documenting and easy to tune.
 */
namespace audio {

// Audio format defaults
constexpr int kDefaultSampleRate = 44100;
constexpr int kDefaultChannels = 2;
constexpr int kBitsPerSample = 16;
constexpr float kMaxSampleValue = 32767.0f;
constexpr float kSampleNormalizationFactor = 32768.0f;

// Buffer sizes
constexpr size_t kMp3ReadBufferSize = 4096;
constexpr size_t kMp3WriteBufferMultiplier = 7200;

// Reverb effect parameters
namespace reverb {
    // Pre-delay: ~20ms at 44.1kHz
    constexpr int kPreDelayMs = 20;
    constexpr int kPreDelaySamples = (kPreDelayMs * kDefaultSampleRate) / 1000;  // 882
    
    // Delay line lengths (prime numbers to avoid resonance)
    constexpr int kDelayLineLengths[] = {1557, 1617, 1491, 1422, 1277, 1356};
    constexpr size_t kNumDelayLines = 6;
    
    // Parameter ranges
    constexpr float kDefaultWetMix = 0.3f;
    constexpr float kDefaultRoomSize = 0.5f;
    constexpr float kDefaultDamping = 0.3f;
    constexpr float kDefaultFeedback = 0.7f;
    constexpr float kMaxWetMix = 1.0f;
    constexpr float kSoftClipThreshold = 0.8f;
}

// Speed effect parameters
namespace speed {
    constexpr float kMinSpeedFactor = 0.1f;
    constexpr float kMaxSpeedFactor = 2.0f;
    constexpr float kDefaultSpeedFactor = 1.0f;
}

// Volume effect parameters
namespace volume {
    constexpr float kMinGain = 0.0f;
    constexpr float kMaxGain = 2.0f;
    constexpr float kDefaultGain = 1.0f;
    constexpr float kSoftClipThreshold = 1.0f;
}

// Normalize effect parameters
namespace normalize {
    constexpr float kDefaultTargetRMS = 0.15f;
    constexpr float kDefaultTargetPeak = 0.95f;
    constexpr float kMinRMSThreshold = 0.0001f;
}

// UI timing constants
namespace ui {
    constexpr int kPreviewDebounceMs = 150;
    constexpr int kPositionUpdateMs = 50;
    constexpr int kParameterDebounceMs = 80;
    constexpr int kStatusMessageDurationMs = 3000;
    constexpr int kStatusMessageShortMs = 1500;
    constexpr int kStatusMessageLongMs = 5000;
}

// Waveform display
namespace waveform {
    constexpr float kMinZoom = 1.0f;
    constexpr float kMaxZoom = 50.0f;
    constexpr float kZoomFactor = 1.2f;
    constexpr float kAutoScrollThreshold = 0.8f;
    constexpr float kAutoScrollPosition = 0.2f;
    constexpr int kPeakSamplingStep = 100;
    constexpr float kDisplayPadding = 0.9f;
}

}  // namespace audio
