#pragma once

#include "ICommand.h"
#include "../Logging/ILogger.h"
#include <memory>
#include <vector>

class CommandHistory {
public:
    explicit CommandHistory(std::shared_ptr<ILogger> logger);

    void executeCommand(std::shared_ptr<ICommand> command);

    void undo();

    void redo();
  
    [[nodiscard]] bool canUndo() const noexcept;
    [[nodiscard]] bool canRedo() const noexcept;

    void clear() noexcept;
    [[nodiscard]] size_t size() const noexcept { return history_.size(); }
    
private:
    std::vector<std::shared_ptr<ICommand>> history_;
    std::shared_ptr<ILogger> logger_;
    int currentIndex_ = -1;
};
