#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "Core/CompositeLogger.h"
#include "Core/FileLogger.h"
#include "Core/ConsoleLogger.h"
#include "Core/AudioClip.h"
#include "Core/EffectFactory.h"
#include "Core/Effects/Reverb.h"
#include "Core/Effects/Echo.h"
#include "Core/Effects/Speed.h"

void initializeRegistry() {
    EffectFactory::registerEffect("Reverb", [](std::shared_ptr<ILogger> logger) { 
        return std::make_shared<Reverb>(logger); 
    });
    EffectFactory::registerEffect("Echo", [](std::shared_ptr<ILogger> logger) { 
        return std::make_shared<Echo>(logger); 
    });
    EffectFactory::registerEffect("Speed", [](std::shared_ptr<ILogger> logger) { 
        return std::make_shared<SpeedChangeEffect>(1.0f, logger); 
    });
}

void parseArguments(int argc, char* argv[], std::string& inputFile, std::string& outputFile, 
                   float& reverbIntensity, float& echoIntensity, float& speed) {
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
    echoIntensity = argMap.count("echo") ? std::stof(argMap["echo"]) : 0.0f; 
    speed = argMap.count("speed") ? std::stof(argMap["speed"]) : 1.0f; 
}

int main(int argc, char* argv[]) {
    // CREATE LOGGER SYSTEM
    auto logger = std::make_shared<CompositeLogger>();
    
    try {
        logger->addLogger(std::make_shared<FileLogger>("log.txt"));
        logger->addLogger(std::make_shared<ConsoleLogger>());
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to initialize logging: " << e.what() << std::endl;
        return 1;
    }
    
    logger->log("Program started");

    initializeRegistry();

    std::string inputFile, outputFile;
    float reverbIntensity, echoIntensity, speed;
    parseArguments(argc, argv, inputFile, outputFile, reverbIntensity, echoIntensity, speed);

    if (!std::filesystem::exists(inputFile)) {
        logger->error("Input file does not exist: " + inputFile);
        std::cerr << "Error: Input file does not exist: " + inputFile << std::endl;
        return 1;
    }

    auto clip = std::make_shared<AudioClip>(inputFile, logger);

    if (!clip->load()) {
        logger->error("Failed to load MP3 file: " + inputFile);
        std::cerr << "Error: Failed to load MP3 file: " + inputFile << std::endl;
        return 1;
    }

    // CORRECT ORDER (✓ Better):
// 1. Echo first
    auto echo = EffectFactory::createEffect("Echo", logger);
    if (echo && echoIntensity > 0) {
        auto echoPtr = std::dynamic_pointer_cast<Echo>(echo);
        if (echoPtr) {
            echoPtr->setIntensity(echoIntensity);
        }
        clip->addEffect(echo);  // ← Applied FIRST
    }

    // 2. Reverb second
    auto reverb = EffectFactory::createEffect("Reverb", logger);
    if (reverb && reverbIntensity > 0) {
        auto reverbPtr = std::dynamic_pointer_cast<Reverb>(reverb);
        if (reverbPtr) {
            reverbPtr->setIntensity(reverbIntensity);
        }
        clip->addEffect(reverb);  // ← Applied SECOND
    }

    // 3. Speed last (unchanged)
    auto speedEffect = EffectFactory::createEffect("Speed", logger);
    if (speedEffect && speed != 1.0f) {
        auto speedPtr = std::dynamic_pointer_cast<SpeedChangeEffect>(speedEffect);
        if (speedPtr) {
            speedPtr->setSpeedFactor(speed);
        }
        clip->addEffect(speedEffect);  // ← Applied LAST
}
    clip->applyEffects();

    if (!clip->save(outputFile)) {
        logger->error("Failed to save processed MP3: " + outputFile);
        std::cerr << "Error: Failed to save processed MP3: " + outputFile << std::endl;
        return 1;
    }

    logger->log("Successfully saved processed MP3 with Reverb, Echo, and Speed to " + outputFile);
    std::cout << "Successfully saved processed MP3 to " + outputFile << std::endl;
    return 0;
}