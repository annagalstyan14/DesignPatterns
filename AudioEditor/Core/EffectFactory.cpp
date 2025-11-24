#include "EffectFactory.h"

std::map<std::string, std::function<std::shared_ptr<IEffect>(std::shared_ptr<ILogger>)>> EffectFactory::registry;

void EffectFactory::registerEffect(const std::string& type, 
                                   std::function<std::shared_ptr<IEffect>(std::shared_ptr<ILogger>)> creator) {
    registry[type] = creator;
}

std::shared_ptr<IEffect> EffectFactory::createEffect(const std::string& effectType, 
                                                     std::shared_ptr<ILogger> logger) {
    auto it = registry.find(effectType);
    if (it != registry.end()) {
        return it->second(logger);
    }
    
    if (logger) {
        logger->error("Unknown effect type: " + effectType);
    }
    return nullptr;
}