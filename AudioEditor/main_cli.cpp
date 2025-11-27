#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "Core/FileLogger.h"
#include "Core/ConsoleLogger.h"
#include "Core/CompositeLogger.h"
#include "Core/AudioClip.h"
#include "Core/EffectFactory.h"
#include "Core/Effects/Reverb.h"
#include "Core/Effects/Speed.h"

void initializeRegistry() {
    EffectFactory::registerEffect("Reverb", [](std::shared_ptr<ILogger> logger) {
        return std::make_shared<Reverb>(logger);
    });
    EffectFactory::registerEffect("Speed", [](std::shared_ptr<ILogger> logger) {
        return std::make_shared<SpeedChangeEffect>(1.0f, logger);
    });
}

void parseArguments(int argc, char* argv[], std::string& inputFile, std::string& outputFile, 
                   float& reverbIntensity, float& speed) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::map<std::string, std::string> argMap;

    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg.substr(0, 2) == "--") arg = arg.substr(2);
        if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
            argMap[arg] = args[++i];
        } else {
            argMap[arg] = "true";
        }
    }

    inputFile = argMap.count("input") ? argMap["input"] : "arctic.mp3";
    outputFile = argMap.count("output") ? argMap["output"] : "output.mp3";
    reverbIntensity = argMap.count("reverb") ? std::stof(argMap["reverb"]) : 0.0f; 
    speed = argMap.count("speed") ? std::stof(argMap["speed"]) : 1.0f; 
}

int main(int argc, char* argv[]) {
    // Setup composite logger (file + console)
    auto composite = std::make_shared<CompositeLogger>();
    composite->addLogger(std::make_shared<FileLogger>("log.txt"));
    composite->addLogger(std::make_shared<ConsoleLogger>());
    
    composite->log("Program started");

    initializeRegistry();

    std::string inputFile, outputFile;
    float reverbIntensity, speed;
    parseArguments(argc, argv, inputFile, outputFile, reverbIntensity, speed);

    if (!std::filesystem::exists(inputFile)) {
        composite->error("Input file does not exist: " + inputFile);
        std::cerr << "Error: Input file does not exist: " + inputFile << std::endl;
        return 1;
    }

    auto clip = std::make_shared<AudioClip>(inputFile, composite);

    if (!clip->load()) {
        composite->error("Failed to load MP3 file: " + inputFile);
        std::cerr << "Error: Failed to load MP3 file: " + inputFile << std::endl;
        return 1;
    }

    if (reverbIntensity > 0) {
        auto reverb = EffectFactory::createEffect("Reverb", composite);
        if (reverb) {
            auto reverbPtr = std::dynamic_pointer_cast<Reverb>(reverb);
            if (reverbPtr) {
                reverbPtr->setIntensity(reverbIntensity);
            }
            clip->addEffect(reverb);
        }
    }

    if (speed != 1.0f) {
        auto speedEffect = EffectFactory::createEffect("Speed", composite);
        if (speedEffect) {
            auto speedPtr = std::dynamic_pointer_cast<SpeedChangeEffect>(speedEffect);
            if (speedPtr) {
                speedPtr->setSpeedFactor(speed);
            }
            clip->addEffect(speedEffect);
        }
    }

    clip->applyEffects();

    if (!clip->save(outputFile)) {
        composite->error("Failed to save processed MP3: " + outputFile);
        std::cerr << "Error: Failed to save processed MP3: " + outputFile << std::endl;
        return 1;
    }

    composite->log("Successfully saved processed MP3 to " + outputFile);
    std::cout << "Successfully saved processed MP3 to " + outputFile << std::endl;
    return 0;
}