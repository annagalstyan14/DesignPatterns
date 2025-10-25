#include <thread>
#include <chrono>
#include <iostream>
#include "Core/Logger.h"
#include "Core/AudioEngine.h"
#include "Core/AudioTrack.h"
#include "Core/AudioClip.h"
#include "Core/AudioProjectBuilder.h"
#include "Core/EffectFactory.h"
#include "Core/Adapters/Wav.h"
#include "Core/Effects/Reverb.h"
#include "Core/Effects/Echo.h"
#include "Core/Effects/Fade.h"

// Initialize EffectFactory registry
void initializeRegistry() {
    EffectFactory::registerEffect("Reverb", []() { return std::make_shared<Reverb>(); });
    EffectFactory::registerEffect("Echo", []() { return std::make_shared<Echo>(); });
    EffectFactory::registerEffect("Fade", []() { return std::make_shared<Fade>(); });
}

int main() {
    // Initialize Logger
    if (!Logger::getInstance().setLogFile("log.txt")) {
        Logger::getInstance().log("Failed to open log file");
        return 1;
    }
    Logger::getInstance().log("Program started");

    // Initialize EffectFactory
    initializeRegistry();

    // Build AudioProject with tracks and clips
    AudioProjectBuilder builder;
    auto clip1 = std::make_shared<AudioClip>("aespa (에스파) 'Whiplash' (Color Coded Lyrics).mp3");

    auto project = builder
        .addTrack("Track 1")
        .addClipToTrack(0, clip1)
        .build();

    // Load audio clips (optional for testing)
    if (!clip1->load()) {
        Logger::getInstance().log("Failed to load AESPA song - continuing with demo");
    }

    // Add effects to clip1 and apply them
    auto reverb = EffectFactory::createEffect("Reverb");
    auto echo = EffectFactory::createEffect("Echo");
    if (reverb) clip1->addEffect(reverb);
    if (echo) clip1->addEffect(echo);
    
    // Apply effects to the loaded audio data
    clip1->applyEffects();

    // Play project with real audio output
    project->play();

    // Real audio playback - wait for user to stop
    Logger::getInstance().log("🎵 Audio is now playing! Press Enter to stop...");
    std::cin.get(); // Wait for user to press Enter

    // Stop playback
    AudioEngine::getInstance().stopAll();

    return 0;
}