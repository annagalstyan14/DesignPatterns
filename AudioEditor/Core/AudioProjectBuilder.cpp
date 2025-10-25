#include "AudioProjectBuilder.h"
#include "Logger.h"
#include <vector>

AudioProjectBuilder::AudioProjectBuilder() : project(std::make_unique<AudioProject>()) {}

AudioProjectBuilder& AudioProjectBuilder::addTrack(const std::string& name) {
    auto track = std::make_shared<AudioTrack>(name);
    tracks.push_back(track);
    project->addTrack(track);
    return *this;
}

AudioProjectBuilder& AudioProjectBuilder::addClipToTrack(size_t trackIndex, const std::shared_ptr<AudioClip>& clip) {
    if (trackIndex < tracks.size()) {
        tracks[trackIndex]->addClip(clip);
    } else {
        Logger::getInstance().log("Invalid track index: " + std::to_string(trackIndex));
    }
    return *this;
}

std::unique_ptr<AudioProject> AudioProjectBuilder::build() {
    return std::move(project);
}

AudioProjectBuilder::~AudioProjectBuilder() = default;

