#ifndef EFFECT_STATE_COMMAND_H
#define EFFECT_STATE_COMMAND_H

#include "ICommand.h"
#include "../../GUI/EffectsPanel.h"
#include "../Logging/ILogger.h"
#include <memory>

/**
 * @brief Command for undoing/redoing effect state changes
 * 
 * Stores the before and after state of the effects panel,
 * allowing undo/redo of effect additions, removals, and parameter changes.
 */
class EffectStateCommand : public ICommand {
public:
    EffectStateCommand(EffectsPanel* panel, 
                       const EffectsPanelState& oldState,
                       const EffectsPanelState& newState,
                       std::shared_ptr<ILogger> logger)
        : panel_(panel)
        , oldState_(oldState)
        , newState_(newState)
        , logger_(logger)
    {
    }

    void execute() override {
        // Command is created after the change, so execute just logs
        logger_->log("EffectStateCommand: Effect state changed");
    }

    void undo() override {
        logger_->log("EffectStateCommand: Undoing effect state change");
        panel_->restoreState(oldState_);
    }

    void redo() override {
        logger_->log("EffectStateCommand: Redoing effect state change");
        panel_->restoreState(newState_);
    }

    std::string getDescription() const override {
        return "Effect Change";
    }

private:
    EffectsPanel* panel_;
    EffectsPanelState oldState_;
    EffectsPanelState newState_;
    std::shared_ptr<ILogger> logger_;
};

#endif // EFFECT_STATE_COMMAND_H