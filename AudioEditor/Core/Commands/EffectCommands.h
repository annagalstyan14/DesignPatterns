#ifndef EFFECT_COMMANDS_H
#define EFFECT_COMMANDS_H

#include "ICommand.h"
#include "../AudioClip.h"
#include "../Effects/IEffect.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>
#include <string>

class MainWindow;  // Forward declare

class AddEffectCommand : public ICommand {
public:
    AddEffectCommand(MainWindow* mainWindow, std::shared_ptr<IEffect> effect, size_t insertIndex = -1);
    void execute() override;
    void undo() override;
    const char* getName() const override { return "Add Effect"; }

private:
    MainWindow* mainWindow_;
    std::shared_ptr<IEffect> effect_;
    size_t insertIndex_;
};

class RemoveEffectCommand : public ICommand {
public:
    RemoveEffectCommand(MainWindow* mainWindow, size_t index);
    void execute() override;
    void undo() override;
    const char* getName() const override { return "Remove Effect"; }

private:
    MainWindow* mainWindow_;
    std::shared_ptr<IEffect> removedEffect_;
    size_t index_;
};

class UpdateEffectParamCommand : public ICommand {
public:
    UpdateEffectParamCommand(MainWindow* mainWindow, size_t effectIndex, const std::string& paramName, float oldValue, float newValue);
    void execute() override;
    void undo() override;
    const char* getName() const override { return "Update Effect Parameter"; }

private:
    MainWindow* mainWindow_;
    size_t effectIndex_;
    std::string paramName_;
    float oldValue_;
    float newValue_;
};

#endif // EFFECT_COMMANDS_H