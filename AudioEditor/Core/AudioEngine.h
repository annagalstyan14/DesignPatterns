#pragma once

#include <vector>
#include <memory>
#include "Logger.h"
#include "AudioTrack.h"

class AudioEngine {

private:
    std::vector<std::shared_ptr<AudioTrack>> tracks;
    bool isPlaying;

public:
    AudioEngine() : isPlaying(false) {}
    static AudioEngine& getInstance();
    void addTrack(const std::shared_ptr<AudioTrack>& track);
    void playAll();
    void stopAll();
    std::shared_ptr<AudioTrack> getTrack(size_t index);
    ~AudioEngine();
};