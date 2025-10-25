//#pragma once
//#include <string>
//#include <vector>
//#include <memory>
//#include "AudioClip.h"
//#include "Logger.h"
//
//class AudioTrack {
//private:
//    std::string name;
//    std::vector<std::shared_ptr<AudioClip>> clips;
//public:
//    AudioTrack(const std::string& trackName);
//    void addClip(const std::shared_ptr<AudioClip>& clip);
//    void removeClip(const std::shared_ptr<AudioClip>& clip);
//    void play();
//    void stop();
//    void mixToBuffer(float* outputBuffer, unsigned long framesPerBuffer);
//    std::vector<std::shared_ptr<AudioClip>> getClips() const;
//    float getTotalDuration() const;
//    ~AudioTrack();
//};