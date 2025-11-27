#pragma once

#include "ICommand.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>

/**
 * @brief Command Pattern Invoker - manages undo/redo stack
 * 
 * Maintains history of executed commands and provides
 * undo/redo navigation through that history.
 * 
 * Design Pattern: Command (Invoker)
 */
class CommandHistory {
public:
    explicit CommandHistory(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Execute a command and add it to history
     * 
     * Clears any redo history (commands after current position).
     */
    void executeCommand(std::shared_ptr<ICommand> command);
    
    /**
     * @brief Undo the most recent command
     */
    void undo();
    
    /**
     * @brief Redo the most recently undone command
     */
    void redo();
    
    /**
     * @brief Check if undo is available
     */
    [[nodiscard]] bool canUndo() const noexcept;
    
    /**
     * @brief Check if redo is available
     */
    [[nodiscard]] bool canRedo() const noexcept;
    
    /**
     * @brief Clear all command history
     */
    void clear() noexcept;
    
    /**
     * @brief Get number of commands in history
     */
    [[nodiscard]] size_t size() const noexcept { return history_.size(); }
    
private:
    std::vector<std::shared_ptr<ICommand>> history_;
    std::shared_ptr<ILogger> logger_;
    int currentIndex_ = -1;
};
