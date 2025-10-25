#include "AudioTrack.h"
#include <algorithm>

AudioTrack::AudioTrack(const std::string& trackName) : name(trackName) {}
AudioTrack::~AudioTrack() { Logger::getInstance().log("Cleaning up track: " + name); }

void AudioTrack::addClip(const std::shared_ptr<AudioClip>& clip) {
    clips.push_back(clip);
    Logger::getInstance().log("Clip added to track: " + name);
}

void AudioTrack::removeClip(const std::shared_ptr<AudioClip>& clip) {
    auto it = std::find(clips.begin(), clips.end(), clip);
    if (it != clips.end()) {
        clips.erase(it);
        Logger::getInstance().log("Clip removed from track: " + name);
    }
}

void AudioTrack::play() {
    Logger::getInstance().log("Playing track: " + name);
    for (const auto& clip : clips) clip->play();
}

void AudioTrack::stop() {
    Logger::getInstance().log("Stopping track: " + name);
    for (const auto& clip : clips) clip->stop();
}

void AudioTrack::mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer) {
    // Mix all clips in this track to the output buffer
    for (const auto& clip : clips) {
        if (clip) {
            clip->mixToBuffer(outputBuffer, framesPerBuffer);
        }
    }
}

std::vector<std::shared_ptr<AudioClip>> AudioTrack::getClips() const {
    return clips;
}

float AudioTrack::getTotalDuration() const {
    float total = 0.0f;
    for (const auto& clip : clips) total += clip->getDuration();
    return total;
}