#include <iostream>
#include <filesystem>
#include "Logger.h"
#include "AudioClip.h"
#include "Effects/Reverb.h"
#include "Effects/Speed.h" // Matches Speed.h

int main(int argc, char* argv[]) {
    if (!Logger::getInstance().setLogFile("log.txt")) {
        Logger::getInstance().log("Failed to open log file");
        std::cerr << "Error: Failed to open log file: log.txt" << std::endl;
        return 1;
    }
    Logger::getInstance().log("Program started");

    std::string inputFile = "input.mp3";
    if (argc >= 2) {
        inputFile = argv[1];
    }
    if (!std::filesystem::exists(inputFile)) {
        Logger::getInstance().log("Input file does not exist: " + inputFile);
        std::cerr << "Error: Input file does not exist: " + inputFile << std::endl;
        return 1;
    }

    float reverbTimeMs = 600.0f; // ms
    float diffusion = 70.0f;     // %
    float decay = 50.0f;         // %
    float wetOut = 40.0f;        // %
    float dryOut = 90.0f;        // %
    float brightness = 20.0f;    // %
    float speedFactor = 1.0f;    // Normal speed


    for (int i = 2; i < argc; i += 2) {
        std::string arg = argv[i];
        if (i + 1 < argc) {
            try {
                if (arg == "--reverb-time") {
                    reverbTimeMs = std::stof(argv[i + 1]);
                    if (reverbTimeMs < 100.0f || reverbTimeMs > 5000.0f) throw std::invalid_argument("Reverb time must be 100–5000ms");
                }
                else if (arg == "--diffusion") {
                    diffusion = std::stof(argv[i + 1]);
                    if (diffusion < 0.0f || diffusion > 100.0f) throw std::invalid_argument("Diffusion must be 0–100%");
                }
                else if (arg == "--decay") {
                    decay = std::stof(argv[i + 1]);
                    if (decay < 0.0f || decay > 100.0f) throw std::invalid_argument("Decay must be 0–100%");
                }
                else if (arg == "--wet") {
                    wetOut = std::stof(argv[i + 1]);
                    if (wetOut < 0.0f || wetOut > 400.0f) throw std::invalid_argument("Wet must be 0–400%");
                }
                else if (arg == "--dry") {
                    dryOut = std::stof(argv[i + 1]);
                    if (dryOut < 0.0f || dryOut > 400.0f) throw std::invalid_argument("Dry must be 0–400%");
                }
                else if (arg == "--brightness") {
                    brightness = std::stof(argv[i + 1]);
                    if (brightness < 0.0f || brightness > 100.0f) throw std::invalid_argument("Brightness must be 0–100%");
                }
                else if (arg == "--speed") {
                    speedFactor = std::stof(argv[i + 1]);
                    if (speedFactor <= 0.0f) throw std::invalid_argument("Speed factor must be positive");
                }
            } catch (const std::exception& e) {
                Logger::getInstance().log("Error: Invalid parameter for " + arg + ": " + e.what());
                std::cerr << "Error: Invalid parameter for " + arg << std::endl;
                return 1;
            }
        }
    }
    Logger::getInstance().log("Parameters: reverbTime=" + std::to_string(reverbTimeMs) + "ms, " +
                             "diffusion=" + std::to_string(diffusion) + "%, " +
                             "decay=" + std::to_string(decay) + "%, " +
                             "wet=" + std::to_string(wetOut) + "%, " +
                             "dry=" + std::to_string(dryOut) + "%, " +
                             "brightness=" + std::to_string(brightness) + "%, " +
                             "speed=" + std::to_string(speedFactor));

    auto clip = std::make_shared<AudioClip>(inputFile);
    if (!clip->load()) {
        Logger::getInstance().log("Failed to load MP3 file: " + inputFile);
        std::cerr << "Error: Failed to load MP3 file: " + inputFile << std::endl;
        return 1;
    }

    // Add Reverb effect with parsed parameters
    auto reverb = std::make_shared<Reverb>(
        reverbTimeMs, diffusion, decay, dryOut / 100.0f, wetOut / 100.0f, brightness
    );
    
    clip->addEffect(reverb);

    // Add SpeedChange effect if specified
    if (speedFactor != 1.0f) {
        clip->addEffect(std::make_shared<SpeedChangeEffect>(speedFactor));
    }

    clip->applyEffects();

    std::string outputFile = "output.mp3";
    if (!clip->save(outputFile)) {
        Logger::getInstance().log("Failed to save processed MP3: " + outputFile);
        std::cerr << "Error: Failed to save processed MP3: " + outputFile << std::endl;
        return 1;
    }

    Logger::getInstance().log("Successfully saved processed MP3 with Reverb and SpeedChange to " + outputFile);
    std::cout << "Successfully saved processed MP3 to " + outputFile << std::endl;
    return 0;
}