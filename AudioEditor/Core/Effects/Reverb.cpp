#include "Reverb.h"
#include <algorithm>
#include <cmath>
// Tuned delay line lengths (in samples at 44.1kHz)
// These are carefully chosen prime numbers for optimal diffusion
static const int COMB_TUNINGS_L[] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
static const int COMB_TUNINGS_R[] = {1116+23, 1188+23, 1277+23, 1356+23, 1422+23, 1491+23, 1557+23, 1617+23};
static const int ALLPASS_TUNINGS_L[] = {556, 441, 341, 225};
static const int ALLPASS_TUNINGS_R[] = {556+23, 441+23, 341+23, 225+23};
static const int NUM_COMBS = 8;
static const int NUM_ALLPASS = 4;
        Reverb::Reverb(std::shared_ptr<ILogger> logger)
        : wetMix_(0.3f), roomSize_(0.5f), damping_(0.3f),  // Lower default damping for brightness
        feedback_(0.84f), preDelaySamples_(0), logger_(logger)
        {
        // Initialize comb filters (left channel)
        for (int i = 0; i < NUM_COMBS; ++i) {
        CombFilter comb;
        comb.buffer.resize(COMB_TUNINGS_L[i], 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        combFiltersL_.push_back(comb);
        }
        // Initialize comb filters (right channel - slightly detuned for stereo width)
        for (int i = 0; i < NUM_COMBS; ++i) {
        CombFilter comb;
        comb.buffer.resize(COMB_TUNINGS_R[i], 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        combFiltersR_.push_back(comb);
        }
        // Initialize all-pass filters (left)
        for (int i = 0; i < NUM_ALLPASS; ++i) {
        AllPassFilter ap;
        ap.buffer.resize(ALLPASS_TUNINGS_L[i], 0.0f);
        ap.index = 0;
        allPassFiltersL_.push_back(ap);
        }
        // Initialize all-pass filters (right)
        for (int i = 0; i < NUM_ALLPASS; ++i) {
        AllPassFilter ap;
        ap.buffer.resize(ALLPASS_TUNINGS_R[i], 0.0f);
        ap.index = 0;
        allPassFiltersR_.push_back(ap);
        }
        // Pre-delay (adjust size as needed, e.g., 20ms at 44.1kHz = 882 samples)
        preDelayBuffer_.resize(882, 0.0f);
        preDelaySamples_ = preDelayBuffer_.size();
        preDelayIndex_ = 0;
        // Echo buffers (for longer tails, e.g., 500ms delay)
        echoDelaySamples_ = 22050;  // 0.5s at 44.1kHz
        echoBufferL_.resize(echoDelaySamples_, 0.0f);
        echoBufferR_.resize(echoDelaySamples_, 0.0f);
        echoFeedback_ = 0.3f;
        echoMix_ = 0.2f;
        // Highpass coeff for ~150Hz cutoff at 44.1kHz
        highpassCoeff_ = std::exp(-2 * std::acos(-1.0) * 150 / 44100.0);
        updateParameters();
        }
        void Reverb::setIntensity(float intensityPercent) {
        float intensity = std::clamp(intensityPercent / 100.0f, 0.0f, 1.0f);
        wetMix_ = intensity * 0.5f;
        roomSize_ = 0.4f + intensity * 0.5f;
        damping_ = 0.1f + (1.0f - intensity) * 0.3f;  // Lower damping at high intensity for less mud
        echoMix_ = intensity * 0.3f;
        updateParameters();
        logger_->log("Reverb intensity set to " + std::to_string(intensityPercent) + "%");
        }
        void Reverb::updateParameters() {
        feedback_ = roomSize_ * 0.28f + 0.7f;
        }
        float Reverb::processComb(CombFilter& comb, float input) {
        float output = comb.buffer[comb.index];
        comb.filterStore = output * (1.0f - damping_) + comb.filterStore * damping_;
        comb.buffer[comb.index] = input + comb.filterStore * feedback_;
        comb.index = (comb.index + 1) % comb.buffer.size();
        return output;
        }
        float Reverb::processAllPass(AllPassFilter& ap, float input) {
        float bufOut = ap.buffer[ap.index];
        float output = bufOut - input;
        ap.buffer[ap.index] = input + bufOut * 0.5f;
        ap.index = (ap.index + 1) % ap.buffer.size();
        return output;
        }
        size_t Reverb::apply(float* buffer, size_t bufferSize) {
        const float dryMix = 1.0f - wetMix_;
        for (size_t i = 0; i < bufferSize; i += 2) {
        size_t leftIdx = i;
        size_t rightIdx = i + 1;
        float inputL = buffer[leftIdx];
        float inputR = buffer[rightIdx];
        // Pre-delay
        float delayedInputL = preDelayBuffer_[preDelayIndex_];
        float delayedInputR = delayedInputL;  // Mono pre-delay for simplicity
        preDelayBuffer_[preDelayIndex_] = inputL;  // Assume mono input for delay
        preDelayIndex_ = (preDelayIndex_ + 1) % preDelaySamples_;
        // Sum of comb filters (parallel)
        float combSumL = 0.0f;
        for (auto& comb : combFiltersL_) {
        combSumL += processComb(comb, delayedInputL);
        }
        float combSumR = 0.0f;
        for (auto& comb : combFiltersR_) {
        combSumR += processComb(comb, delayedInputR);
        }
        // All-pass filters (series for diffusion)
        float reverbComponentL = combSumL * 0.125f;  // Normalize by num combs
        for (auto& ap : allPassFiltersL_) {
        reverbComponentL = processAllPass(ap, reverbComponentL);
        }
        float reverbComponentR = combSumR * 0.125f;
        for (auto& ap : allPassFiltersR_) {
        reverbComponentR = processAllPass(ap, reverbComponentR);
        }
        // Highpass to reduce mud
        float lowpassL = highpassCoeff_ * wetHighpassStoreL_ + (1 - highpassCoeff_) * reverbComponentL;
        wetHighpassStoreL_ = lowpassL;
        reverbComponentL -= lowpassL;  // Highpass = input - lowpass
        float lowpassR = highpassCoeff_ * wetHighpassStoreR_ + (1 - highpassCoeff_) * reverbComponentR;
        wetHighpassStoreR_ = lowpassR;
        reverbComponentR -= lowpassR;
        // Gentle echo for tail extension
        float echoOutL = echoBufferL_[echoWriteIndexL_];
        echoBufferL_[echoWriteIndexL_] = reverbComponentL + echoOutL * echoFeedback_;
        echoWriteIndexL_ = (echoWriteIndexL_ + 1) % echoDelaySamples_;
        float echoOutR = echoBufferR_[echoWriteIndexR_];
        echoBufferR_[echoWriteIndexR_] = reverbComponentR + echoOutR * echoFeedback_;
        echoWriteIndexR_ = (echoWriteIndexR_ + 1) % echoDelaySamples_;
        float echoComponentL = echoOutL * echoMix_ * 0.18f;  // gentle, secondary echo
        float echoComponentR = echoOutR * echoMix_ * 0.18f;
        float wetComponentL = reverbComponentL + echoComponentL;
        float wetComponentR = reverbComponentR + echoComponentR;
        buffer[leftIdx] = std::clamp(inputL * dryMix + wetComponentL, -0.95f, 0.95f);
        buffer[rightIdx] = std::clamp(inputR * dryMix + wetComponentR, -0.95f, 0.95f);
        // Safety clipping
        buffer[leftIdx] = std::clamp(buffer[leftIdx], -1.0f, 1.0f);
        buffer[rightIdx] = std::clamp(buffer[rightIdx], -1.0f, 1.0f);
        }
        return bufferSize;
        }
        void Reverb::reset() {
        for (auto& comb : combFiltersL_) {
        std::fill(comb.buffer.begin(), comb.buffer.end(), 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        }
        for (auto& comb : combFiltersR_) {
        std::fill(comb.buffer.begin(), comb.buffer.end(), 0.0f);
        comb.index = 0;
        comb.filterStore = 0.0f;
        }
        for (auto& ap : allPassFiltersL_) {
        std::fill(ap.buffer.begin(), ap.buffer.end(), 0.0f);
        ap.index = 0;
        }
        for (auto& ap : allPassFiltersR_) {
        std::fill(ap.buffer.begin(), ap.buffer.end(), 0.0f);
        ap.index = 0;
        }
        std::fill(preDelayBuffer_.begin(), preDelayBuffer_.end(), 0.0f);
        preDelayIndex_ = 0;
        std::fill(echoBufferL_.begin(), echoBufferL_.end(), 0.0f);
        std::fill(echoBufferR_.begin(), echoBufferR_.end(), 0.0f);
        echoWriteIndexL_ = 0;
        echoWriteIndexR_ = 0;
        wetHighpassStoreL_ = 0.0f;
        wetHighpassStoreR_ = 0.0f;
        }