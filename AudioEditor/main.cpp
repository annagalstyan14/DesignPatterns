#include <iostream>
#include <filesystem>
#include "Core/Logger.h"
#include "Core/AudioClip.h"
#include "Core/EffectFactory.h"
#include "Core/Effects/Reverb.h"
#include "Core/Effects/Echo.h"

void initializeRegistry() {
    EffectFactory::registerEffect("Reverb", []() { return std::make_shared<Reverb>(); });
    EffectFactory::registerEffect("Echo", []() { return std::make_shared<Echo>(); });
}

int main() {
    if (!Logger::getInstance().setLogFile("log.txt")) {
        Logger::getInstance().log("Failed to open log file");
        std::cerr << "Error: Failed to open log file: log.txt" << std::endl;
        return 1;
    }
    Logger::getInstance().log("Program started");

    initializeRegistry();

    std::string inputFile = "test.mp3";
    if (!std::filesystem::exists(inputFile)) {
        Logger::getInstance().log("Input file does not exist: " + inputFile);
        std::cerr << "Error: Input file does not exist: " + inputFile << std::endl;
        return 1;
    }

    auto clip = std::make_shared<AudioClip>(inputFile);

    if (!clip->load()) {
        Logger::getInstance().log("Failed to load MP3 file: " + inputFile);
        std::cerr << "Error: Failed to load MP3 file: " + inputFile << std::endl;
        return 1;
    }

    auto reverb = EffectFactory::createEffect("Reverb");
    auto echo = EffectFactory::createEffect("Echo");
    if (!reverb || !echo) {
        Logger::getInstance().log("Failed to create effects");
        std::cerr << "Error: Failed to create effects" << std::endl;
        return 1;
    }
    clip->addEffect(reverb);
    clip->addEffect(echo);

    clip->applyEffects();

    std::string outputFile = "output.mp3";
    if (!clip->save(outputFile)) {
        Logger::getInstance().log("Failed to save processed MP3: " + outputFile);
        std::cerr << "Error: Failed to save processed MP3: " + outputFile << std::endl;
        return 1;
    }

    Logger::getInstance().log("Successfully saved processed MP3 with Reverb and Echo to " + outputFile);
    std::cout << "Successfully saved processed MP3 to " << outputFile << std::endl;
    return 0;
}