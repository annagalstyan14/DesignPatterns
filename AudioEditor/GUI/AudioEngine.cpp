#include "AudioEngine.h"
#include "../Core/AudioClip.h"
#include "../Core/Effects/Reverb.h"
#include "../Core/Effects/Speed.h"
#include <QMediaDevices>
#include <QAudioDevice>
#include <QtEndian>
#include <QDebug>
#include <cmath>
#include <algorithm>

AudioEngine::AudioEngine(QObject* parent)
    : QObject(parent)
    , audioClip_(nullptr)
    , audioSink_(nullptr)
    , audioBuffer_(nullptr)
    , positionTimer_(new QTimer(this))
    , state_(PlaybackState::Stopped)
    , volume_(1.0f)
    , sampleRate_(44100)
    , channels_(2)
    , pausedPosition_(0)
{
    // Timer to emit position updates every 50ms during playback
    connect(positionTimer_, &QTimer::timeout, this, &AudioEngine::updatePosition);
}

AudioEngine::~AudioEngine() {
    stop();
    delete audioSink_;
    delete audioBuffer_;
}

void AudioEngine::setAudioClip(std::shared_ptr<AudioClip> clip) {
    stop();
    
    audioClip_ = clip;
    
    if (audioClip_) {
        sampleRate_ = 44100;
        channels_ = 2;
        
        // THIS LINE IS CRITICAL
        originalSamples_ = audioClip_->getSamples();
        previewSamples_.clear();
        hasPreview_ = false;
        
        convertSamplesToBytes();
        setupAudio();
        
        emit durationChanged(getDurationMs());
    }
}

std::shared_ptr<AudioClip> AudioEngine::getAudioClip() const {
    return audioClip_;
}

void AudioEngine::setupAudio() {
    // Clean up previous audio sink
    if (audioSink_) {
        audioSink_->stop();
        delete audioSink_;
        audioSink_ = nullptr;
    }
    
    if (audioBuffer_) {
        delete audioBuffer_;
        audioBuffer_ = nullptr;
    }
    
    // Setup audio format (16-bit signed PCM, stereo, 44100 Hz)
    QAudioFormat format;
    format.setSampleRate(sampleRate_);
    format.setChannelCount(channels_);
    format.setSampleFormat(QAudioFormat::Int16);
    
    // Get default audio output device
    QAudioDevice audioDevice = QMediaDevices::defaultAudioOutput();
    
    if (!audioDevice.isFormatSupported(format)) {
        qWarning() << "Audio format not supported, trying nearest format";
        format = audioDevice.preferredFormat();
    }
    
    // Create audio sink
    audioSink_ = new QAudioSink(audioDevice, format, this);
    
    // Connect state change signal
    connect(audioSink_, &QAudioSink::stateChanged, 
            this, &AudioEngine::onAudioStateChanged);
    
    // Create buffer with audio data
    audioBuffer_ = new QBuffer(&audioData_, this);
}

void AudioEngine::convertSamplesToBytes() {
    if (!audioClip_) {
        audioData_.clear();
        return;
    }
    
    std::vector<float> samples = audioClip_->getSamples();
    
    if (samples.empty()) {
        audioData_.clear();
        return;
    }
    
    // Allocate buffer: 2 bytes per sample (16-bit)
    audioData_.resize(samples.size() * sizeof(qint16));
    qint16* dataPtr = reinterpret_cast<qint16*>(audioData_.data());
    
    // Convert float [-1.0, 1.0] to int16 [-32768, 32767]
    for (size_t i = 0; i < samples.size(); ++i) {
        float sample = std::clamp(samples[i], -1.0f, 1.0f);
        dataPtr[i] = static_cast<qint16>(sample * 32767.0f);
    }
}

void AudioEngine::play() {
    if (!audioSink_ || audioData_.isEmpty()) {
        qWarning() << "Cannot play: no audio loaded";
        return;
    }
    
    if (state_ == PlaybackState::Playing) {
        return;  // Already playing
    }
    
    if (state_ == PlaybackState::Paused) {
        // Resume from paused position
        audioBuffer_->open(QIODevice::ReadOnly);
        audioBuffer_->seek(pausedPosition_);
    } else {
        // Start from beginning
        audioBuffer_->open(QIODevice::ReadOnly);
        audioBuffer_->seek(0);
    }
    
    audioSink_->start(audioBuffer_);
    
    state_ = PlaybackState::Playing;
    emit stateChanged(state_);
    
    // Start position update timer
    positionTimer_->start(50);  // Update every 50ms
}

void AudioEngine::pause() {
    if (state_ != PlaybackState::Playing) {
        return;
    }
    
    // Save current position before stopping
    pausedPosition_ = audioBuffer_->pos();
    
    audioSink_->stop();
    audioBuffer_->close();
    
    state_ = PlaybackState::Paused;
    emit stateChanged(state_);
    
    positionTimer_->stop();
}

void AudioEngine::stop() {
    if (state_ == PlaybackState::Stopped) {
        return;
    }
    
    if (audioSink_) {
        audioSink_->stop();
    }
    
    if (audioBuffer_ && audioBuffer_->isOpen()) {
        audioBuffer_->close();
    }
    
    pausedPosition_ = 0;
    
    state_ = PlaybackState::Stopped;
    emit stateChanged(state_);
    emit positionChanged(0);
    
    positionTimer_->stop();
}

void AudioEngine::seek(qint64 positionMs) {
    if (!audioBuffer_ || audioData_.isEmpty()) {
        return;
    }
    
    // Convert milliseconds to byte position
    // Bytes = (ms / 1000) * sampleRate * channels * bytesPerSample
    qint64 bytePosition = (positionMs * sampleRate_ * channels_ * sizeof(qint16)) / 1000;
    
    // Align to frame boundary (channels * bytesPerSample)
    int frameSize = channels_ * sizeof(qint16);
    bytePosition = (bytePosition / frameSize) * frameSize;
    
    // Clamp to valid range
    bytePosition = std::clamp(bytePosition, qint64(0), qint64(audioData_.size()));
    
    if (state_ == PlaybackState::Playing) {
        // If playing, we need to restart from new position
        audioSink_->stop();
        audioBuffer_->close();
        audioBuffer_->open(QIODevice::ReadOnly);
        audioBuffer_->seek(bytePosition);
        audioSink_->start(audioBuffer_);
    } else {
        // If paused or stopped, just save the position
        pausedPosition_ = bytePosition;
    }
    
    emit positionChanged(positionMs);
}

PlaybackState AudioEngine::getState() const {
    return state_;
}

qint64 AudioEngine::getPositionMs() const {
    if (!audioBuffer_ || audioData_.isEmpty()) {
        return 0;
    }
    
    qint64 bytePos;
    
    if (state_ == PlaybackState::Playing && audioBuffer_->isOpen()) {
        bytePos = audioBuffer_->pos();
    } else {
        bytePos = pausedPosition_;
    }
    
    // Convert bytes to milliseconds
    // ms = (bytes / (sampleRate * channels * bytesPerSample)) * 1000
    qint64 positionMs = (bytePos * 1000) / (sampleRate_ * channels_ * sizeof(qint16));
    
    return positionMs;
}

qint64 AudioEngine::getDurationMs() const {
    if (audioData_.isEmpty()) {
        return 0;
    }
    
    // Duration = totalBytes / (sampleRate * channels * bytesPerSample) * 1000
    qint64 durationMs = (audioData_.size() * 1000) / 
                        (sampleRate_ * channels_ * sizeof(qint16));
    
    return durationMs;
}

float AudioEngine::getVolume() const {
    return volume_;
}

void AudioEngine::setVolume(float volume) {
    volume_ = std::clamp(volume, 0.0f, 1.0f);
    
    if (audioSink_) {
        audioSink_->setVolume(volume_);
    }
}

void AudioEngine::onAudioStateChanged(QAudio::State audioState) {
    switch (audioState) {
        case QAudio::IdleState:
            // Playback finished (buffer empty)
            if (state_ == PlaybackState::Playing) {
                stop();
                emit audioFinished();
            }
            break;
            
        case QAudio::StoppedState:
            // Check for errors
            if (audioSink_->error() != QAudio::NoError) {
                qWarning() << "Audio error:" << audioSink_->error();
                stop();
            }
            break;
            
        case QAudio::ActiveState:
            // Audio is playing
            break;
            
        case QAudio::SuspendedState:
            // Audio is paused (system level)
            break;
    }
}

void AudioEngine::updatePosition() {
    if (state_ == PlaybackState::Playing) {
        emit positionChanged(getPositionMs());
    }
}

void AudioEngine::setOriginalSamples(const std::vector<float>& samples) {
    originalSamples_ = samples;
    previewSamples_.clear();
    hasPreview_ = false;
}

void AudioEngine::previewWithEffects(const std::vector<std::shared_ptr<IEffect>>& effects) {
    if (originalSamples_.empty()) {
        qWarning() << "previewWithEffects: No original samples loaded";
        return;
    }
    
    if (effects.empty()) {
        revertToOriginal();
        return;
    }
    
    // Start from original samples
    previewSamples_ = originalSamples_;
    size_t dataSize = previewSamples_.size();  // Track actual audio data size
    
    // Apply all effects
    for (const auto& effect : effects) {
        if (!effect) continue;
        
        // Reset effect state
        if (auto* reverb = dynamic_cast<Reverb*>(effect.get())) {
            reverb->reset();
        }
        
        // For speed effect, ensure buffer is large enough BEFORE applying
        if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
            float factor = speed->getSpeedFactor();
            size_t neededSize = static_cast<size_t>(dataSize / factor) + 1024; // Extra padding
            if (neededSize > previewSamples_.size()) {
                previewSamples_.resize(neededSize, 0.0f);
            }
        }
        
        // Apply effect - pass actual data size, not buffer size
        size_t newSize = effect->apply(previewSamples_.data(), dataSize);
        dataSize = newSize;
    }
    
    // Trim to actual data size
    previewSamples_.resize(dataSize);
    hasPreview_ = true;
    
    // Update audio buffer
    audioData_.resize(previewSamples_.size() * sizeof(qint16));
    qint16* dataPtr = reinterpret_cast<qint16*>(audioData_.data());
    for (size_t i = 0; i < previewSamples_.size(); ++i) {
        float sample = std::clamp(previewSamples_[i], -1.0f, 1.0f);
        dataPtr[i] = static_cast<qint16>(sample * 32767.0f);
    }
    
    emit durationChanged(getDurationMs());
}

void AudioEngine::previewWithSamples(const std::vector<float>& samples) {
    if (samples.empty()) {
        revertToOriginal();
        return;
    }

    previewSamples_ = samples;
    hasPreview_ = true;

    audioData_.resize(previewSamples_.size() * sizeof(qint16));
    qint16* dataPtr = reinterpret_cast<qint16*>(audioData_.data());
    for (size_t i = 0; i < previewSamples_.size(); ++i) {
        float sample = std::clamp(previewSamples_[i], -1.0f, 1.0f);
        dataPtr[i] = static_cast<qint16>(sample * 32767.0f);
    }

    emit durationChanged(getDurationMs());
}

void AudioEngine::commitEffects() {
    if (hasPreview_ && !previewSamples_.empty()) {
        originalSamples_ = previewSamples_;
        hasPreview_ = false;
    }
}

void AudioEngine::revertToOriginal() {
    if (originalSamples_.empty()) {
        return;
    }
    
    previewSamples_.clear();
    hasPreview_ = false;
    
    // Restore original audio
    audioData_.resize(originalSamples_.size() * sizeof(qint16));
    qint16* dataPtr = reinterpret_cast<qint16*>(audioData_.data());
    for (size_t i = 0; i < originalSamples_.size(); ++i) {
        float sample = std::clamp(originalSamples_[i], -1.0f, 1.0f);
        dataPtr[i] = static_cast<qint16>(sample * 32767.0f);
    }
    
    emit durationChanged(getDurationMs());
}