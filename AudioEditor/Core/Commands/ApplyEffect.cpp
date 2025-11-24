#include "ApplyEffectCommand.h"
#include "../Logger.h"

ApplyEffectCommand::ApplyEffectCommand(std::shared_ptr<AudioClip> clip,
                                       std::shared_ptr<IEffect> effect)
    : clip_(clip), effect_(effect) {}

void ApplyEffectCommand::execute() {
    if (!executed_) {
        // Save state before applying effect
        beforeState_ = clip_->getSamples();
        
        clip_->addEffect(effect_);
        clip_->applyEffects();
        
        afterState_ = clip_->getSamples();
        executed_ = true;
        
        Logger::getInstance().log("Effect applied and state saved");
    } else {
        // Re-apply from saved state
        clip_->setSamples(afterState_);
        Logger::getInstance().log("Effect re-applied from saved state");
    }
}

void ApplyEffectCommand::undo() {
    clip_->setSamples(beforeState_);
    Logger::getInstance().log("Effect undone");
}

const char* ApplyEffectCommand::getName() const {
    return "Apply Effect";
}