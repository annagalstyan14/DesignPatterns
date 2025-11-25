#include "CommandHistory.h"

CommandHistory::CommandHistory(std::shared_ptr<ILogger> logger) : logger_(logger) {}

void CommandHistory::executeCommand(std::shared_ptr<ICommand> command) {
    // Remove any commands after current index (redo history)
    if (currentIndex_ < static_cast<int>(history_.size()) - 1) {
        history_.erase(history_.begin() + currentIndex_ + 1, history_.end());
    }
    
    command->execute();
    history_.push_back(command);
    currentIndex_++;
    
    if (logger_) {
        logger_->log("Command executed");
    }
}

void CommandHistory::undo() {
    if (canUndo()) {
        history_[currentIndex_]->undo();
        currentIndex_--;
        if (logger_) {
            logger_->log("Undo performed");
        }
    }
}

void CommandHistory::redo() {
    if (canRedo()) {
        currentIndex_++;
        history_[currentIndex_]->execute();
        if (logger_) {
            logger_->log("Redo performed");
        }
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