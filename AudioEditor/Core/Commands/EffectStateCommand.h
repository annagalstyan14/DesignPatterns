#pragma once

#include "ICommand.h"
#include "EffectState.h"  // State structs defined separately to avoid circular deps
#include "../Logging/ILogger.h"
#include <memory>

// Forward declare EffectsPanel (avoid circular include with GUI)
class EffectsPanel;

/**
 * @brief Command for undoing/redoing effect state changes
 * 
 * Stores the before and after state of the effects panel,
 * allowing undo/redo of effect additions, removals, and parameter changes.
 * 
 * Design Pattern: Command (ConcreteCommand) + Memento (state snapshots)
 */
class EffectStateCommand : public ICommand {
public:
    EffectStateCommand(EffectsPanel* panel, 
                       EffectsPanelState oldState,
                       EffectsPanelState newState,
                       std::shared_ptr<ILogger> logger);

    void execute() override;
    void undo() override;
    void redo() override;
    
    [[nodiscard]] std::string getDescription() const override {
        return "Effect Change";
    }

private:
    EffectsPanel* panel_;
    EffectsPanelState oldState_;
    EffectsPanelState newState_;
    std::shared_ptr<ILogger> logger_;
};
