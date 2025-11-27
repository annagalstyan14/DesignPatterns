#pragma once

#include <memory>
#include <string>
#include <map>
#include <functional>
#include "Effects/IEffect.h"
#include "Logging/ILogger.h"

/**
 * @brief Factory Pattern with Registry for creating effects
 * 
 * Uses self-registration pattern - effects register themselves
 * at startup, enabling creation by string name.
 * 
 * Design Pattern: Factory Method + Registry
 */
class EffectFactory {
public:
    using Creator = std::function<std::shared_ptr<IEffect>(std::shared_ptr<ILogger>)>;
    
    /**
     * @brief Register an effect creator function
     * 
     * @param type Effect type name (e.g., "Reverb", "Speed")
     * @param creator Factory function that creates the effect
     */
    static void registerEffect(const std::string& type, Creator creator);
    
    /**
     * @brief Create an effect by type name
     * 
     * @param effectType Registered effect type name
     * @param logger Logger to inject into the effect
     * @return Created effect, or nullptr if type not found
     */
    [[nodiscard]] static std::shared_ptr<IEffect> createEffect(
        const std::string& effectType, 
        std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Check if an effect type is registered
     */
    [[nodiscard]] static bool isRegistered(const std::string& type);
    
private:
    static std::map<std::string, Creator>& getRegistry();
};
