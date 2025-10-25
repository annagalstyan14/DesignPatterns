#include "AudioEngine.h"
#include <iostream>

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

void AudioEngine::addTrack(const std::shared_ptr<AudioTrack>& track) {
    tracks.push_back(track);
    std::cout << "Track added to AudioEngine." << std::endl;
}

void AudioEngine::playAll() {
    if (!isPlaying) {
        std::cout << "Starting playback of all tracks." << std::endl;
        for (const auto& track : tracks) {
            track->playAllClips();
        }
        isPlaying = true;
    } else {
        std::cout << "AudioEngine is already playing." << std::endl;
    }
}

void AudioEngine::stopAll() {
    if (isPlaying) {
        std::cout << "Stopping playback of all tracks." << std::endl;
        // In a real implementation, you would stop all clips here
        isPlaying = false;
    } else {
        std::cout << "AudioEngine is not playing." << std::endl;
    }
}

std::shared_ptr<AudioTrack> AudioEngine::getTrack(size_t index) {
    if (index < tracks.size()) {
        return tracks[index];
    } else {
        std::cout << "Track index out of range." << std::endl;
        return nullptr;
    }
}

AudioEngine::~AudioEngine() {
    std::cout << "Destroying AudioEngine and releasing all tracks." << std::endl;
}

