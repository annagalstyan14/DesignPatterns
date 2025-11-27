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
            beforeState_ = clip_->getSamples();
            std::vector<float> samples = beforeState_;
            size_t dataSize = samples.size();
            
            for (auto& effect : effects_) {
                // Ensure buffer is large enough for speed changes
                if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
                    float factor = speed->getSpeedFactor();
                    size_t neededSize = static_cast<size_t>(dataSize / factor) + 1024;
                    if (neededSize > samples.size()) {
                        samples.resize(neededSize, 0.0f);
                    }
                }
                
                size_t newSize = effect->apply(samples.data(), dataSize);
                dataSize = newSize;
            }
            
            samples.resize(dataSize);
            clip_->setSamples(samples);
            afterState_ = samples;
            executed_ = true;
            
            if (logger_) {
                logger_->log("Effects applied and state saved for undo");
            }
        } else {
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