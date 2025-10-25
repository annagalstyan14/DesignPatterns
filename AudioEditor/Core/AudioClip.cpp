#include "AudioClip.h"
#include <iostream>

AudioClip::AudioClip() : filePath(""), duration(0.0f), isLoaded(false) {
    Logger::getInstance().log("AudioClip created with default constructor");
}

AudioClip::AudioClip(const std::string& path) : filePath(path), duration(0.0f), isLoaded(false) {
    Logger::getInstance().log("AudioClip created: " + filePath);
}

bool AudioClip::load() {
    std::cout << "Loading audio file from: " << filePath << std::endl;
    // In a real implementation, you would add file loading logic here
    isLoaded = true;
    duration = 120.0f;
    return isLoaded;
}

void AudioClip::play() {
    if (isLoaded) {
        std::cout << "Playing audio clip: " << filePath << std::endl;
    } else {
        std::cout << "Audio clip not loaded. Cannot play." << std::endl;
    }
}

void AudioClip::stop() {
    if (isLoaded) {
        std::cout << "Stopping audio clip: " << filePath << std::endl;
    } else {
        std::cout << "Audio clip not loaded. Cannot stop." << std::endl;
    }
}

float AudioClip::getDuration() const {
    return duration;
}

AudioClip::~AudioClip() {
    if (isLoaded) {
        std::cout << "Releasing resources for audio clip: " << filePath << std::endl;
    }
}

