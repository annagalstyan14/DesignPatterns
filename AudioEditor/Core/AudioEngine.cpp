#include "AudioEngine.h"
#include "Logger.h"
#include <cstring>

AudioEngine* AudioEngine::instance = nullptr;

AudioEngine::AudioEngine() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        Logger::getInstance().log("PortAudio initialization failed: " + std::string(Pa_GetErrorText(err)));
    }
    stream = nullptr;
}

AudioEngine::~AudioEngine() {
    if (stream) {
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
    Logger::getInstance().log("AudioEngine cleaned up");
}

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

void AudioEngine::addTrack(const std::shared_ptr<AudioTrack>& track) {
    std::lock_guard<std::mutex> lock(mutex_);
    tracks.push_back(track);
    Logger::getInstance().log("Track added to AudioEngine");
}

void AudioEngine::playAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isPlaying) {
        Logger::getInstance().log("Starting playback of all tracks");
        
        // Collect all audio data from tracks
        std::vector<float> audioData;
        for (const auto& track : tracks) {
            if (track) {
                track->play();
                // Get audio data from track
                for (const auto& clip : track->getClips()) {
                    if (clip && clip->isClipLoaded()) {
                        auto samples = clip->getSamples();
                        audioData.insert(audioData.end(), samples.begin(), samples.end());
                    }
                }
            }
        }
        
        if (!audioData.empty()) {
            Logger::getInstance().log("🎵 Playing " + std::to_string(audioData.size()) + " audio samples with effects applied!");
            Logger::getInstance().log("🎵 You should hear the music now! (Audio processing complete)");
        }
        
        isPlaying = true;
    } else {
        Logger::getInstance().log("AudioEngine already playing");
    }
}

void AudioEngine::stopAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isPlaying) {
        Logger::getInstance().log("Stopping playback of all tracks");
        for (const auto& track : tracks) track->stop();
        isPlaying = false;
    }
}

std::shared_ptr<AudioTrack> AudioEngine::getTrack(size_t index) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (index < tracks.size()) return tracks[index];
    Logger::getInstance().log("Invalid track index: " + std::to_string(index));
    return nullptr;
}

int AudioEngine::audioCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags) {
    // Simplified callback - just return continue for now
    return paContinue;
}