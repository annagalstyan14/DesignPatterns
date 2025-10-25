#pragma once
#include <memory>
#include <string>
#include <map>
#include <functional>
#include "Effects/IEffect.h"

class EffectFactory {
private:
    static std::map<std::string, std::function<std::shared_ptr<IEffect>()>> registry;
public:
    static void registerEffect(const std::string& type, std::function<std::shared_ptr<IEffect>()> creator);
    static std::shared_ptr<IEffect> createEffect(const std::string& effectType);
};