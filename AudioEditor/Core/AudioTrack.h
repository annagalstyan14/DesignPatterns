#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AudioClip.h"
#include "Logger.h"

class AudioTrack {
private:
    std::vector<std::shared_ptr<AudioClip>> clips;
    std::string name;

public:
    AudioTrack(const std::string& trackName) : name(trackName) {}
    void addClip(const std::shared_ptr<AudioClip>& clip);
    void removeClip(const std::shared_ptr<AudioClip>& clip);
    void playAllClips();
    float getTotalDuration() const;
    ~AudioTrack();
};