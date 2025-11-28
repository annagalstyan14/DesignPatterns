#pragma once

#include "ICommand.h"
#include "../AudioClip.h"
#include "../Effects/IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>

/**
 * @brief Concrete Command for applying audio effects
 * 
 * Stores before/after state for complete undo/redo support.
 * Can apply single or multiple effects in sequence.
 * 
 * Design Pattern: Command (ConcreteCommand)
 */
class ApplyEffectCommand : public ICommand {
public:
    /**
     * @brief Construct command for single effect
     */
    ApplyEffectCommand(std::shared_ptr<AudioClip> clip, 
                       std::shared_ptr<IEffect> effect,
                       std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Construct command for multiple effects
     */
    ApplyEffectCommand(std::shared_ptr<AudioClip> clip, 
                       std::vector<std::shared_ptr<IEffect>> effects,
                       std::shared_ptr<ILogger> logger);
    
    void execute() override;
    void undo() override;
    
    [[nodiscard]] std::string getDescription() const override { 
        return "Apply Effect"; 
    }

private:
    std::shared_ptr<AudioClip> clip_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    std::shared_ptr<ILogger> logger_;
    std::vector<float> beforeState_;
    std::vector<float> afterState_;
    bool executed_ = false;
};
