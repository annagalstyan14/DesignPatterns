#pragma once
#include "ICommand.h"
#include "../AudioClip.h"
#include "../Effects/IEffect.h"
#include <memory>
#include <vector>

class ApplyEffectCommand : public ICommand {
public:
    ApplyEffectCommand(std::shared_ptr<AudioClip> clip, 
                       std::shared_ptr<IEffect> effect);
    
    void execute() override;
    void undo() override;
    const char* getName() const override;

private:
    std::shared_ptr<AudioClip> clip_;
    std::shared_ptr<IEffect> effect_;
    std::vector<float> beforeState_;
    std::vector<float> afterState_;
    bool executed_ = false;
};