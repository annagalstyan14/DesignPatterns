#pragma once

#include "ICommand.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>
#include <string>

class CommandHistory {
public:
    explicit CommandHistory(std::shared_ptr<ILogger> logger);
    ~CommandHistory() = default;
    
    CommandHistory(const CommandHistory&) = delete;
    CommandHistory& operator=(const CommandHistory&) = delete;
    CommandHistory(CommandHistory&&) = default;
    CommandHistory& operator=(CommandHistory&&) = default;

    void executeCommand(std::shared_ptr<ICommand> command);
    void undo();
    void redo();
  
    [[nodiscard]] bool canUndo() const noexcept;
    [[nodiscard]] bool canRedo() const noexcept;

    void clear() noexcept;
    
    [[nodiscard]] size_t size() const noexcept { return history_.size(); }
    [[nodiscard]] int currentPosition() const noexcept { return currentIndex_; }
    
    [[nodiscard]] std::string getUndoDescription() const;
    [[nodiscard]] std::string getRedoDescription() const;
    
private:
    std::vector<std::shared_ptr<ICommand>> history_;
    std::shared_ptr<ILogger> logger_;
    int currentIndex_;
};
