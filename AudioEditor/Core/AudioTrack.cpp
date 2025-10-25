#include "AudioTrack.h"
#include <iostream>

void AudioTrack::addClip(const std::shared_ptr<AudioClip>& clip) {
    clips.push_back(clip);
    std::cout << "Added clip to track: " << name << std::endl;
}

void AudioTrack::removeClip(const std::shared_ptr<AudioClip>& clip) {
    auto it = std::find(clips.begin(), clips.end(), clip);
    if (it != clips.end()) {
        clips.erase(it);
        std::cout << "Removed clip from track: " << name << std::endl;
    } else {
        std::cout << "Clip not found in track: " << name << std::endl;
    }
}

void AudioTrack::playAllClips() {
    std::cout << "Playing all clips in track: " << name << std::endl;
    for (const auto& clip : clips) {
        clip->play();
    }
}

float AudioTrack::getTotalDuration() const {
    float totalDuration = 0.0f;
    for (const auto& clip : clips) {
        totalDuration += clip->getDuration();
    }
    return totalDuration;
}

AudioTrack::~AudioTrack() {
    std::cout << "Destroying AudioTrack: " << name << std::endl;
}
