#include "CommandHistory.h"
#include "../Logger.h"

void CommandHistory::executeCommand(std::shared_ptr<ICommand> command) {
    // Remove any commands after current index (redo history)
    if (currentIndex_ < static_cast<int>(history_.size()) - 1) {
        history_.erase(history_.begin() + currentIndex_ + 1, history_.end());
    }
    
    command->execute();
    history_.push_back(command);
    currentIndex_++;
    
    Logger::getInstance().log("Command executed: " + std::string(command->getName()));
}

void CommandHistory::undo() {
    if (canUndo()) {
        history_[currentIndex_]->undo();
        currentIndex_--;
        Logger::getInstance().log("Undo performed");
    }
}

void CommandHistory::redo() {
    if (canRedo()) {
        currentIndex_++;
        history_[currentIndex_]->execute();
        Logger::getInstance().log("Redo performed");
    }
}

bool CommandHistory::canUndo() const {
    return currentIndex_ >= 0;
}

bool CommandHistory::canRedo() const {
    return currentIndex_ < static_cast<int>(history_.size()) - 1;
}

void CommandHistory::clear() {
    history_.clear();
    currentIndex_ = -1;
}