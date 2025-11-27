#include "EffectCommands.h"
#include "../../MainWindow.h"  // Adjust path as needed

AddEffectCommand::AddEffectCommand(MainWindow* mainWindow, std::shared_ptr<IEffect> effect, size_t insertIndex)
    : mainWindow_(mainWindow), effect_(effect), insertIndex_(insertIndex) {}

void AddEffectCommand::execute() {
    if (insertIndex_ == -1) insertIndex_ = mainWindow_->effectChain_.size();
    mainWindow_->effectChain_.insert(mainWindow_->effectChain_.begin() + insertIndex_, effect_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Effect added");
}

void AddEffectCommand::undo() {
    mainWindow_->effectChain_.erase(mainWindow_->effectChain_.begin() + insertIndex_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Effect addition undone");
}

RemoveEffectCommand::RemoveEffectCommand(MainWindow* mainWindow, size_t index)
    : mainWindow_(mainWindow), index_(index) {}

void RemoveEffectCommand::execute() {
    removedEffect_ = mainWindow_->effectChain_[index_];
    mainWindow_->effectChain_.erase(mainWindow_->effectChain_.begin() + index_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Effect removed");
}

void RemoveEffectCommand::undo() {
    mainWindow_->effectChain_.insert(mainWindow_->effectChain_.begin() + index_, removedEffect_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Effect removal undone");
}

UpdateEffectParamCommand::UpdateEffectParamCommand(MainWindow* mainWindow, size_t effectIndex, const std::string& paramName, float oldValue, float newValue)
    : mainWindow_(mainWindow), effectIndex_(effectIndex), paramName_(paramName), oldValue_(oldValue), newValue_(newValue) {}

void UpdateEffectParamCommand::execute() {
    auto effect = mainWindow_->effectChain_[effectIndex_];
    effect->setParameter(paramName_, newValue_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Parameter updated to " + std::to_string(newValue_));
}

void UpdateEffectParamCommand::undo() {
    auto effect = mainWindow_->effectChain_[effectIndex_];
    effect->setParameter(paramName_, oldValue_);
    mainWindow_->updatePreview();
    mainWindow_->logger_->log("Parameter undone to " + std::to_string(oldValue_));
}