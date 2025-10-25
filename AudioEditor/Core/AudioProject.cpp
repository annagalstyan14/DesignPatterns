//#include "AudioProject.h"
//
//void AudioProject::addTrack(const std::shared_ptr<AudioTrack>& track) {
//    tracks.push_back(track);
//}
//
//void AudioProject::play() {
//    for (const auto& track : tracks) {
//        track->play();
//    }
//}
//
//AudioProject::~AudioProject() {
//    tracks.clear();
//}
//
//void AudioProject::addClipToTrack(size_t trackIndex, const std::shared_ptr<AudioClip>& clip) {
//    if (trackIndex < tracks.size()) {
//        tracks[trackIndex]->addClip(clip);
//    }
//}