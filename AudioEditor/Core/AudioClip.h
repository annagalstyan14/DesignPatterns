#pragma once
#include <string>
#include "Logger.h"

class AudioClip {

private:
    std::string filePath;
    float duration;
    bool isLoaded;
    
public:
    AudioClip();
    AudioClip(const std::string& path);
    bool load();
    void play();
    void stop();
    float getDuration() const;
    ~AudioClip();
};