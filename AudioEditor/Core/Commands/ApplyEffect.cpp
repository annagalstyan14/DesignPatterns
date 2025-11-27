#include "ApplyEffect.h"
#include "../Effects/Reverb.h"
#include "../Effects/Speed.h"

ApplyEffectCommand::ApplyEffectCommand(std::shared_ptr<AudioClip> clip,
                                       std::shared_ptr<IEffect> effect,
                                       std::shared_ptr<ILogger> logger)
    : clip_(clip), logger_(logger) {
    effects_.push_back(effect);
}

ApplyEffectCommand::ApplyEffectCommand(std::shared_ptr<AudioClip> clip,
                                       std::vector<std::shared_ptr<IEffect>> effects,
                                       std::shared_ptr<ILogger> logger)
    : clip_(clip), effects_(effects), logger_(logger) {}

void ApplyEffectCommand::execute() {
    if (!executed_) {
        // Save state before applying effects
        beforeState_ = clip_->getSamples();
        
        // Apply effects directly to samples (not through AudioClip::addEffect)
        std::vector<float> samples = beforeState_;
        
        for (auto& effect : effects_) {
            // Handle speed effect which changes buffer size
            if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
                float factor = speed->getSpeedFactor();
                size_t newSize = static_cast<size_t>(samples.size() / factor);
                if (newSize > samples.size()) {
                    samples.resize(newSize);
                }
            }
            
            size_t newSize = effect->apply(samples.data(), samples.size());
            samples.resize(newSize);
        }
        
        clip_->setSamples(samples);
        afterState_ = samples;
        executed_ = true;
        
        if (logger_) {
            logger_->log("Effects applied and state saved for undo");
        }
    } else {
        // Re-execute (redo)
        clip_->setSamples(afterState_);
        if (logger_) {
            logger_->log("Effects re-applied (redo)");
        }
    }
}

void ApplyEffectCommand::undo() {
    clip_->setSamples(beforeState_);
    if (logger_) {
        logger_->log("Effects undone - restored " + std::to_string(beforeState_.size()) + " samples");
    }
}

const char* ApplyEffectCommand::getName() const {
    return "Apply Effect";
}