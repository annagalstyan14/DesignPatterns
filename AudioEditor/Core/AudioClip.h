#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "Adapters/AudioFileAdapter.h"
#include "Effects/IEffect.h"
#include "Logger.h"

class AudioClip {
private:
    std::string filePath;
    std::shared_ptr<AudioFileAdapter> audioFile;
    std::vector<float> samples;
    float startTime = 0.0f;
    float duration = 0.0f;
    bool isLoaded = false;
    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<IEffect>> effects;
public:
    AudioClip(const std::string& path);
    bool load();
    void play();
    void stop();
    float getDuration() const;
    float getStartTime() const;
    void setStartTime(float time);
    void addEffect(std::shared_ptr<IEffect> effect);
    void applyEffects();
    void mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer);
    bool isClipLoaded() const;
    std::vector<float> getSamples() const;
};