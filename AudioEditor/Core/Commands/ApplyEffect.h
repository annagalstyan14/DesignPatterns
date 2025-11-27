#pragma once
#include "ICommand.h"
#include "../AudioClip.h"
#include "../Effects/IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>

class ApplyEffectCommand : public ICommand {
public:
    // Single effect constructor
    ApplyEffectCommand(std::shared_ptr<AudioClip> clip, 
                       std::shared_ptr<IEffect> effect,
                       std::shared_ptr<ILogger> logger);
    
    // Multiple effects constructor
    ApplyEffectCommand(std::shared_ptr<AudioClip> clip, 
                       std::vector<std::shared_ptr<IEffect>> effects,
                       std::shared_ptr<ILogger> logger);
    
    void execute() override;
    void undo() override;
    const char* getName() const override;

private:
    std::shared_ptr<AudioClip> clip_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    std::shared_ptr<ILogger> logger_;
    std::vector<float> beforeState_;
    std::vector<float> afterState_;
    bool executed_ = false;
};