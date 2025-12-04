#include "CommandHistory.h"

CommandHistory::CommandHistory(std::shared_ptr<ILogger> logger) 
    : logger_(std::move(logger))
{
}

void CommandHistory::executeCommand(std::shared_ptr<ICommand> command) {
    if (!command) return;
    
    if (currentIndex_ < static_cast<int>(history_.size()) - 1) {
        history_.erase(history_.begin() + currentIndex_ + 1, history_.end());
    }
    
    command->execute();
    history_.push_back(std::move(command));
    ++currentIndex_;
    
    if (logger_) {
        logger_->log("Command executed, history size: " + std::to_string(history_.size()));
    }
}

void CommandHistory::undo() {
    if (!canUndo()) return;
    
    history_[currentIndex_]->undo();
    --currentIndex_;
    
    if (logger_) {
        logger_->log("Undo performed, position: " + std::to_string(currentIndex_));
    }
}

void CommandHistory::redo() {
    if (!canRedo()) return;
    
    ++currentIndex_;
    history_[currentIndex_]->redo();
    
    if (logger_) {
        logger_->log("Redo performed, position: " + std::to_string(currentIndex_));
    }
}

bool CommandHistory::canUndo() const noexcept {
    return currentIndex_ >= 0;
}

bool CommandHistory::canRedo() const noexcept {
    return currentIndex_ < static_cast<int>(history_.size()) - 1;
}

void CommandHistory::clear() noexcept {
    history_.clear();
    currentIndex_ = -1;
}
