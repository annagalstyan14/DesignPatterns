#include "EffectFactory.h"
#include "Logger.h"

std::map<std::string, std::function<std::shared_ptr<IEffect>()>> EffectFactory::registry;

void EffectFactory::registerEffect(const std::string& type, std::function<std::shared_ptr<IEffect>()> creator) {
    registry[type] = creator;
}

std::shared_ptr<IEffect> EffectFactory::createEffect(const std::string& effectType) {
    auto it = registry.find(effectType);
    if (it != registry.end()) return it->second();
    Logger::getInstance().log("Unknown effect type: " + effectType);
    return nullptr;
}