#include "ApplyEffect.h"

ApplyEffectCommand::ApplyEffectCommand(std::shared_ptr<AudioClip> clip,
                                       std::shared_ptr<IEffect> effect,
                                       std::shared_ptr<ILogger> logger)
    : clip_(clip), effect_(effect), logger_(logger) {}

void ApplyEffectCommand::execute() {
    if (!executed_) {
        // Save state before applying effect
        beforeState_ = clip_->getSamples();
        
        clip_->addEffect(effect_);
        clip_->applyEffects();
        
        afterState_ = clip_->getSamples();
        executed_ = true;
        
        if (logger_) {
            logger_->log("Effect applied and state saved");
        }
    } else {
        // Re-apply from saved state
        clip_->setSamples(afterState_);
        if (logger_) {
            logger_->log("Effect re-applied from saved state");
        }
    }
}

void ApplyEffectCommand::undo() {
    clip_->setSamples(beforeState_);
    if (logger_) {
        logger_->log("Effect undone");
    }
}

const char* ApplyEffectCommand::getName() const {
    return "Apply Effect";
}