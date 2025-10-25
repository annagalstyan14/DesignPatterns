#pragma once
#include <vector>
#include <memory>
#include <mutex>
#include <portaudio.h>
#include "AudioTrack.h"
#include "Logger.h"

class AudioEngine {
private:
    static AudioEngine* instance;
    std::vector<std::shared_ptr<AudioTrack>> tracks;
    bool isPlaying = false;
    PaStream* stream;
    std::mutex mutex_;
    AudioEngine();
public:
    static AudioEngine& getInstance();
    void addTrack(const std::shared_ptr<AudioTrack>& track);
    void playAll();
    void stopAll();
    std::shared_ptr<AudioTrack> getTrack(size_t index);
    int audioCallback(const void* inputBuffer, void* outputBuffer,
                     unsigned long framesPerBuffer,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags);
    ~AudioEngine();
    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
};