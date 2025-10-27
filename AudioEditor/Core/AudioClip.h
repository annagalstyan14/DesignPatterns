#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "Adapters/AudioFileAdapter.h"
#include "EffectFactory.h"
#include "Logger.h"

class AudioClip {
private:
    std::string filePath;
    std::shared_ptr<AudioFileAdapter> audioFile;
    std::vector<float> samples;
    float duration = 0.0f;
    float startTime = 0.0f;
    bool isLoaded = false;
    std::vector<std::shared_ptr<IEffect>> effects;
    std::mutex mutex_;
public:
    AudioClip(const std::string& path);
    bool load();
    void play();
    void stop();
    void mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer);
    void applyEffects();
    bool save(const std::string& outputPath);
    float getDuration() const;
    float getStartTime() const;
    void setStartTime(float time);
    void addEffect(std::shared_ptr<IEffect> effect);
    bool isClipLoaded() const;
    std::vector<float> getSamples() const;
};