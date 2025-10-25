#pragma once
#include <memory>
#include <vector>
#include "AudioTrack.h"

class AudioProject {
private:
    std::vector<std::shared_ptr<AudioTrack>> tracks;

public:
    void addTrack(const std::shared_ptr<AudioTrack>& track);
    void play();
    ~AudioProject();
    void addClipToTrack(size_t trackIndex, const std::shared_ptr<AudioClip>& clip);
};