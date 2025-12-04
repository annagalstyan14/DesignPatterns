#include "EffectStateCommand.h"
#include "../../GUI/EffectsPanel.h"

EffectStateCommand::EffectStateCommand(EffectsPanel* panel,
                                       EffectsPanelState oldState,
                                       EffectsPanelState newState,
                                       std::shared_ptr<ILogger> logger)
    : panel_(panel)
    , oldState_(std::move(oldState))
    , newState_(std::move(newState))
    , logger_(std::move(logger))
{
}

void EffectStateCommand::execute() {
    if (logger_) {
        logger_->log("EffectStateCommand: Effect state changed");
    }
}

void EffectStateCommand::undo() {
    if (logger_) {
        logger_->log("EffectStateCommand: Undoing effect state change");
    }
    
    if (panel_) {
        panel_->restoreState(oldState_);
    }
}

void EffectStateCommand::redo() {
    if (logger_) {
        logger_->log("EffectStateCommand: Redoing effect state change");
    }
    
    if (panel_) {
        panel_->restoreState(newState_);
    }
}
