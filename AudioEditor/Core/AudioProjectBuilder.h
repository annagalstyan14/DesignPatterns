//#pragma once
//#include <memory>
//#include <vector>
//#include "AudioTrack.h"
//#include "AudioProject.h"
//
//class AudioProjectBuilder {
//public:
//    AudioProjectBuilder();
//    ~AudioProjectBuilder();
//
//    AudioProjectBuilder& addTrack(const std::string& name);
//    AudioProjectBuilder& addClipToTrack(size_t trackIndex, const std::shared_ptr<AudioClip>& clip);
//    std::unique_ptr<AudioProject> build();
//
//private:
//    std::unique_ptr<AudioProject> project;
//    std::vector<std::shared_ptr<AudioTrack>> tracks;
//};
//
//