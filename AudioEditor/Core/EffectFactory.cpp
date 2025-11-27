#include "EffectFactory.h"

std::map<std::string, EffectFactory::Creator>& EffectFactory::getRegistry() {
    // Meyer's singleton - thread-safe in C++11+
    static std::map<std::string, Creator> registry;
    return registry;
}

void EffectFactory::registerEffect(const std::string& type, Creator creator) {
    getRegistry()[type] = std::move(creator);
}

std::shared_ptr<IEffect> EffectFactory::createEffect(const std::string& effectType, 
                                                     std::shared_ptr<ILogger> logger) {
    auto& registry = getRegistry();
    const auto it = registry.find(effectType);
    
    if (it != registry.end()) {
        return it->second(std::move(logger));
    }
    
    if (logger) {
        logger->error("Unknown effect type: " + effectType);
    }
    
    return nullptr;
}

bool EffectFactory::isRegistered(const std::string& type) {
    return getRegistry().find(type) != getRegistry().end();
}
