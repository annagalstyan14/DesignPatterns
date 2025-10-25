#include <iostream>       // for console output
#include <memory>         // for shared_ptr
#include "Core/Logger.h"       // log actions
#include "Core/AudioEngine.h"  // access engine
#include "Core/AudioTrack.h"   // create tracks
#include "AudioClip.h"

int main() {
    Logger::getInstance().setLogFile("log.txt");

    auto& engine = AudioEngine::getInstance();
    auto track1 = std::make_shared<AudioTrack>("Track 1");
    auto clip1 = std::make_shared<AudioClip>("song.wav");

    clip1->load();
    track1->addClip(clip1);
    engine.addTrack(track1);

    engine.playAll();
}